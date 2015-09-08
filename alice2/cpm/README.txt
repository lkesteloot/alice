Using the "Fake CP/M Hardware" in the emulator

Build cpmtools, add the entry in diskdefs to the distribution's diskdefs, then install.  Or install prebuilt cpmtools and add the content in diskdefs to the installed diskdefs (e.g. /usr/local/share/diskdefs).  This installs a CP/M disk definition for an 8MB file matching the definition in fakehw_bios.asm.

Create an 8MB file, e.g. "dd if=/dev/zero bs=1024 count=8192 of=driveA.bin", then "mkfs.cpm -f brads8mb driveA.bin".  This file can now be used with the emulator's "-fakecpmhw" option as a disk drive.

Copy files in and out with "cpmcp".  For example, to move all the files in "cpm22/" onto the "user 0" partition in driveA.bin, use "cpmcp -f brads8mb driveA.bin cpm22/* 0:".  (The "user 0" partition is the default partition you get on bootup.  There are 16 user partitions.)

