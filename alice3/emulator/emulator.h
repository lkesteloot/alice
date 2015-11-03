#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <vector>
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

    virtual void idle(void) {};
    virtual void pause(void) {};
    virtual void resume(void) {};
};

extern std::vector<board_base*> boards;

#endif /* _SIMULATOR_H_ */
