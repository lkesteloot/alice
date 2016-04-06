#ifndef __IO_SERVICE_H__
#define __IO_SERVICE_H__

extern int gUnclaimedWrite;
extern unsigned int gUnclaimedWriteAddress;
extern unsigned int gUnclaimedWriteData;
extern int gUnclaimedRead;
extern unsigned int gUnclaimedReadAddress;
extern int gReadWasAlreadyInactive;

int IOSERVICE_get_command(volatile unsigned char **p);
void IOSERVICE_start_response();
void IOSERVICE_append_response_byte(unsigned char r);
void IOSERVICE_finish_response();
void IOSERVICE_clear_command();
void IOSERVICE_clear_response();
int IOSERVICE_get_response_staging_length();
int IOSERVICE_is_response_empty();

#endif /* __IO_SERVICE_H__ */
