#!/usr/bin/env python3
"""Survey a PS2 executable and (optionally) write its first splat config.

    python -I tools/survey.py orig/SLPM_656.92/SLPM_656.92            # report only
    python -I tools/survey.py orig/SLPM_656.92/SLPM_656.92 --write f2 # + config/SLPM_656.92/

What it reports:
  - ELF program headers and section headers (file offsets, vram, sizes)
  - the $gp value, read from crt0 (lui/addiu $gp at the entry point) or from _gp
  - compiler fingerprints: GCC .comment strings, Metrowerks / SN Systems markers,
    C++ mangling style (MW `__ct__`/`__dt__` vs GCC 2.9x `_$_`/`__tf`)
  - Sony SDK library version strings, CRI middleware versions, IOP module names,
    and network strings (DNAS, KDDI hosts) that matter for the Obsrv work

With --write it creates config/<ELF>/splat.yaml (one subsegment per ELF section,
in file order) and config/<ELF>/symbol_addrs.txt. Both are a starting point:
section boundaries are real, translation-unit boundaries come later.

Only facts about the binary are written (offsets, sizes, hashes); nothing from
the game itself ends up in config/.
"""
import argparse
import hashlib
import json
import re
import struct
import sys
from collections import Counter
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

PT_LOAD = 1
SHT_NOBITS = 8
SHF_ALLOC = 0x2
SHF_EXECINSTR = 0x4


def u32(d, o):
    return struct.unpack_from("<I", d, o)[0]


def parse_elf(d):
    if d[:4] != b"\x7fELF":
        sys.exit("not an ELF file")
    if d[4] != 1 or d[5] != 1:
        sys.exit("expected a 32-bit little-endian ELF (PS2 EE)")
    machine = struct.unpack_from("<H", d, 0x12)[0]
    entry, phoff, shoff = struct.unpack_from("<III", d, 0x18)
    phentsize, phnum, shentsize, shnum, shstrndx = struct.unpack_from("<HHHHH", d, 0x2A)
    phdrs = []
    for i in range(phnum):
        p_type, off, vaddr, paddr, filesz, memsz, flags, align = struct.unpack_from(
            "<8I", d, phoff + i * phentsize)
        phdrs.append(dict(type=p_type, offset=off, vaddr=vaddr, filesz=filesz,
                          memsz=memsz, flags=flags, align=align))
    shdrs = []
    if shoff and shnum:
        raw = [struct.unpack_from("<10I", d, shoff + i * shentsize) for i in range(shnum)]
        stro = raw[shstrndx][4] if shstrndx < shnum else 0
        for r in raw:
            name = d[stro + r[0]:d.index(b"\0", stro + r[0])].decode("ascii", "replace") if stro else ""
            shdrs.append(dict(name=name, type=r[1], flags=r[2], addr=r[3], offset=r[4],
                              size=r[5], link=r[6], info=r[7], align=r[8], entsize=r[9]))
    return dict(machine=machine, entry=entry, phoff=phoff, shoff=shoff, phdrs=phdrs, shdrs=shdrs)


def find_symbol(d, elf, wanted):
    for s in elf["shdrs"]:
        if s["type"] != 2:  # SHT_SYMTAB
            continue
        strtab = elf["shdrs"][s["link"]]
        for k in range(s["size"] // 16):
            o = s["offset"] + k * 16
            name_off, value = struct.unpack_from("<II", d, o)
            no = strtab["offset"] + name_off
            if d[no:d.index(b"\0", no)] == wanted:
                return value
    return None


def vram_to_off(elf, addr):
    for p in elf["phdrs"]:
        if p["type"] == PT_LOAD and p["vaddr"] <= addr < p["vaddr"] + p["filesz"]:
            return p["offset"] + addr - p["vaddr"]
    return None


def find_gp(d, elf):
    """$gp as crt0 sets it: either `lui/addiu $gp` directly, or (Sony's crt0) a constant
    built in another register with lui/addiu/ori and then `move $gp, reg`."""
    sym = find_symbol(d, elf, b"_gp")
    if sym is not None:
        return sym, "symbol _gp"
    off = vram_to_off(elf, elf["entry"])
    if off is None:
        return None, "entry point not in a load segment"
    regs = {0: 0}
    for i in range(1024):          # Sony crt0 clears every register first
        ins = u32(d, off + 4 * i)
        op, rs, rt, rd, imm = ins >> 26, (ins >> 21) & 31, (ins >> 16) & 31, (ins >> 11) & 31, ins & 0xFFFF
        simm = imm - 0x10000 if imm & 0x8000 else imm
        if op == 0x0F:                                   # lui
            regs[rt] = (imm << 16) & 0xFFFFFFFF
        elif op in (0x09, 0x19) and rs in regs:          # addiu / daddiu
            regs[rt] = (regs[rs] + simm) & 0xFFFFFFFF
        elif op == 0x0D and rs in regs:                  # ori
            regs[rt] = regs[rs] | imm
        elif op == 0 and (ins & 0x3F) in (0x21, 0x2D, 0x25) and rt == 0 and rs in regs:  # move rd, rs
            regs[rd] = regs[rs]
        else:
            for r in (rt, rd):
                if r != 28:
                    regs.pop(r, None)
            continue
        if regs.get(28):
            return regs[28], f"crt0 at entry+{4 * i:#x}"
    return None, "not found near entry (set gp_value by hand)"


STRINGS = re.compile(rb"[\x20-\x7e]{4,}")


def fingerprints(d, elf):
    comment = b""
    for s in elf["shdrs"]:
        if s["name"] == ".comment":
            comment = d[s["offset"]:s["offset"] + s["size"]]
    strs = [m.group().decode("ascii") for m in STRINGS.finditer(d)]
    blob = "\n".join(strs)
    c = Counter()
    gcc = sorted(set(re.findall(r"GCC: \([^)]*\) [\w.\-]+(?: [^\n\x00]{0,40})?",
                                comment.decode("latin1") + "\n" + blob)))
    mw = sorted(set(re.findall(r"MW MIPS C Compiler \([\d.]+\)", comment.decode("latin1") + "\n" + blob)))
    c["mw_markers"] = len(mw) + len(re.findall(r"Metrowerks|CodeWarrior|mwcc|mwld|MWERKS", blob))
    c["sn_markers"] = len(re.findall(r"SN Systems|ProDG|SN BUILD|snDebug|__SN_", blob))
    c["mw_mangling"] = len(re.findall(r"__(?:ct|dt)__\w+", blob)) + len(re.findall(r"__RTTI__", blob))
    c["gcc29_mangling"] = len(re.findall(r"_\$_\w+|__tf\d+\w+|__vt\.\w+|__vt_\d+\w+", blob))
    sections = {s["name"] for s in elf["shdrs"]}
    c["mw_sections"] = len(sections & {".mwcats", ".vtables", ".exceptix", ".init_array"})
    sdk = sorted(set(re.findall(r"PsII\w+\s+\d{3,4}", blob)))
    cri = sorted(set(re.findall(r"CRI [\w/ .\-]{2,40}?Ver\.? ?[\d.]+\w*", blob)))
    irx = sorted(set(m.upper() for m in re.findall(r"[\w\-]+\.(?:IRX|irx)", blob)))
    net = sorted(set(re.findall(r"[\w.\-]*(?:dnas|DNAS|kddi|KDDI|playstation\.org|capcom)[\w.\-/]*", blob)))[:60]
    return dict(gcc_comments=gcc, mw_comments=mw, counters=dict(c), sdk_versions=sdk, cri=cri, iop_modules=irx,
                network_strings=net)


def guess_compiler(fp):
    c = fp["counters"]
    votes = []
    if c["mw_markers"] or c["mw_sections"] or c["mw_mangling"] > c["gcc29_mangling"]:
        votes.append("Metrowerks CodeWarrior for PS2 (mwccps2)" + (f": {', '.join(fp['mw_comments'])}" if fp["mw_comments"] else ""))
    if c["sn_markers"]:
        votes.append("SN Systems ProDG (ee-gcc 2.9x 'SN BUILD')")
    if fp["gcc_comments"] or c["gcc29_mangling"]:
        votes.append("ee-gcc (at least the Sony/CRI libraries)")
    if not votes:
        votes.append("unknown: no markers; decide by test-compiling small functions (docs/COMPILER.md)")
    return votes


def subsegments(d, elf, load):
    """Section-accurate subsegments for one PT_LOAD, in file order."""
    lo, hi = load["offset"], load["offset"] + load["filesz"]
    secs = [s for s in elf["shdrs"] if s["flags"] & SHF_ALLOC and s["size"]
            and s["type"] != SHT_NOBITS and lo <= s["offset"] < hi]
    secs.sort(key=lambda s: s["offset"])
    out = []
    if not secs:
        kind = "asm" if load["flags"] & 1 else "data"
        out.append(dict(start=lo, type=kind, name=f"seg_{load['vaddr']:X}", section="(no section headers)"))
    for s in secs:
        n = s["name"].lstrip(".") or f"sec_{s['addr']:X}"
        n = re.sub(r"\W", "_", n)
        if "vu" in s["name"].lower():
            kind = "data"          # VU microcode / VU data: never disassemble as EE code
        elif s["flags"] & SHF_EXECINSTR:
            kind = "asm"
        elif s["name"] in (".rodata",):
            kind = "rodata"
        else:
            kind = "data"
        if out and s["offset"] < out[-1]["start"]:
            continue
        out.append(dict(start=s["offset"], type=kind, name=n, section=s["name"]))
    bss = [s for s in elf["shdrs"] if s["type"] == SHT_NOBITS and s["flags"] & SHF_ALLOC and s["size"]
           and load["vaddr"] <= s["addr"] < load["vaddr"] + load["memsz"]]
    bss.sort(key=lambda s: s["addr"])
    if not bss and load["memsz"] > load["filesz"]:
        bss = [dict(name=".bss", addr=load["vaddr"] + load["filesz"], size=load["memsz"] - load["filesz"])]
    return out, bss


def yaml_header(ver, elfname, target, target_path, gp, sha, symbol_files, compiler):
    return [
        f"# {ver['title']} ({ver['serial']}) - {target}, generated by tools/survey.py.",
        "# Section boundaries come from the binary; translation-unit boundaries are refined by hand.",
        f"name: {ver['title']} - {target}",
        f"sha1: {sha}",
        "options:",
        f"  basename: {elfname}.{target}",
        f"  target_path: {target_path}",
        "  base_path: ../..",
        "  platform: ps2",
        f"  compiler: {compiler}",
    ] + ([f"  gp_value: {gp:#x}"] if gp is not None else []) + [
        f"  asm_path: asm/{elfname}/{target}",
        f"  asset_path: assets/{elfname}/{target}",
        "  src_path: src",
        f"  build_path: build/{elfname}/{target}",
        f"  ld_script_path: build/{elfname}/{target}/splat.ld",
        "  symbol_addrs_path:",
    ] + [f"    - {f}" for f in symbol_files] + [
        f"  undefined_funcs_auto_path: build/{elfname}/{target}/undefined_funcs_auto.txt",
        f"  undefined_syms_auto_path: build/{elfname}/{target}/undefined_syms_auto.txt",
        "  o_as_suffix: True",
        "  use_legacy_include_asm: False",
        "  migrate_rodata_to_functions: False",
        "  string_encoding: EUC-JP",
        "  rodata_string_guesser_level: 2",
        "  data_string_guesser_level: 2",
        "  named_regs_for_c_funcs: False",
        "  ld_bss_is_noload: True",
        "",
    ]


def yaml_for(ver, elfname, d, elf, gp, sha, compiler):
    lines = yaml_header(ver, elfname, "main", f"orig/{elfname}/{elfname}", gp, sha,
                        [f"config/{elfname}/symbol_addrs.txt"], compiler) + [
        "segments:",
        "  - name: elf_header",
        "    type: databin",
        "    start: 0x0",
    ]
    loads = [p for p in elf["phdrs"] if p["type"] == PT_LOAD and p["filesz"]]
    end = 0
    for i, p in enumerate(loads):
        subs, bss = subsegments(d, elf, p)
        lines += [
            "",
            f"  - name: load{i}",
            "    type: code",
            f"    start: {p['offset']:#x}",
            f"    vram: {p['vaddr']:#x}",
            f"    bss_size: {p['memsz'] - p['filesz']:#x}",
            "    subsegments:",
        ]
        for s in subs:
            lines.append(f"      - [{s['start']:#x}, {s['type']}, {s['name']}]   # {s['section']}")
        if bss:
            # one bss subsegment covers every NOBITS section of the load (.sbss, .bss, ...)
            names = ", ".join(f"{s['name']} {s['addr']:#x}+{s['size']:#x}" for s in bss)
            lines.append(f"      - {{ start: {p['offset'] + p['filesz']:#x}, type: bss, name: bss{i}, "
                         f"vram: {bss[0]['addr']:#x} }}   # {names}")
        end = max(end, p["offset"] + p["filesz"])
    lines += ["", "  - name: elf_tail", "    type: databin", f"    start: {end:#x}", f"  - [{len(d):#x}]", ""]
    return "\n".join(lines)


MWO3 = struct.Struct("<4sIIIIIII32s")


def parse_mwo3(d):
    """Metrowerks PS2 overlay: 0x40-byte header, loaded verbatim at `load`, then .text, .data."""
    magic, ovl_id, load, text, data, bss, sinit0, sinit1, name = MWO3.unpack_from(d, 0)
    if magic != b"MWo3":
        sys.exit("not a Metrowerks overlay (MWo3)")
    if 0x40 + text + data != len(d):
        sys.exit(f"overlay sizes don't add up: 0x40 + {text:#x} + {data:#x} != {len(d):#x}")
    return dict(id=ovl_id, load=load, text=text, data=data, bss=bss, sinit=(sinit0, sinit1),
                name=name.split(b"\0")[0].decode("ascii", "replace"))


def overlay_yaml(ver, elfname, target, path, d, gp, sha, compiler):
    h = parse_mwo3(d)
    lines = yaml_header(ver, elfname, target, path, gp, sha,
                        [f"config/{elfname}/symbol_addrs.txt", f"config/{elfname}/{target}.symbol_addrs.txt"],
                        compiler)
    t0, d0, end = 0x40, 0x40 + h["text"], 0x40 + h["text"] + h["data"]
    lines += [
        f"# MWo3 overlay id {h['id']} '{h['name']}': load {h['load']:#x}, text {h['text']:#x}, "
        f"data {h['data']:#x}, bss {h['bss']:#x}, sinit {h['sinit'][0]:#x}-{h['sinit'][1]:#x}",
        "segments:",
        "  - name: header",
        "    type: databin",
        "    start: 0x0",
        "",
        "  - name: load0",
        "    type: code",
        f"    start: {t0:#x}",
        f"    vram: {h['load'] + t0:#x}",
        f"    bss_size: {h['bss']:#x}",
        "    subsegments:",
        f"      - [{t0:#x}, asm, text]",
        f"      - [{d0:#x}, data, data]",
    ]
    if h["bss"]:
        lines.append(f"      - {{ start: {end:#x}, type: bss, name: bss0, vram: {h['load'] + end:#x} }}")
    lines += ["", f"  - [{end:#x}]", ""]
    return "\n".join(lines), h


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("elf", type=Path)
    ap.add_argument("--write", metavar="VERSION", help="write config/<ELF>/ for this version key (f1, f2)")
    ap.add_argument("--json", action="store_true", help="print the report as JSON")
    a = ap.parse_args()
    d = a.elf.read_bytes()
    elf = parse_elf(d)
    sha = hashlib.sha1(d).hexdigest()
    gp, gp_how = find_gp(d, elf)
    fp = fingerprints(d, elf)
    report = dict(file=str(a.elf), size=len(d), sha1=sha, entry=hex(elf["entry"]),
                  gp=hex(gp) if gp is not None else None, gp_source=gp_how,
                  compiler_guess=guess_compiler(fp), **fp,
                  loads=[{k: (hex(v) if isinstance(v, int) else v) for k, v in p.items()}
                         for p in elf["phdrs"]],
                  sections=[dict(name=s["name"], offset=hex(s["offset"]), addr=hex(s["addr"]),
                                 size=hex(s["size"]), flags=hex(s["flags"]), nobits=s["type"] == SHT_NOBITS)
                            for s in elf["shdrs"] if s["name"]])
    if a.json:
        print(json.dumps(report, indent=1))
    else:
        print(f"{a.elf.name}: {len(d):,} bytes  sha1 {sha}")
        print(f"entry {elf['entry']:#x}   gp {report['gp']} ({gp_how})")
        print("load segments:")
        for p in elf["phdrs"]:
            print(f"  type {p['type']}  off {p['offset']:#8x}  vram {p['vaddr']:#9x}  "
                  f"filesz {p['filesz']:#9x}  memsz {p['memsz']:#9x}  flags {p['flags']:#x}")
        print("sections:" if elf["shdrs"] else "sections: none (stripped section headers)")
        for s in elf["shdrs"]:
            if s["name"]:
                print(f"  {s['name']:<18} off {s['offset']:#8x}  addr {s['addr']:#9x}  size {s['size']:#9x}"
                      f"{'  NOBITS' if s['type'] == SHT_NOBITS else ''}")
        print("compiler guess:", *(f"\n  - {v}" for v in report["compiler_guess"]))
        print("  counters:", fp["counters"])
        for k in ("mw_comments", "gcc_comments", "sdk_versions", "cri", "iop_modules", "network_strings"):
            vals = fp[k]
            print(f"{k} ({len(vals)}):", *(f"\n  {v}" for v in vals[:40]))
    if a.write:
        versions = json.loads((ROOT / "config/versions.json").read_text())
        ver = versions[a.write]
        if ver["elf"] != a.elf.name:
            sys.exit(f"{a.elf.name} is not the executable of {a.write} ({ver['elf']})")
        cdir = ROOT / "config" / ver["elf"]
        cdir.mkdir(parents=True, exist_ok=True)
        compiler = "MWCCPS2" if fp["mw_comments"] or fp["counters"]["mw_markers"] else "EEGCC"

        def write_once(path, text):
            if path.exists():
                print(f"{path.relative_to(ROOT)} exists, not overwriting")
            else:
                path.write_text(text)
                print(f"wrote {path.relative_to(ROOT)}")

        write_once(cdir / "main.yaml", yaml_for(ver, ver["elf"], d, elf, gp, sha, compiler))
        write_once(cdir / "symbol_addrs.txt", f"_start = {elf['entry']:#010x}; // type:func\n")
        overlays = {}
        for name, rel in ver.get("overlays", {}).items():
            op = a.elf.parent / rel
            if not op.exists():
                print(f"overlay {name}: {op} missing, skipped")
                continue
            od = op.read_bytes()
            osha = hashlib.sha1(od).hexdigest()
            text, h = overlay_yaml(ver, ver["elf"], name, f"orig/{ver['elf']}/{rel}", od, gp, osha, compiler)
            write_once(cdir / f"{name}.yaml", text)
            write_once(cdir / f"{name}.symbol_addrs.txt", f"// symbols that live in the {name} overlay\n")
            overlays[name] = dict(path=rel, size=len(od), sha1=osha, **{k: (hex(v) if isinstance(v, int) and k != "id" else v)
                                                                        for k, v in h.items() if k != "sinit"})
        (cdir / "survey.json").write_text(json.dumps(
            {**{k: report[k] for k in ("size", "sha1", "entry", "gp", "gp_source", "compiler_guess",
                                       "counters", "mw_comments", "gcc_comments", "sdk_versions", "cri", "loads", "sections")},
             "overlays": overlays}, indent=1) + "\n")


if __name__ == "__main__":
    main()
