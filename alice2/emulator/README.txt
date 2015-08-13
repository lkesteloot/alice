Alice 2 Emulator

This program emulates most of the hardware in the Alice 2.

It serves a VNC screen that contains a representation of video
memory (at 0x4000, 176x262 rows), a representation ox a 2x16 LCD,
and converts VNC keypresses to AT keyboard codes.

It listens for connections on port 6606, and any incoming bytes are
sent to the hardware as if they were received on the serial port
on the PIC IO board.

Building
    Install libvncviewer (e.g. "sudo port install libvncserver")
    Edit Makefile if you aren't using MacPorts
        * set libvncserver include and library directory
    % make

Running
    % emulator ROM.bin
    Start a VNC Client (I'm using Chicken of the VNC) and connect to localhost
    Type your name and press Return

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

ToDo
----------
Why does sending a video BIN end up resetting or hanging the system?  Did it always do that?
combine RAM and ROM boards
    Add a port to switch ROM out for RAM
give video board "safe region" (green) - draw rest as dark green
image of video monitor around video board output
image of LCD around LCD bytes
