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

data_pad_size = vec2(.3, 1.3)
data_pad_left_to_left = .5
first_data_pad_ll = vec2(-11 + 1.25 + .15, -6.1)

data_pad_ll = first_data_pad_ll

for n in range(1, 41):
    p = pad("%d" % n, 0, data_pad_size, data_pad_ll)
    pads.append(p)
    data_pad_ll = vec2_add(p.center, data_pad_left_to_left, 0);

# G left
p = pad_lower_left(None, 0, vec2(1.8, 2.2), vec2(-12.8, 0))
pads.append(p)

# G right
p = pad_lower_left(None, 0, vec2(1.8, 2.2), vec2(11, 0))
pads.append(p)

for p in pads:
    print '  (pad %s smd rect (at %f %f %f) (size %f %f) (layers F.Cu F.Paste F.Mask)' % (p.name, p.center.x, p.center.y, p.degrees, p.size.x, p.size.y)
    print '    (clearance 0.20066))'
