# Using the Alice 3 Emulator 



## Using the premade disk images including full options for debugging, etc.:

```
./emulator -debugger "fill 0 1 0;readhex ../cpm/cpm22.hex;readhex ../cpm/alice3_cbios.hex;symbols ../cpm/alice3_cbios.prn;symbols ../cpm/cpm22.prn;pc 0xbf00" ../cpm/boot.dsk,wordstar.dsk ../rom/rom.hex
# then telnet localhost 6607, and break into telnet's prompt and set character mode with "mode char"
g
```



## Making your own CP/M hard disk images:

### Prerequisites:

* cpmtools
  * Brew and MacPorts provide a cpmtools package. 
  * Or, build cpmtools *including libdsk*
  * Add the entry in diskdefs to the distribution's diskdefs, then install. (e.g. /usr/local/share/diskdefs).  This installs a CP/M disk definition for an 8MB file matching the definition in fakehw_bios.asm.
  * It turns out that cpmtools didn't do "myz80" disks right as of mid-2015 or so.  So to get the files off the Humongous ISO disks, first strip off the first 256 bytes.  COPY_DISK does this stripping

Copy files in and out with "cpmcp".  For example, to move all the files in "cpm22/" onto the "user 0" partition in driveA.bin, use "cpmcp -T raw -f brads8mb driveA.bin cpm22/* 0:".  (The "user 0" partition is the default partition you get on bootup.  There are 16 user partitions.)

### Possibility : Hacking an empty disk image

```
# Make an empty file with the "e5" empty sector identifier
python -c 'import sys; sys.stdout.write("\xe5"*(8192*1024));' > driveA.bin
```

### Possibility : Use cpmtools CPM mkfs from cpmtools to create a new hard disk image

* Create an 8MB file, e.g. "dd if=/dev/zero bs=1024 count=8192 of=driveA.bin", then "mkfs.cpm -f brads8mb driveA.bin".  This file can now be used with the emulator's "-fakecpmhw" option as a disk drive.



## Copying Files to the disk image

### Possibility : Use cpmcp from cpmtools to copy in interesting files.  Replace with paths to which you've unpacked or downloaded CP/M files (this for-do-done is bash syntax)

```
for i in ~/Downloads/Mbasic.com ~/Downloads/SARGON.COM ~/Downloads/WS.COM ; do cpmcp -T raw -f brads8mb driveA.bin $i 0: `basename $i`; done
```

### Possibility : copy files from a hard disk image to a hard disk image using cpmcp from cpmtools

```
cpmcp -T raw -f brads8mb driveA.bin ~/Downloads/cpm22-binaries/* 0:
```

### Possibility : use COPY_DISK script to copy the contents of a myz80 disk image to a hard disk image: 

```
sh -x ./COPY_DISK ~/packages/cpm2.2/HumongousCPM/LIVE/MAXZ80/A.DSK driveA.bin 
```

### 
