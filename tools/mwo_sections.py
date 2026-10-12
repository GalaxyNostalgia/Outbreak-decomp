#!/usr/bin/env python3
"""Rename the per-function sections of a Metrowerks PS2 object so GNU ld can place
functions individually: `.text` -> `.text.<func>` (and `.rel.text` -> `.rel.text.<func>`), and a
function's jump tables `.rodata` -> `.rodata.<func>`.

MW already emits one `.text` section per function; it just names them all `.text`.
The new names go into a copy of .shstrtab appended to the file; no contents move.
Also turns MW's processor-specific symbol binding 13 (out-of-line inline copies)
into STB_WEAK so GNU ld accepts it.

Adapted from dmc3-decomp's tools/mwo_sections.py (CC0).

    python -I tools/mwo_sections.py in.o out.o
"""
import struct
import sys

SHT_SYMTAB, SHT_REL, STT_FUNC = 2, 9, 2


def rename(src, dst):
    d = bytearray(open(src, "rb").read())
    assert d[:4] == b"\x7fELF" and d[4] == 1 and d[5] == 1, "ELF32 LE only"
    e_shoff, = struct.unpack_from("<I", d, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", d, 0x2E)
    shdrs = [list(struct.unpack_from("<10I", d, e_shoff + i * e_shentsize)) for i in range(e_shnum)]
    stro, strsz = shdrs[e_shstrndx][4], shdrs[e_shstrndx][5]
    names = bytes(d[stro:stro + strsz])

    def name(off):
        return names[off:names.index(b"\0", off)].decode()

    owner = {}
    for s in shdrs:
        if s[1] == SHT_SYMTAB:
            symstr = shdrs[s[6]]
            for k in range(s[5] // 16):
                st_name, st_value, _, st_info, _, st_shndx = struct.unpack_from("<IIIBBH", d, s[4] + k * 16)
                if st_info & 0xF == STT_FUNC and st_value == 0 and st_shndx < e_shnum:
                    o = symstr[4] + st_name
                    owner[st_shndx] = bytes(d[o:d.index(b"\0", o)]).decode()
                off = s[4] + k * 16 + 12
                if d[off] >> 4 == 13:
                    d[off] = (2 << 4) | (d[off] & 0xF)

    # a function's .rodata (its switch jump tables) comes right before its .text, and its
    # string literals (.data) right before that
    for i, s in enumerate(shdrs):
        n = name(s[0])
        if n in (".rodata", ".data"):
            j = i + 1
            while j < e_shnum and name(shdrs[j][0]) in (".rodata", ".rel.rodata", ".rel.data"):
                j += 1
            if j < e_shnum and name(shdrs[j][0]) == ".text" and j in owner:
                owner[i] = owner[j]
    new = bytearray(names)
    for i, s in enumerate(shdrs):
        n = name(s[0])
        target = i if n in (".text", ".rodata", ".data") else \
            (s[7] if n in (".rel.text", ".rel.rodata", ".rel.data") and s[1] == SHT_REL else None)
        if target is not None and target in owner:
            s[0] = len(new)
            new += f"{n}.{owner[target]}".encode() + b"\0"
    shdrs[e_shstrndx][4], shdrs[e_shstrndx][5] = len(d), len(new)
    d += new
    for i, s in enumerate(shdrs):
        struct.pack_into("<10I", d, e_shoff + i * e_shentsize, *s)
    open(dst, "wb").write(d)


def undefined_symbols(path):
    """Names of the symbols an ELF32 LE object uses but doesn't define."""
    d = open(path, "rb").read()
    e_shoff, = struct.unpack_from("<I", d, 0x20)
    e_shentsize, e_shnum, _ = struct.unpack_from("<HHH", d, 0x2E)
    sh = [struct.unpack_from("<10I", d, e_shoff + i * e_shentsize) for i in range(e_shnum)]
    out = set()
    for s in sh:
        if s[1] == SHT_SYMTAB:
            so = sh[s[6]][4]
            for k in range(s[5] // 16):
                st_name, _, _, _, _, st_shndx = struct.unpack_from("<IIIBBH", d, s[4] + k * 16)
                if st_shndx == 0 and st_name:
                    out.add(d[so + st_name:d.index(b"\0", so + st_name)].decode())
    return out


def sections(path):
    """[(name, size)] of an ELF32 LE object."""
    d = open(path, "rb").read()
    e_shoff, = struct.unpack_from("<I", d, 0x20)
    e_shentsize, e_shnum, e_shstrndx = struct.unpack_from("<HHH", d, 0x2E)
    sh = [struct.unpack_from("<10I", d, e_shoff + i * e_shentsize) for i in range(e_shnum)]
    so = sh[e_shstrndx][4]
    return [(d[so + s[0]:d.index(b"\0", so + s[0])].decode(), s[5]) for s in sh]


if __name__ == "__main__":
    rename(sys.argv[1], sys.argv[2])
