"/Applications/microchip/xc8/v1.35/bin/xc8" --warn=-9 -DPIC_FIRMWARE_VERSION="Mon Nov 16 16:37:26 PST 2015" --chip=18F452 -G -mio_pic.map  --double=24 --float=24 --emi=wordwrite --opt=default,+asm,+asmfile,-speed,+space,-debug --addrqual=ignore --mode=free -P -N255 --warn=0 --asmlist --summary=default,-psect,-class,+mem,-hex,-file --output=default,-inhx032 --runtime=default,+clear,+init,-keep,-no_startup,-download,+config,+clib,-plib --output=-mcof,+elf:multilocs --stack=compiled:auto:auto:auto "--errformat=%f:%l: error: (%n) %s" "--warnformat=%f:%l: warning: (%n) %s" "--msgformat=%f:%l: advisory: (%n) %s"    --memorysummary memoryfile.xml -oio_pic.elf  io_pic.p1
