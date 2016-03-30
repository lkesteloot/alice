#!/usr/bin/env python

import argparse
import os
import struct
import sys

memspec = lambda x: int(x.replace('$','0x'),0)

def read_spec(specname):
    spec = {}
    for line in open(specname):
        line = line.split("#")[0].strip()
        if line == '': continue
        fields = list(field.strip() for field in line.strip().split(" | "))
        opcode = int(fields[0], 16)
        spec[opcode] = dict(zip(['extra', 'extra_type', 'optype', 'mnemonic', 'dst', 'src', 'summary', 'desc'],fields[1:]))
        spec[opcode]['extra'] = int(spec[opcode]['extra'])
    return spec

def parse_annotations(lines):
    addr = memspec(lines.next())
    comment = ""
    for line in lines:
        try:
            tmp2 = memspec(line)
            yield (addr, comment)
            addr = tmp2
            comment = ""
        except ValueError as e:
            comment += line
    yield (addr, comment)

def parse_ranges(lines):
    split_lines     = (line.split("#", 1) if '#' in line else (line, "") for line in lines)
    clean_lines     = ((rng.strip(), cmt.strip()) for (rng, cmt) in split_lines)
    nonblank_range  = ((rng, cmt) for (rng, cmt) in clean_lines if rng != '')
    split_range     = (rng.split("-", 1) + [cmt,] if '-' in rng else (rng, rng, cmt) for (rng, cmt) in nonblank_range)
    return            ((memspec(s), memspec(e), c.strip()) for (s, e, c) in split_range)

def help():
    print """
# cat ROMFILE | bg80d.py [options]

    MEMORY_SPEC = 4096 or 0x1000 or \$1000  (don't forget to escape $'s in the shell)

--format=FORMAT_SPEC         Currently unused (FIXME)
--start=MEMORY_SPEC          Where decoding (or stride output) should start, defaults to 0
--end=MEMORY_SPEC            Where decoding (or stride output) should end, defaults to INF
--offset=MEMORY_SPEC         Defines a starting point for addressing.  This is ONLY used in output.  Start, end,
                             strides, skips, and ranges are all expected to be calibrated to 0.  That is to say:
                             $ cat ROM | ./bg80d.pl --start=\$400 --end=\$440 --offset=\$8000
                             ...will decode the bytes 0x400-0x440 in ROM, and the output will be shown addressed
                             from $8400 to $8440.
--ranges=ranges.txt,...      Use a file that lists explicit ranges to decode.  Start and end override these ranges.
--skips=skip.txt,...         A file that lists extents that should be skipped entirely.
--strideN=stride.txt,...     Use a file that lists extents that should be output, undecoded, with a stride of N
                             Multiple entries can be provided, including multiple entries with the same stride.
                             Example: --stride3=tables_of_3byte_entries.txt
--annotations=anno.txt,...   Use a file that consists of a memory spec on a line by itself followed by an annotation
                             to insert into that address"""
    exit()

def parse_args():
    if '--help' in sys.argv[1:]: help()
    argspairs = (arg.split("=", 1) for arg in sys.argv[1:])
    args = dict((k.lstrip('-'), v.split(',')) for k, v in argspairs)
    for k in ['start', 'end', 'offset']:
        if k in args:
            globals()[k] = memspec(args[k][0])
    if 'ranges' in args:
        for rangefile in args['ranges']:
            for (start, end, comment) in parse_ranges(open(rangefile)):
                if comment != '':
                    if start not in globals()['annotations']: globals()['annotations'][start] = []
                    globals()['annotations'][start].append(comment)
                for addr in range(start, end+1):
                    globals()['decodemask'][addr] = True
    for arg in args:
        if arg[0:6] == 'stride':
            for stridefile in args[arg]:
                for (start, end, comment) in parse_ranges(open(stridefile)):
                    if comment != '':
                        if start not in globals()['annotations']: globals()['annotations'][start] = []
                        globals()['annotations'][start].append(comment)
                    for addr in range(start, end+1):
                        globals()['strides'][addr] = {'start':start, 'end':end, 'stride':int(arg[6:])}
    if 'skips' in args:
        for skipfile in args['skips']:
            for (start, end, comment) in parse_ranges(open(skipfile)):
                if comment != '':
                    if start not in globals()['annotations']: globals()['annotations'][start] = []
                    globals()['annotations'][start].append(comment)
                for addr in range(start, end+1):
                    globals()['decodemask'][addr] = False
    if 'annotations' in args:
        for annotationfile in args['annotations']:
            for (addr, annotation) in parse_annotations(open(annotationfile)):
                if addr not in globals()['annotations']: globals()['annotations'][addr] = []
                globals()['annotations'][addr].append(annotation)
    if 'format' in args:
        pass  # TODO

start = end = None
offset = 0
strides = {}
annotations = {}
decodemask = {}
parse_args()

def bytestream(fh):
    while True:
        byte = fh.read(1)
        if byte != None: yield byte
        else: break

opcode_specs = {"NP":"z80_NP.dat", "CB":"z80_CB.dat", "ED":"z80_ED.dat", "DD":"z80_DD.dat", "DDCB":"z80_DDCB.dat", "FD":"z80_FD.dat", "FDCB":"z80_FDCB.dat"}
self_path = os.path.dirname(os.path.abspath(__file__))
for k, v in opcode_specs.iteritems(): opcode_specs[k] = read_spec(os.path.join(self_path, v))

def print_disasm(cur, opcode, param, spec):
    fmt = "%-5s $%.4X   %-6s %-15s %-11s ;  %s"
    opcode_str = " ".join(("%.2x"%op for op in opcode))
    desc = spec['desc']
    param_str = ' '.join(("%.2x" % (p) for p in param))
    if spec['extra'] == 1 and 'n' in spec['mnemonic']:
        desc = desc.replace('{n}', param_str)
    elif spec['extra'] == 2 and 'nn' in spec['mnemonic']:
        nnaddr = "%.2x%.2x (%s)" % (param[1], param[0], str(param[0] + param[1]*256))
        desc = desc.replace('{nn}', nnaddr)
    elif spec['extra'] == 1 and 'd' in spec['mnemonic']:
        # Compute relative jump. This is one-byte two's complement.
        relative = param[0] if param[0] < 128 else -256 + param[0]

        # Compute the jump destination as an absolute address, since we know it. It's
        # relative to the PC address of the next instruction.
        absolute = cur + 1 + len(param) + relative

        # Special substitution if we're doing a PC-relative jump. Skip the "J" since it's
        # only sometimes capitalized. Always show a + in front of the relative amount
        # to clarify that it's relative.
        desc = desc.replace('ump {d}', 'ump %+d (%d, $%.4X)' % (relative, absolute, absolute))

        # Substitute other relative addresses (IX, IY, etc.).
        desc = desc.replace('{d}', str(relative))
    print fmt % (cur, cur, opcode_str, spec['mnemonic'], param_str, desc)

z80 = os.fdopen(sys.stdin.fileno(), 'rb', 1)
def stream_bytes(fh):
    for byte in bytestream(z80):
        if len(byte) == 0: break
        yield struct.unpack('<B', byte)[0]

cur = 0
stride = []
uchars = stream_bytes(z80)
for byte in uchars:
    if cur + offset in annotations:
        print "\n".join(annotations[cur + offset]),
    if decodemask == {}:  # not using ranges/skips
        if start != None and cur < start:
            cur += 1
            continue
        if end != None and cur >= end: break 
    else:  # not using ranges/skips
        if cur not in decodemask or decodemask[cur] == False:
            cur += 1
            continue
    if cur in strides:
        stride.append("%.2X" % (byte))
        if ((len(stride) == strides[cur]['stride']) or
            (cur+1 not in strides) or
            (cur+1 in strides and strides[cur]['start'] != strides[cur+1]['start']) or
            (cur+1 in decodemask and decodemask[cur] == False)):
            print ";", " ".join(stride)
            stride = []
        cur += 1
        continue

    # decode
    prefix = None
    opcode = []
    param = []
    spec = None

    opcode.append(byte)
    while opcode[-1] in (203, 221, 237, 253):
        if opcode[-1] == 221: prefix = 'DD'
        if opcode[-1] == 253: prefix = 'FD'
        opcode.append(uchars.next())

    if prefix == 'DD':
        if len(opcode) > 1 and opcode[-2] == 203: spec = opcode_specs["DDCB"][opcode[-1]]
        elif len(opcode) > 1 and opcode[-2] == 237: spec = opcode_specs["DDED"][opcode[-1]]
        else: spec = opcode_specs["DD"][opcode[-1]]
    elif prefix == 'FD':
        if len(opcode) > 1 and opcode[-2] == 203: spec = opcode_specs["FDCB"][opcode[-1]]
        elif len(opcode) > 1 and opcode[-2] == 237: spec = opcode_specs["FDED"][opcode[-1]]
        else: spec = opcode_specs["FD"][opcode[-1]]
    else:
        if len(opcode) > 1 and opcode[-2] == 203: spec = opcode_specs["CB"][opcode[-1]]
        elif len(opcode) > 1 and opcode[-2] == 237: spec = opcode_specs["ED"][opcode[-1]]
        else: spec = opcode_specs["NP"][opcode[-1]]

    for i in range(0, spec['extra']):  param.append(uchars.next())
    print_disasm(cur + offset, opcode, param, spec)
    cur += len(opcode) + spec['extra']
