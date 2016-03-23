import sys

name = sys.argv[1]
file = open(sys.argv[2], "rb")

print "unsigned char %s_bytes[] = {" % name

length = 0

bytes = file.read(16)
while bytes:
    length += len(bytes)
    print "    " + ", ".join([ ("0x%02x" % (ord(b))) for b in bytes]) + ", "
    bytes = file.read(16)
print "};"
print "unsigned int %s_length = %d;" % (name, length)
