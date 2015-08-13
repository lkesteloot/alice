#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <vector>
#include <rfb/rfb.h>
#undef max

extern bool Z80_INTERRUPT_FETCH;
extern unsigned short Z80_INTERRUPT_FETCH_DATA;

struct board_base
{
    virtual bool memory_write(int addr, unsigned char data) { return false; }
    virtual bool memory_read(int addr, unsigned char &data) { return false; }
    virtual bool io_write(int addr, unsigned char data) { return false; }
    virtual bool io_read(int addr, unsigned char &data) { return false; }
    virtual bool board_get_interrupt(int& irq) { return false; }
    virtual void init(void) { }

    virtual bool handle_rfb_key(rfbBool down, rfbKeySym key, rfbClientPtr cl) { return false; }
    virtual bool get_rfb_requirements(int& w, int& h) { w=-1; h=-1; return false; } 
    virtual void set_rfb(rfbScreenInfoPtr server, int x, int y) { }
    virtual void idle(void) {};
};

extern std::vector<board_base*> boards;

#endif /* _SIMULATOR_H_ */
