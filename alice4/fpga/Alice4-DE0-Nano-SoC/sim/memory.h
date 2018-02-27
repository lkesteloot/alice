
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <verilated.h>

/**
 * Class to simulate a chunk of memory.
 */
class Memory {
public:
    Memory(uint32_t base, int wordCount)
        : mBase(base), mWordCount(wordCount)
    {
        mWords = new uint64_t[wordCount];
    }

    virtual ~Memory() {
        delete[] mWords;
    }

    uint64_t &operator[](uint32_t address) {
        return mWords[addressToIndex(address)];
    }

    void eval(vluint8_t burstCount, vluint8_t read, vluint64_t address, vluint8_t &waitRequest, vluint8_t &readDataValid, vluint64_t &readData) {
        if (read) {
            readData = (*this)[address];
            waitRequest = 0;
            readDataValid = 1;
        } else {
            waitRequest = 0;
            readDataValid = 0;
        }
    }

public:
    uint32_t mBase;
    int mWordCount;
    uint64_t *mWords;

    uint32_t addressToIndex(uint32_t address) {
        if (address < mBase) {
            printf("Address 0x%0X is too low (less than 0x%0X).\n", address, mBase);
            throw std::exception();
        }

        if (address >= mBase + mWordCount) {
            printf("Address 0x%0X is too high (past 0x%0X).\n", address, mBase + mWordCount);
            throw std::exception();
        }

        return address - mBase;
    }
};

#endif // __MEMORY_H__
