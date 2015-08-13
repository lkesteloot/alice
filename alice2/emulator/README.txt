Alice 2 Emulator

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
    Make the HEX file (e.g. at 0x8000)
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

