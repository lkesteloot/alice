#ifndef __IO_SERVICE_H__
#define __IO_SERVICE_H__

volatile extern int gUnclaimedWrite;
volatile extern unsigned int gUnclaimedWriteAddressPins;
volatile extern unsigned int gUnclaimedWriteData;
volatile extern int gUnclaimedRead;
volatile extern unsigned int gUnclaimedReadAddressPins;
volatile extern int gReadWasAlreadyInactive;
volatile extern int gVideoSnoopedWrite;

int IOSERVICE_get_command(volatile unsigned char **p);
void IOSERVICE_start_response();
void IOSERVICE_append_response_byte(unsigned char r);
void IOSERVICE_finish_response();
void IOSERVICE_clear_command();
void IOSERVICE_clear_response();
int IOSERVICE_get_response_staging_length();
int IOSERVICE_is_response_empty();

inline unsigned int BUS_compute_shuffled_ADDRESS(unsigned int A, unsigned int B, unsigned C)
{
    unsigned int address = ((A & 0x6000) << 1) | ((B & 0x0700) >> 8) | ((B & 0x0018) << 7) | (C & 0x33F8);
    return address;
}


#endif /* __IO_SERVICE_H__ */
