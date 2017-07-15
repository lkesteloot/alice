Alice 2 Emulator

This program emulates most of the hardware in the Alice 2.

It serves a VNC screen that contains a representation of video
memory (at 0x4000, 176x262 rows), a representation ox a 2x16 LCD,
and converts VNC keypresses to AT keyboard codes.

It listens for connections on port 6606, and any incoming bytes are
sent to the hardware as if they were received on the serial port
on the PIC IO board.

Building
    Only tested for building on MacOSX with Apple G++ LLVM 6.0
    Install libvncviewer (e.g. "sudo port install libvncserver")
    Edit Makefile if you aren't using MacPorts
        * set libvncserver include and library directory
    % make

Running
    % emulator ROM.bin
    Start a VNC Client (I'm using Chicken of the VNC) and connect to localhost
    Type your name and press Return
    Hit ESC to quit the Emulator.

To send a HEX file to memory and run it:
    Make the HEX file so that 0x8000 is the first instruction
    % sendhex -j newfile.bin newfile.hex
    For this next step you'll need NetCat ("nc") or something that
      can send a file to a socket
    % nc localhost 6606 < newfile.bin

To send a PPM image
    Scale PPM and maybe dither as well; Alice2's screen is 176x262.
    % ppmtohex < your.ppm > newfile.hex
    % sendhex newfile.bin newfile.hex
    For this next step you'll need NetCat ("nc") or something that
      can send a file to a socket
    % nc localhost 6606 < newfile.bin

PPM Example
    % ppmtohex < alice.ppm > alice.hex
    % sendhex alice.bin alice.hex
    # Run simulator in background or other terminal
    % nc localhost 6606 < alice.bin

Debugger
----------
Run with the "-debugger" command-line option with a debugger command
(or an empty string) to start the emulator in the debugger.  Type
"help" for a list of commands.

Here's an example command that
1. Swaps RAM for ROM (using an I/O port for control of MEMORYboard)
2. loads the CPM 2.2 and Brad's Fake CPM HW assembled sources into RAM
3. loads symbols for those sources

    -debugger "out 9 0;readhex ../cpm/cpm22.hex;readhex ../cpm/fakehw_bios.hex;symbols ../cpm/fakehw_bios.prn;symbols ../cpm/cpm22.prn"

Note:
    * Only az80-format PRN listing files are currently supported for symbols

Fake CP/M 2.2 Hardware platform
-------------------------------
Run with "-fakecpmhw" option to enable "fake" CP/M hardware support
(for use with Brad's CP/M BIOS that knows the fake HW I/O ports).
Provide a comma-delimited set of 8MB CP/M disk images as a parameter.

    -fakecpmhw blarg.bin

Here's a python command to create a formatted CP/M 8MB disk image
called driveA.bin.

    python -c 'import sys; sys.stdout.write(\"\\xe5\"*(8192*1024));' > driveA.bin

ToDo
----------
Why does sending a video BIN end up resetting or hanging the system?  Did it always do that?
give video board "safe region" (green) - draw rest as dark green
image of video monitor around video board output
image of LCD around LCD bytes
