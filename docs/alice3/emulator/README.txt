Alice 3 Emulator

This program emulates the hardware in the Alice 3.

It listens for connections on port 6607, and any incoming bytes are
sent to the hardware as if they were received on the serial port
on PIC IO, and any outgoing console bytes are sent from the emulator
to the port.

Building
    Only tested for building on MacOSX with Apple G++ LLVM 6.0
    Edit Makefile if you aren't using MacPorts
    % make

Running
    % ./emulator hard_disk.bin rom.hex
    telnet to localhost 6607
    switch to character mode with ^] to enter telnet's command prompt, then "m c" and return


Debugger
----------
Run with the "-debugger" command-line option with a debugger command
(or an empty string) to start the emulator in the debugger.  Type
"help" for a list of commands.

Here's an example command that
1. Swaps RAM for ROM (by writing to address 0)
2. loads the CPM 2.2 and Brad's Fake CPM HW assembled sources into RAM
3. loads symbols for those sources
4. drops into the debugger before executing any Z80 instructions

    -debugger "fill 0 1 0;readhex ../cpm/cpm22.hex;readhex ../cpm/fakehw_bios.hex;symbols ../cpm/fakehw_bios.prn;symbols ../cpm/cpm22.prn"

Note:
    * Only az80-format PRN listing files are currently supported for symbols


Alice 3 CP/M 2.2 Hardware platform
-------------------------------
Provide a comma-delimited set of 8MB CP/M disk images as a parameter.

    driveA.bin,wordstar.dsk

Here's a python command to create a formatted CP/M 8MB disk image
called driveA.bin.

    python -c 'import sys; sys.stdout.write(\"\\xe5\"*(8192*1024));' > driveA.bin
