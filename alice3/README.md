
# Alice III

This is the third model of the Alice machine. It runs a silicon Z80 with
two co-processors: an ARM for I/O and a Propeller for video.

In the `kicad` directory are various schematics and PCBs. The latest is
`MOBO_SMD`, which uses surface mount versions of the parts.

To bring up a new board:

* Power:
    * Use a wall wart to provide micro-USB power. Verify that one LED is on.
      If you have an in-line USB current meter, there should be less than
      10 mA being pulled.
* Propeller:
    * Hook up the Prop Plug. The Vss pin should be closest to the corner of
      the board.
    * Run PropellerIDE, load the `video_board/propeller/Main.spin` file,
      and program the EEPROM. After this you should be able to hook up a VGA
      monitor and see the line `Alice 3 GPU firmware`.
    * Remove the Prop Plug.
* ARM:
    * Connect your computer to the USB jack of the Alice III.
    * Short the jumper closest to the ARM.
    * Hard-reset the board (the push button closest to the LEDs).
    * Wait one second, then remove the jumper.
    * Run this command: `sudo ./dfu-util -d 0483:df11 -a 0 -D ioboard_arm_is_ram.dfu`

