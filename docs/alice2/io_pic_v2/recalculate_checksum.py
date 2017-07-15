import sys

for line in sys.stdin:
    bytes = line[1:-3]
    sum = 0
    for i in range(0, len(bytes), 2):
        sum += int(bytes[i:i+2], 16)
    print ":%s%02X" % (bytes, ((sum ^ 0xffff) + 1) & 0xff)

"""
:10005800020EF420DA6E83CFDFFF02010051D8B4AA
:10005800020EF420DA6E83CFDFFF02010051D8B41C
"""


