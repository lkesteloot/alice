#!/usr/bin/env python

import sys

# Ugly code, but works.
# Use: ./dat2h.py "" < z80_NP.dat >> bg80d.h;  ./dat2h.py "CB" < z80_CB.dat >> bg80d.h;  ./dat2h.py "DD" < z80_DD.dat >> bg80d.h;  ./dat2h.py "ED" < z80_ED.dat >> bg80d.h;  ./dat2h.py "FD" < z80_FD.dat >> bg80d.h;  ./dat2h.py "DDCB" < z80_DDCB.dat >> bg80d.h;  ./dat2h.py "FDCB" < z80_FDCB.dat >> bg80d.h
# And don't forget to add %s to {NP/DD/FD}{10/18/20/28/30/38}

prefix = sys.argv[1]

print "static opcode_spec z80_%s[] = {" % (prefix)
for line in sys.stdin.readlines():
    line = line.split("#")[0].strip()
    if line == '': continue
    fields = list(field.strip() for field in line.strip().split(" | "))
    fields[0] = fields[0].upper()

    if fields[2] == "1":
        fields[8] = fields[8].replace('{n}', '0x%.2X')
    elif fields[2] == "2":
        if '+{d}' in fields[8]:
            fields[8] = fields[8].replace('+{d}', '%+d')
        else:
            fields[8] = fields[8].replace('{d}', '%+d')
    elif fields[2] == "3":
        if '${nn}' in fields[8]:
            fields[8] = fields[8].replace('{nn}', '%.4X')
        else:
            fields[8] = fields[8].replace('{nn}', '0x%.4X')
    elif fields[2] == "4":
        fields[8] = fields[8].replace('+{d}', '%+d')
        fields[8] = fields[8].replace('{n}', '0x%.2X')

    fmt = '    /*{0}{1}*/  {{"{0}", {2}, {3}, {4}, "{5}", "{7}", "{8}", "{6}", "{9}", "{10}"}},'
    fields.insert(1, int(fields[0], 16))
    fields.insert(0, prefix)
    # prefix, OPh, OP, xtr, xtp, type, mnemonic, dst, src, summary, description
    print fmt.format(*fields)
print "};"
