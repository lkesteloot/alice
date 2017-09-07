# Using FAN5333A/B datasheet at https://www.fairchildsemi.com/datasheets/FA/FAN5333A.pdf plus resistor values cribbed from TFTFriend

R0 = 12.0
R1 = 12.0
R2 = 12.0
R3 = 6.0
print "R0 = %f, R1 = %f, R2 = %f, R3 = %f" % (R0, R1, R2, R3)
print "R0 is always connected, other resistors (R*) have jumpers (J*) to ground"

openshorted = {0: "open", 1: "shorted"}

J0_on = 1
for i in xrange(0, 8):
    J1_on = (i >> 0) & 1
    J2_on = (i >> 1) & 1
    J3_on = (i >> 2) & 1
    R_to_ground = 1 / (1 / R0 + J1_on * 1 / R1 + J2_on * 1 / R2 + J3_on * 1 / R3)
    I_LED = .315 / R_to_ground
    print "J1 = %s, J2 = %s, J3 = %s, current = %f mA" % (openshorted[J1_on], openshorted[J2_on], openshorted[J3_on], I_LED * 1000)
