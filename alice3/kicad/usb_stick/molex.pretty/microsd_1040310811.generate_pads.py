class vec2(object):

    def __init__(self, x, y):
        self.x = x
        self.y = y

def vec2_add(v, dx, dy):
    return vec2(v.x + dx, v.y + dy)

class pad(object):
    def __init__(self, name, degrees, size, center):
        if name is None:
            name = '""'
        self.name = name
        self.degrees = degrees
        self.size = size
        self.center = center

def pad_lower_left(name, degrees, size, ll):
    return pad(name, degrees, size, vec2_add(ll, size.x/2.0, -size.y/2.0))

def pad_upper_left(name, degrees, size, ul):
    return pad(name, degrees, size, vec2_add(ul, size.x/2.0, size.y/2.0))

def pad_get_upper_left(p):
    return vec2(p.center.x - p.size.x / 2.0, p.center.y - p.size.y / 2.0)

pads = []

center = vec2(0, 0) # center line and product line for Molex 1040310811

pad_1_8_size = vec2(.85, 1.1)
pad_1_8_left_to_left = -1.1

p = pad("1", 0, pad_1_8_size, vec2_add(center, 3.105, -.25))
pads.append(p)

p = pad("2", 0, pad_1_8_size, vec2_add(p.center, pad_1_8_left_to_left, 0))
pads.append(p)

p = pad("3", 0, pad_1_8_size, vec2_add(p.center, pad_1_8_left_to_left, 0))
pads.append(p)

p = pad("4", 0, pad_1_8_size, vec2_add(p.center, pad_1_8_left_to_left, 0))
pads.append(p)

p = pad("5", 0, pad_1_8_size, vec2_add(p.center, pad_1_8_left_to_left, 0))
pads.append(p)

p = pad("6", 0, pad_1_8_size, vec2_add(p.center, pad_1_8_left_to_left, 0))
pads.append(p)

p = pad("7", 0, pad_1_8_size, vec2_add(p.center, pad_1_8_left_to_left, 0))
pads.append(p)

p = pad_lower_left("8", 0, vec2(.75, 1.1), vec2_add(center, -4.17 - .75, .3))
pads.append(p)

p = pad_lower_left("CD1", 0, vec2(1.2, 1.0), vec2_add(center, 5.14, -5.9))
pads.append(p)

p = pad_lower_left("CD2", 0, vec2(1.2, 1.0), vec2_add(center, 5.14, -9.6))
pads.append(p)

# G1
p = pad_upper_left(None, 0, vec2(1.55, 1.35), vec2_add(center, 4.79, .3 - 1.35))
pads.append(p)

# G2
p = pad_upper_left(None, 0, vec2(1.17, 1.8), vec2_add(center, -(5.17 + 1.17), .3 - 1.8))
pads.append(p)

# G3
p = pad_upper_left(None, 0, vec2(1.9, 1.35), vec2_add(center, -(2.78 + 1.9), -10.4 - 1.35))
pads.append(p)

# G4
p = pad_upper_left(None, 0, vec2(1.9, 1.35), vec2_add(center, 1.29, -10.4 - 1.35))
pads.append(p)

for p in pads:
    print '  (pad %s smd rect (at %f %f %f) (size %f %f) (layers F.Cu F.Paste F.Mask)' % (p.name, p.center.x, p.center.y, p.degrees, p.size.x, p.size.y)
    print '    (clearance 0.20066))'
