
# Makes a boot script that can be put on the SD card to configure
# various things. Run from the SoC EDS command shell.
u-boot.scr: u-boot.txt Makefile
	mkimage -A arm -T script -O linux -C none -a 0 -e 0 -n "Boot script" -d u-boot.txt u-boot.scr
	@echo Now copy u-boot.scr to the SD card.


