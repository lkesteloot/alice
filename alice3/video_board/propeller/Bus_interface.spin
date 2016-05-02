{{

Interfaces with the Z80 bus:

* Runs the Z80 clock via CTRA
* I/O 128 - Enqueues bytes to the VGA/VT100 drivers
* (not yet) Sets registers in the AY-3-8910 emulator
* (not yet) Sets/reads custom timer
* (not yet) New video modes?

It does this by watching for IORQ==0 && WR==0 && A7==1

}}

CON
    _xinfreq = 5_000_000
    _clkmode = xtal1 + pll16x
    PROP_SPEED = _xinfreq * 16

    Z_CLK  = 5
    Z_IORQ = 1    ' Active low.
    Z_WR   = 2    ' Active low.
    Z_RD   = 3    ' Active low.

    Z_D0   = 16   ' First data pin.
    Z_A7   = 4    ' Active high.
    Z_A0   = 24   ' Active high.
    Z_A1   = 25   ' Active high.
    Z_A2   = 26   ' Active high.

    Z80_SPEED = 2_000_000  ' 2_250_000 is about as fast as ARMasRAM can go

PUB start(QueuePtrParam, HeadIndexPtrParam, TailIndexPtrParam, CapacityParam)

    QueuePtr := QueuePtrParam
    HeadIndexPtr := HeadIndexPtrParam
    TailIndexPtr := TailIndexPtrParam
    Capacity := CapacityParam
    CapacityMask := Capacity - 1

    ' What we really want to express is this:
    ' z80FRQ := $FFFF_FFFF / (PROP_SPEED / Z80_SPEED)
    ' ...but SPIN only deals with signed 32-bit numbers so
    ' we do this at a lower precision and multiply
    z80FRQ := $7FFF_FFFF / (PROP_SPEED / Z80_SPEED)
    z80FRQ := z80FRQ * 2


    z80CTR := (%00100 << 26) | Z_CLK        ' NCO single-ended
    z80DirWR := |<Z_CLK                ' output z80 clock only
    'z80DirRD := |<Z_CLK | $FF<Z_D0     ' output z80 clock + data pins

    cognew(@businit, 0)


DAT

            org 0

businit
            mov     dira, z80DIRWR
            ' Configure CTRA/FRQA so that PHSA[31] oscillates at Z80_SPEED Hz
            mov     ctra, z80CTR
            mov     frqa, z80FRQ
            ' The z80 clock should now be oscillating
                        
                        
            ' We're looking for IORQ and RW low and A7 high.
            mov     State, #|<Z_A7
            mov     Mask, #(|<Z_IORQ | |<Z_WR | |<Z_A7)

:loop       ' Wait for IORQ, WR, and A7 to be asserted.
            waitpeq State, Mask

            mov     ctra, #0    ' pause Z80 clock

            ' Immediately read all input pins.
            mov     Data, ina

            ' Shift data into low byte, wrbyte below will handle masking
            shr     Data, #Z_D0

            ' Write to queue.
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  Data, TailPtr  ' only copies lowest byte

            ' Advance queue.
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     ctra, z80CTR  ' reenable Z80 clock

            ' Wait until end of pins.
            waitpne State,Mask
            jmp     #:loop


QueuePtr        long    0
HeadIndexPtr    long    0
TailIndexPtr    long    0
Capacity        long    0
CapacityMask    long    0

z80CTR          long    0
z80FRQ          long    0  ' NCO FRQ register value, not "clock frequency"

z80DIRRD        long    0
z80DIRWR        long    0

State           long    1
Mask            long    1
Data            long    1
TailPtr         long    1
TailIndex       long    1
