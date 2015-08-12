#ifndef _SIMULATOR_H_
#define _SIMULATOR_H_

#include <vector>

struct board_base
{
    virtual bool memory_write(int addr, unsigned char data) { return false; }
    virtual bool memory_read(int addr, unsigned char &data) { return false; }
    virtual bool io_write(int addr, unsigned char data) { return false; }
    virtual bool io_read(int addr, unsigned char &data) { return false; }
    // interrupt?
};

extern std::vector<board_base*> boards;

#endif /* _SIMULATOR_H_ */
