import sys

lib_body = """EESchema-LIBRARY Version 2.3
#encoding utf-8
#
# touchscreen_controller
#
DEF %(partname)s U 0 40 Y Y 1 F N
F0 "U" 0 0 60 H V C CNN
F1 "%(partname)s" 0 %(label)d 60 H V C CNN
F2 "" 0 0 60 H V C CNN
F3 "" 0 0 60 H V C CNN
DRAW
S %(left)d %(top)d %(right)d %(bottom)d 0 1 0 N
%(pins)s
ENDDRAW
ENDDEF
#
#End Library"""

dcm_body = """EESchema-DOCLIB  Version 2.0
#
#End Doc Library"""

partname = sys.argv[1]

vars = {
    "partname" : partname,
}

# input file is a series of lines of left and right pins, in the form "name,type,name,type".
# if name is blank, no pin is emitted.
# type can be KiCAD types, like I for input, O for output, B for bidirectional, or W for power.

rows = []
lpins = 0
rpins = 0
for l in sys.stdin:
    words = l.strip().split(",")
    rows.append([words[0], words[1], words[2], words[3]])
    if words[0].strip():
        lpins += 1
    if words[2].strip():
        rpins += 1

totalpins = lpins + rpins

pins = ""
maxY = (len(rows) - 1) * 100 / 2
Y = maxY

halfwidth = max(400, len(rows) * 100 / 2)
print "halfwidth = ", halfwidth
vars["left"] = -(halfwidth - 150)
vars["right"] = (halfwidth - 150)
vars["top"] = maxY + 100
vars["bottom"] = -(maxY + 100)
vars["label"] = -(maxY + 150)

lpin = 1
rpin = lpins + rpins
for (lname, ltype, rname, rtype) in rows:
    if lname.strip():
        pins += "X %s %d -%d %d 200 R 50 50 1 1 %s\n" % (lname, lpin, halfwidth, Y, ltype)
        lpin += 1
    if rname.strip():
        pins += "X %s %d %d %d 200 L 50 50 1 1 %s\n" % (rname, rpin, halfwidth, Y, rtype)
        rpin -= 1
    Y -= 100

pro_guidance = """#
# Put this in your .pro file, adding "../" if necessary
# "LibName31=%(partname)s"
#"""

sch_guidance = """#
# Put this in your .sch file:"
# "LIBS:%(partname)s"
#"""

vars["pins"] = pins
libfile = open(partname + ".lib", "w")
print >>libfile, lib_body % vars
print >>libfile, pro_guidance % vars
print >>libfile, sch_guidance % vars
libfile.close();

dcmfile = open(partname + ".dcm", "w")
print >>dcmfile, dcm_body % vars
dcmfile.close();

print pro_guidance % vars
print sch_guidance % vars
