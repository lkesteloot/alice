//----------------------------------------------------------------------------
// FatFS device routines

#include "ff.h"
#include "diskio.h"
#include "defs.h"
#include "logprintf.h"
#include "sd_spi.h"

DSTATUS disk_status (BYTE pdrv)
{
    if(pdrv != 0)
        return STA_NODISK;

    return 0;
}

DSTATUS disk_initialize (BYTE pdrv)
{
    if(pdrv != 0)
        return RES_ERROR;

    return RES_OK;
}

DRESULT disk_read (BYTE pdrv, BYTE *buff, DWORD sector,	UINT count)
{
    if(pdrv != 0)
        return RES_ERROR;

    for(int i = 0; i < count; i++)
        if(!SDCARD_readblock(sector + i, buff + SD_BLOCK_SIZE * i))  {
            logprintf(DEBUG_ERRORS, "ERROR: failed reading SD block %d\n", sector + i);
            return RES_ERROR;
        }
    
    return RES_OK;
}

DRESULT disk_write (BYTE pdrv, const BYTE *buff, DWORD sector, UINT count)
{
    if(pdrv != 0)
        return RES_ERROR;

    for(int i = 0; i < count; i++)
        if(!SDCARD_writeblock(sector + i, buff + SD_BLOCK_SIZE * i)) {
            logprintf(DEBUG_ERRORS, "ERROR: failed reading SD block %d\n", sector + i);
            return RES_ERROR;
        }
    
    return RES_OK;
}

DRESULT disk_ioctl (BYTE pdrv, BYTE cmd, void *buff)
{
    DRESULT result = RES_OK;

    if(pdrv != 0)
        return RES_ERROR;

    switch(cmd) {
        case CTRL_SYNC:
            result = RES_OK;
            break;

        default:
            logprintf(DEBUG_ERRORS, "ERROR: unexpected FatFS ioctl %d\n", cmd);
            result = RES_ERROR;
            break;
    }

    return result;
}

DWORD get_fattime(void)
{
    // returns FatFS formatted date-time
    // hardcoded to Mar 15 2016, midnight
    return
        ((_NORTC_YEAR - 1980) << 25) | 
        (_NORTC_MON << 21) | 
        (_NORTC_MDAY << 16) | 
        (0 << 11) | 
        (0 << 5) | 
        (0 << 0)
        ;
}

