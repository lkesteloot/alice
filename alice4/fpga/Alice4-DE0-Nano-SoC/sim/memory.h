
#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <verilated.h>

#define DUMP_MEMORY_ACCESS false

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

    void evalRead(vluint8_t burstCount, vluint8_t read, vluint64_t address, vluint8_t &waitRequest,
            vluint8_t &readDataValid, vluint64_t &readData) {

        if (read) {
            if (burstCount != 1) {
                printf("Burst count is %d, must be 1.\n", (int) burstCount);
                throw std::exception();
            }

            readData = (*this)[address];
            if (DUMP_MEMORY_ACCESS && (address & 0xFF) == 0) {
                printf("-------------------- Reading 0x%016lx from 0x%08lx\n", readData, address);
            }
            readDataValid = 1;
        } else {
            readDataValid = 0;
        }

        waitRequest = 0;
    }

    void evalWrite(vluint8_t burstCount, vluint8_t write, vluint64_t address, vluint8_t &waitRequest,
            vluint8_t byteEnable, vluint64_t writeData) {

        if (write) {
            if (burstCount != 1) {
                printf("Burst count is %d, must be 1.\n", (int) burstCount);
                throw std::exception();
            }

            if (byteEnable != 0xFF) {
                printf("Byte enable is 0x%02X, must be 0xFF.\n", (int) byteEnable);
                throw std::exception();
            }

            if (DUMP_MEMORY_ACCESS && (address & 0xFF) == 0) {
                printf("-------------------- Writing 0x%016lx to 0x%08lx\n", writeData, address);
            }
            (*this)[address] = writeData;
        }

        waitRequest = 0;
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
