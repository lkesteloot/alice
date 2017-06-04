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
CMD_END = 7

def main():
    count = 0
    for line in sys.stdin.xreadlines():
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
