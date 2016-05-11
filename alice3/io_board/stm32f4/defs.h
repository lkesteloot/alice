#ifndef __MAIN_H__
#define __MAIN_H__

// 1 for v1 and v2, 3 for v3
#define ALICE3_V1 1
#define ALICE3_V3 3
#define ALICE3_VERSION  ALICE3_V3

// if 1, then ARM reads and writes only A0-A15 and reads MREQ
// if 0, then ARM uses A0-A17 and bus mastering to access external RAM,
//   only writes MREQ
#define ALICE3_V3_ARM_IS_RAM 1

// if 1, print some information on missed bus RD and WR, and toggle a pin
// at interesting times in the ISRs
#define DEBUG_BUS_ISR 0 

#define XSTR(x) STR(x)
#define STR(x) # x

#define enable_interrupts() __enable_irq()
#define disable_interrupts() __disable_irq()

extern void panic(void);

// XXX these should be an extern unsigned char[] set through linker
inline void ccmram_set(unsigned int address, unsigned char b)
{
    ((unsigned char *)0x10000000)[address] = b;
}

inline unsigned char ccmram_get(unsigned int address)
{
    return ((unsigned char *)0x10000000)[address];
}

#endif /* __MAIN_H__ */
