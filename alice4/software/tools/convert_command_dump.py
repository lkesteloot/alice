#!/usr/bin/python

# Converts textual output of a demo program that contains the hex
# commands into a binary file. Removes all END commands except for
# the last one.
#
# Usage:
#     python convert_command_dump.py < cmd.txt > cmd.bin
#

import sys
import struct

PREFIX = "fpga cmd 0x"
CMD_CLEAR = 1
CMD_ZCLEAR = 2
CMD_PATTERN = 3
CMD_DRAW = 4
CMD_BITMAP = 5
CMD_SWAP = 6
CMD_END = 7
CMD_CZCLEAR = 8

DRAW_TRIANGLES = 0
DRAW_LINES = 1
DRAW_POINTS = 2

def main():
    # There are two formats, one from the hardware rasterizer (see
    # DUMP_ALL_COMMANDS in hardware_rasterizer.c) and one from
    # the network rasterizer (see SAVE_DRAWING_FILENAME in
    # network_rasterizer.c). Look at the first line to detect which
    # it is.

    count = 0
    is_first_line = True
    z_enable = 0
    pattern_enable = 0
    vertex_count = 0

    for line in sys.stdin.xreadlines():
        if is_first_line:
            is_network_format = line.startswith("winopen ")
            is_first_line = False

        if is_network_format:
            line = line.strip()
            if not line or line.startswith("#"):
                # Blank line or comment, skip.
                pass
            else:
                fields = line.split()
                if fields:
                    if vertex_count:
                        x = int(fields[0])/32
                        y = 479 - int(fields[1])/32
                        value = \
                                (x << 2) | \
                                (y << 15) | \
                                ((int(fields[2]) >> 16) << 24) | \
                                (int(fields[3]) << 56) | \
                                (int(fields[4]) << 48) | \
                                (int(fields[5]) << 40)
                        sys.stdout.write(struct.pack("<Q", value))
                        vertex_count -= 1
                    else:
                        if fields[0] == "winopen":
                            # Window open, skip.
                            pass
                        elif fields[0] == "bitmap":
                            raise Exception("Bitmaps aren't supported")
                        elif fields[0] == "swap":
                            sys.stdout.write(struct.pack("<Q", CMD_SWAP))
                        elif fields[0] == "zbuffer":
                            z_enable = int(fields[1])
                        elif fields[0] == "pattern":
                            pattern_enable = int(fields[1])
                        elif fields[0] == "clear":
                            value = CMD_CLEAR \
                                | (int(fields[1]) << 56) \
                                | (int(fields[2]) << 48) \
                                | (int(fields[3]) << 40)
                            sys.stdout.write(struct.pack("<Q", value))
                        elif fields[0] == "zclear":
                            value = CMD_ZCLEAR \
                                | (int(fields[1]) << 16)
                            sys.stdout.write(struct.pack("<Q", value))
                        elif fields[0] == "linewidth":
                            # Line width, ignore.
                            pass
                        elif fields[0] == "setpattern":
                            sys.stdout.write(struct.pack("<Q", CMD_PATTERN))
                            for i in range(4):
                                # XXX Not sure if this is the right layout.
                                value = \
                                        (int(fields[i*4 + 1], 16) << 0) | \
                                        (int(fields[i*4 + 2], 16) << 16) | \
                                        (int(fields[i*4 + 3], 16) << 32) | \
                                        (int(fields[i*4 + 4], 16) << 48)
                                sys.stdout.write(struct.pack("<Q", value))
                        elif fields[0] == "draw":
                            draw_type = int(fields[1])
                            vertex_count = int(fields[2])

                            if draw_type != DRAW_TRIANGLES:
                                raise Exception("Only triangles are supported")

                            value = CMD_DRAW \
                                | (draw_type << 8) \
                                | (1 << 16) \
                                | (z_enable << 32) \
                                | (pattern_enable << 33)
                            sys.stdout.write(struct.pack("<Q", value))
        else:
            if line.startswith(PREFIX):
                value = int(line[len(PREFIX):], 16)
                if value == CMD_END:
                    # END command, skip it.
                    pass
                else:
                    # Little-endian 64-bit value.
                    sys.stdout.write(struct.pack("<Q", value))

    # Append END command.
    sys.stdout.write(struct.pack("<Q", CMD_END))

main()
