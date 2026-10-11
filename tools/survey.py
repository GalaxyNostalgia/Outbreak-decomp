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
    """crt0 sets $gp with `lui $gp, hi` + `addiu|ori $gp, $gp, lo` near the entry point."""
    sym = find_symbol(d, elf, b"_gp")
    if sym is not None:
        return sym, "symbol _gp"
    off = vram_to_off(elf, elf["entry"])
    if off is None:
        return None, "entry point not in a load segment"
    hi = None
    for i in range(64):
        ins = u32(d, off + 4 * i)
        op, rs, rt, imm = ins >> 26, (ins >> 21) & 31, (ins >> 16) & 31, ins & 0xFFFF
        if op == 0x0F and rt == 28:                  # lui $gp
            hi = imm << 16
        elif hi is not None and rs == 28 and rt == 28 and op in (0x09, 0x0D):
            lo = imm - 0x10000 if (op == 0x09 and imm & 0x8000) else imm
            return (hi + lo) & 0xFFFFFFFF if op == 0x09 else hi | imm, \
                f"crt0 at entry+{4 * i:#x}"
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
    c["mw_markers"] = len(re.findall(r"Metrowerks|CodeWarrior|mwcc|mwld|MWERKS", blob))
    c["sn_markers"] = len(re.findall(r"SN Systems|ProDG|SN BUILD|snDebug|__SN_", blob))
    c["mw_mangling"] = len(re.findall(r"__(?:ct|dt)__\w+", blob)) + len(re.findall(r"__RTTI__", blob))
    c["gcc29_mangling"] = len(re.findall(r"_\$_\w+|__tf\d+\w+|__vt\.\w+|__vt_\d+\w+", blob))
    sections = {s["name"] for s in elf["shdrs"]}
    c["mw_sections"] = len(sections & {".mwcats", ".vtables", ".exceptix", ".init_array"})
    sdk = sorted(set(re.findall(r"PsII\w+\s+\d{3,4}", blob)))
    cri = sorted(set(re.findall(r"CRI [\w/ .\-]{2,40}?Ver\.? ?[\d.]+\w*", blob)))
    irx = sorted(set(m.upper() for m in re.findall(r"[\w\-]+\.(?:IRX|irx)", blob)))
    net = sorted(set(re.findall(r"[\w.\-]*(?:dnas|DNAS|kddi|KDDI|playstation\.org|capcom)[\w.\-/]*", blob)))[:60]
    return dict(gcc_comments=gcc, counters=dict(c), sdk_versions=sdk, cri=cri, iop_modules=irx,
                network_strings=net)


def guess_compiler(fp):
    c = fp["counters"]
    votes = []
    if c["mw_markers"] or c["mw_sections"] or c["mw_mangling"] > c["gcc29_mangling"]:
        votes.append("Metrowerks CodeWarrior for PS2 (mwccps2)")
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


def yaml_for(ver, elfname, d, elf, gp, sha):
    lines = [
        f"# {ver['title']} ({ver['serial']}), generated by tools/survey.py.",
        "# Section boundaries are exact; translation-unit boundaries are refined by hand.",
        f"name: {ver['title']}",
        f"sha1: {sha}",
        "options:",
        f"  basename: {elfname}",
        f"  target_path: orig/{elfname}/{elfname}",
        f"  elf_path: build/{elfname}/{elfname}",
        "  base_path: ../..",
        "  platform: ps2",
        "  compiler: EEGCC            # set to MWCCPS2 once docs/COMPILER.md pins it",
    ]
    if gp is not None:
        lines.append(f"  gp_value: {gp:#X}".replace("0X", "0x"))
    lines += [
        f"  asm_path: asm/{elfname}",
        f"  asset_path: assets/{elfname}",
        "  src_path: src",
        f"  build_path: build/{elfname}",
        f"  ld_script_path: build/{elfname}/splat.ld",
        "  symbol_addrs_path:",
        f"    - config/{elfname}/symbol_addrs.txt",
        f"  undefined_funcs_auto_path: build/{elfname}/undefined_funcs_auto.txt",
        f"  undefined_syms_auto_path: build/{elfname}/undefined_syms_auto.txt",
        "  o_as_suffix: True",
        "  use_legacy_include_asm: False",
        "  migrate_rodata_to_functions: False",
        "  string_encoding: EUC-JP",
        "  rodata_string_guesser_level: 2",
        "  data_string_guesser_level: 2",
        "  named_regs_for_c_funcs: False",
        "  ld_bss_is_noload: True",
        "",
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
        for k in ("gcc_comments", "sdk_versions", "cri", "iop_modules", "network_strings"):
            vals = fp[k]
            print(f"{k} ({len(vals)}):", *(f"\n  {v}" for v in vals[:40]))
    if a.write:
        versions = json.loads((ROOT / "config/versions.json").read_text())
        ver = versions[a.write]
        if ver["elf"] != a.elf.name:
            sys.exit(f"{a.elf.name} is not the executable of {a.write} ({ver['elf']})")
        cdir = ROOT / "config" / ver["elf"]
        cdir.mkdir(parents=True, exist_ok=True)
        y = cdir / "splat.yaml"
        if y.exists():
            print(f"{y} exists, not overwriting")
        else:
            y.write_text(yaml_for(ver, ver["elf"], d, elf, gp, sha))
            print(f"wrote {y.relative_to(ROOT)}")
        s = cdir / "symbol_addrs.txt"
        if not s.exists():
            s.write_text(f"_start = {elf['entry']:#010x}; // type:func\n")
            print(f"wrote {s.relative_to(ROOT)}")
        (cdir / "survey.json").write_text(json.dumps(
            {k: report[k] for k in ("size", "sha1", "entry", "gp", "gp_source", "compiler_guess",
                                     "counters", "gcc_comments", "sdk_versions", "cri", "loads", "sections")},
            indent=1) + "\n")


if __name__ == "__main__":
    main()
