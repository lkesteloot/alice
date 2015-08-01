#!/usr/bin/env python

import os, sys, string, re, commands
from stat import *


def get_image_size(filename):
    cmd = "convert " + filename + " ppm:- | pnmfile"
    status, output = commands.getstatusoutput(cmd)
    if status == 0:
        match = re.search("([0-9]*) by ([0-9]*)", output)
        size = map(int, match.groups())
    else:
        print 'Cannot find the size of "' + filename + '"'
        size = None

    return size


def fix_value(img_src, full_tag, value_re, good_value, label):
    match = value_re.search(full_tag)
    if match is None:
        print img_src + ": Set " + label + " to " + str(good_value)
        full_tag = full_tag + ' %s="%d"' % (label, good_value)
    else:
        value = int(match.group(1))

        if value != good_value:
            print "%s: %s was wrong (%d), updated to %d" % \
                (img_src, label, value, good_value)
            full_tag = full_tag[:match.start(1)] + str(good_value) + \
                full_tag[match.end(1):]

    return full_tag

def add_size(directory, text, tag):
    start = 0
    src_re = re.compile(r'[sS][rR][cC]="([^"]*)"')
    width_re = re.compile(r'[wW][iI][dD][tT][hH]="([^"]*)"')
    height_re = re.compile(r'[hH][eE][iI][gG][hH][tT]="([^"]*)"')

    while 1:
        pos = string.find(text, tag, start)
        if pos == -1:
            break
        end = string.find(text, ">", pos)
        if end == -1:
            continue
        full_tag = text[pos:end]
        orig_full_tag = full_tag

        # print "Full tag:", full_tag
        match = src_re.search(full_tag)
        if match is None:
            print "Error, no SRC in IMG"
        else:
            img_src = match.group(1)
            #print "Image source:", img_src
            size = get_image_size(directory + '/' + img_src)
            #print "Image size:", size

            full_tag = fix_value(img_src,
                full_tag, width_re, size[0], "WIDTH")
            full_tag = fix_value(img_src,
                full_tag, height_re, size[1], "HEIGHT")

        text = text[0:pos] + full_tag + text[end:]
        start = end + len(full_tag) - len(orig_full_tag)

    return text

def process_html(directory, f):
    filename = directory + '/' + f
    print 'Processing "' + filename + '"'
    text = open(filename).read()
    orig = text

    text = add_size(directory, text, '<img')
    text = add_size(directory, text, '<IMG')
    text = add_size(directory, text, '<Img')

    if text != orig:
        open(filename, 'w').write(text)
        print 'Wrote "' + filename + '"'

def process(dir, ext, func):
    if ext[0] != '.':
        ext = '.' + ext
    ext_len = len(ext)

    for f in os.listdir(dir):
        mode = os.stat('%s/%s' % (dir, f))[ST_MODE]
        if S_ISDIR(mode):
            # recurse into directory
            process('%s/%s' % (dir, f), ext, func)
        elif S_ISREG(mode) and f[-ext_len:] == ext:
            func(dir, f)

if __name__ == '__main__':
    process('.', 'html', process_html)
