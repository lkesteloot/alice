Using the "Fake CP/M Hardware" in the emulator

Premade disks:
    ./emulator -debugger "out 9 0;readhex ../cpm/cpm22.hex;readhex ../cpm/fakehw_bios.hex;symbols ../cpm/fakehw_bios.prn;symbols ../cpm/cpm22.prn;pc 0xbf00" -fakecpmhw driveA.bin,wordstar.dsk ../brads_alice2_archive/asm/rom.hex
    g
    # then telnet localhost 6607, and break into telnet's prompt and set character mode with "mode char"

Build cpmtools, add the entry in diskdefs to the distribution's diskdefs, then install.  Or install prebuilt cpmtools and add the content in diskdefs to the installed diskdefs (e.g. /usr/local/share/diskdefs).  This installs a CP/M disk definition for an 8MB file matching the definition in fakehw_bios.asm.

python -c 'import sys; sys.stdout.write("\xe5"*(8192*1024));' > driveA.bin
for i in /Users/grantham/packages/cpm2.2//pcsauro.altervista.org/CPMSW/lang/Mbasic.com /Users/grantham/packages/cpm2.2//pcsauro.altervista.org/CPMSW/games/SARGON.COM /Users/grantham/packages/cpm2.2//Wskpro33/WS.COM ; do cpmcp -f brads8mb driveA.bin $i 0:`basename $i`; done
sh -x ./copy_disk ~/packages/cpm2.2/HumongousCPM/LIVE/MAXZ80/A.DSK driveA.bin 
cpmcp -f brads8mb driveA.bin ~/packages/cpm2.2/www.cpm.z80.de/download/cpm22-binary/* 0:

Create an 8MB file, e.g. "dd if=/dev/zero bs=1024 count=8192 of=driveA.bin", then "mkfs.cpm -f brads8mb driveA.bin".  This file can now be used with the emulator's "-fakecpmhw" option as a disk drive.

Copy files in and out with "cpmcp".  For example, to move all the files in "cpm22/" onto the "user 0" partition in driveA.bin, use "cpmcp -f brads8mb driveA.bin cpm22/* 0:".  (The "user 0" partition is the default partition you get on bootup.  There are 16 user partitions.)

After booting CPM, store the CCP, BDOS, and BIOS to disk by running "putsys":
    readhex ../cpm/putsys.hex
    jump 0x800
At the end of running any large program, the program may return to CP/M through "warm boot", which reloads CCP from disk.  This is why putsys must store CCP.
