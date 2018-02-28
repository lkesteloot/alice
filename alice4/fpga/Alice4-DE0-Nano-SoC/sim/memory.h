
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

        for (int i = 0; i < 256; i++) {
            uint64_t mask = 0;

            // Bit 0 corresponds to the least significant byte.
            // See the Avalong Memory-Mapped spec.
            for (int j = 0; j < 8; j++) {
                if (((i >> j) & 1) != 0) {
                    mask |= (uint64_t) 0xFF << (j*8);
                }
            }

            mByteEnableMask[i] = mask;
        }
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

            if (DUMP_MEMORY_ACCESS && (address & 0xFF) == 0) {
                printf("-------------------- Writing 0x%016lx to 0x%08lx\n", writeData, address);
            }

            if (byteEnable == 0xFF) {
                (*this)[address] = writeData;
            } else {
                uint64_t mask = mByteEnableMask[byteEnable];
                (*this)[address] = ((*this)[address] & ~mask) | (writeData & mask);
            }
        }

        waitRequest = 0;
    }

public:
    uint32_t mBase;
    int mWordCount;
    uint64_t *mWords;
    uint64_t mByteEnableMask[256];

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
