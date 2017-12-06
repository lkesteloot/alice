#!python

import sys

print '',

print_labels = False

for line in sys.stdin:
    if line.strip() == "; +++ global symbols +++":
        break;

dummy = sys.stdin.next()

for line in sys.stdin:
    if len(line.strip()) == 0:
        break
    else:
        parts = line.strip().split("=");
        if len(parts) > 1:
            label = parts[0].strip()
            value = parts[1].strip()[1:]
            print "%s %s" % (value, label)

print '',
print
