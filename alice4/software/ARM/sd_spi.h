#ifndef __SD_SPI_H__
#define __SD_SPI_H__

#define SD_BLOCK_SIZE 512 // XXX can actually be something different?

void SPI_enable_sd();
void SPI_disable_sd();
void SPI_config_for_sd();

int SDCARD_readblock(unsigned int blocknum, unsigned char *block);
int SDCARD_writeblock(unsigned int blocknum, const unsigned char *block);
int SDCARD_init();

#endif /* __SD_SPI_H__ */
