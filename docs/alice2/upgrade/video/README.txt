The timing ROM contains a byte for every clock tick.
The byte contains a mask with /SYNC (bit 7, 0x80), /BLANK (bit 5,
0x20) (don't allow pixels), and /RESET for the pixel clock counter
(bit 3, 0x08).  Note these are all active low in the current circuit.

how to generate timing rom for 15.5834MHz crystal on video board:

    # This command makes the binary file to write to ROM:
    ./makefield `echo 15.5834 / 4 | bc -l` | makerom ../../emulator/alice.ppm > 15mhz.bin

    # This command prints out the usable area, etc.  First output
    # for 15.5834MHz should be "248 262", which is the number of columns and
    # rows in the timing rom (including sync and blank.
    ./makefield `echo 15.5834 / 4 | bc -l` | head -3

How to view the resulting binary file for verification:

    # a8 = non-blank, non-sync, resulting in purple
    # 20 = blank, non-sync, resulting in blue
    # a0 = blank, sync, resulting in yellow
    ./decode_rom 248 262 a8=800080 20=0000ff a0=ffff00 < /tmp/v.bin  | xv -


