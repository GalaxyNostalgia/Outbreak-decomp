#!/bin/bash
# dis.sh file.c func: my compile of func next to the original, normalized (OVL=game etc.; default main)
ROOT=$(cd "$(dirname "$0")/../.." && pwd); cd "$ROOT"
T=$(mktemp -d)
python3 -I tools/try.py f1.${OVL:-main} $1 >/dev/null 2>&1
mips-linux-gnu-objdump -d --no-show-raw-insn -M no-aliases build/try.o 2>/dev/null | awk -v f="<$2>:" '$0 ~ f {p=1; next} p && /^$/ {exit} p' | sed -E 's/^ *[0-9a-f]+:\s*//' > $T/mine.raw
MAXL=1000 python3 tools/dev/show.py asm/SLPM_654.28/${OVL:-main}/text.s $2 | tail -n +2 | grep -v "^\.L" > $T/orig.raw
python3 - $T <<'PY'
import re,sys
T=sys.argv[1]
def norm(l):
    l=l.strip().replace('$','')
    l=re.sub(r'\s+',' ',l)
    l=re.sub(r'<[^>]*>','',l)
    l=re.sub(r'^beq zero,zero,\S*','b',l); l=re.sub(r'^b \S+','b',l)
    l=re.sub(r'^sll zero,zero,0x0','nop',l)
    l=re.sub(r'^beq (\w+),zero,\S*',r'beqz \1',l); l=re.sub(r'^bne (\w+),zero,\S*',r'bnez \1',l)
    l=re.sub(r'^(beqz|bnez) (\w+),? ?\S*',r'\1 \2',l)
    l=re.sub(r'^(beq|bne) (\w+), ?(\w+),.*',r'\1 \2,\3',l)
    l=re.sub(r'%\w+\(([^)]*)\)','0',l)
    l=re.sub(r', ',',',l)
    l=re.sub(r'(-?0x[0-9a-fA-F]+|-?\d+)\(',lambda m:str(int(m.group(1),0))+'(',l)
    l=re.sub(r',(-?0x[0-9a-fA-F]+)$',lambda m:','+str(int(m.group(1),0)),l)
    l=re.sub(r'^jal \S+','jal',l)
    return l.strip()
m=[norm(l) for l in open(T+'/mine.raw') if l.strip()]
o=[norm(l) for l in open(T+'/orig.raw') if l.strip()]
open(T+'/mine.txt','w').write('\n'.join(m)+'\n'); open(T+'/orig.txt','w').write('\n'.join(o)+'\n')
PY
diff -y -W 80 $T/mine.txt $T/orig.txt; r=$?; rm -rf $T; exit $r
