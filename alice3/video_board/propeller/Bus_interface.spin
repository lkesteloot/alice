{{

Interfaces with the Z80 bus:

* Runs the Z80 clock via CTRA
* I/O 128 - Enqueues bytes to the VGA/VT100 drivers
* Sets registers in the AY-3-8910 emulator
* Sets/reads custom timer
* (not yet) New video modes?

It does this by watching for IORQ==0 && A7==1, and then using A0/A1/A2
to determine which I/O port ("OUT 128..135, n") is being accessed.


TODO:
* ct_persecond handler
* ay_all handler/state machine
* update charset state machine
* CP/M programs that test each capability of this bus interface
  * cpuspeed.com
  * settimer.com
  * ztunes.com
  * Something that uses INIR/OTIR
* Code cleanup
  * I don't think those longs need to be initialized to 1
  * "when the assembler encounters a RES, it reserves space in cog memory, but not in hub memory where the program is stored"  (tricks and traps pg7)
  * Do we actually need to know where the head is for the terminal queue?
  * Do we need to retrieve the tailidx, or can we just work from our local copy and update the tailidxptr as needed?  No other queue reader will be updating it.  Specifically "rdlong  TailIndex, TailIndexPtr" can probably go
  * Lots of double-shifting and and'ing, probably not necessary
  * Use test against INA directly where it makes sense: "test  bus_pattern, ina  wz  ' ina MUST be source, not dest"

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

    BP_1   = 6    ' bartport
    BP_2   = 7
    BP_3   = 27

    Z80_SPEED = 2_000_000  ' 2_250_000 is about as fast as ARMasRAM can go


PUB start(QueuePtrParam, HeadIndexPtrParam, TailIndexPtrParam, CapacityParam, AY)

    QueuePtr := QueuePtrParam
    HeadIndexPtr := HeadIndexPtrParam
    TailIndexPtr := TailIndexPtrParam
    Capacity := CapacityParam
    CapacityMask := Capacity - 1
    AYRegisters := AY

    ' What we really want to express is this:
    ' z80FRQ := $FFFF_FFFF / (PROP_SPEED / Z80_SPEED)
    ' ...but SPIN only deals with signed 32-bit numbers so
    ' we do this at a lower precision and multiply with
    ' the understanding that the intermediate and final
    ' results aren't greater than MAXINT
    z80FRQ := $7FFF_FFFF / (PROP_SPEED / Z80_SPEED)
    z80FRQ := z80FRQ * 2
    z80CTR := (%00100 << 26) | Z_CLK        ' NCO single-ended

    ' On 80MHz prop: 1Hz == $0000_001A 60Hz == $0000_064A, 1KHz == $0000_68DB, 10KHz == $0004_1893
    CTFRQ := $0000_053E
    CTCTR := (%00100 << 26) | BP_3          ' NCO single-ended

    z80DirWR := |<Z_CLK | |<BP_3                ' output z80 clock only
    z80DirRD := |<Z_CLK | |<BP_3 | ($FF<<Z_D0)  ' output z80 clock + data pins

    cognew(@init, 0)


DAT

            org 0

' - - - - Set up our handler state machines
init
            movs    ay_state, #ay_start  ' initialize "ay" state machine pointer
            movs    ct_state, #ct_start  ' initialize "ct" state machine pointer

' - - - - Start the Z80 clock, initial custom timer, set up our bus mask
businit
            mov     dira, z80DIRWR
            ' Configure CTRA/FRQA so that PHSA[31] oscillates at Z80_SPEED Hz
            mov     ctra, z80CTR
            mov     frqa, z80FRQ
            ' The z80 clock should now be oscillating

            mov     ctrb, CTCTR
            mov     frqb, CTFRQ

            ' We're looking for IORQ low and A7 high.
            mov     bus_pattern, #(|<Z_A7)  ' Z_IORQ is active *LOW*
           'mov     bus_mask, #(|<Z_IORQ | |<Z_WR | |<Z_RD | |<Z_A7)
            mov     bus_mask, #(|<Z_IORQ | |<Z_A7)

' - - - - Bus watcher and handler jump tables
loop       ' Wait for A7 and !IORQ to be asserted, we eval write/read next
            waitpeq bus_pattern, bus_mask

            mov     ctra, #0    ' pause Z80 clock
            mov     Data, ina   ' immediately read all input pins

            mov     IOAddr, Data
            shr     IOAddr, #Z_A0
            and     IOAddr, #7
            test    Data, #(|<Z_WR)      wz
    if_z    add     IOAddr, #out_handlers  ' WR line was low, which is *active*, this is a write ("OUT port, n")
    if_nz   add     IOAddr, #in_handlers   ' WR line was high, which is *inactive*, this is a read ("IN port")
            jmp     IOAddr

out_handlers
            jmp     #terminal_in   '0
            jmp     ay_state       '1 - jump to the address IN ay_state, not the address OF ay_state
            jmp     ct_state       '2 - ditto
            jmp     #complete      '3
            jmp     #complete      '4
            jmp     #complete      '5
            jmp     #complete      '6
            jmp     #complete      '7

in_handlers
            jmp     #complete      '0 - no reads from terminal
            jmp     #complete      '1 - no reads from ay
            jmp     #ct_read       '2
            jmp     #complete      '3
            jmp     #complete      '4
            jmp     #complete      '5
            jmp     #complete      '6
            jmp     #complete      '7

' - - - - VT100 terminal queue

terminal_in
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
            jmp #complete

' - - - -

' - - - - AY-3-8910 register update state machine

ay_start
            shr     Data, #Z_D0
            and     Data, #255
            cmp     Data, #16       wc
    if_nc   jmp #complete    ' in the future: ay_allatonce
            mov     AYRegPtr, Data
            movs    ay_state, #ay_oneatatime
            jmp #complete

ay_oneatatime
            shr     Data, #Z_D0
            add     AYRegPtr, AYRegisters
            wrbyte  Data, AYRegPtr
            movs    ay_state, #ay_start
            jmp #complete

' - - - - - - - - - - - - - - - -

' - - - - Custom timer update state machine

ct_read
            mov     TMP, phsb
            shr     TMP, #31
            and     TMP, #$FF

            shl     TMP, #Z_D0
            mov     dira, z80DirRD  ' change dira pins to allow output to data bus
            mov     outa, TMP       ' write byte to bus
            mov     ctra, z80CTR    ' reenable Z80 clock
            waitpne  bus_mask, Data ' wait for bus state to change: Z80 should bring Z_RD high (inactive) after reading from data bus
            mov     ctra, #0        ' pause Z80 clock
            mov     dira, z80DirWR  ' change outa pins back to mask/prevent writing to data bus
            mov     outa, #0        ' bring the data bus back low
            jmp #complete

ct_start
            shr     Data, #Z_D0
            and     Data, #255
            cmp     Data, #1       wc
    if_nc   movs    ct_state, #ct_persecond  ' OUT 130, 1
    if_c    movs    ct_state, #ct_direct_1   ' OUT 130, 0
            jmp #complete

ct_persecond
            shr     Data, #Z_D0
            and     Data, #255
            ' calculate CTFRQ based on {Data}/s ... table lookup, even from hub, might be fastest
            ' mov     frqb, CTFRQ
            movs    ct_state, #ct_start
            jmp #complete

ct_direct_1
            shr     Data, #Z_D0
            and     Data, #$FF
            mov     CTFRQ, Data
            movs    ct_state, #ct_direct_2
            jmp #complete

ct_direct_2
            shr     Data, #Z_D0
            and     Data, #$FF
            shl     Data, #8
            or      CTFRQ, Data
            movs    ct_state, #ct_direct_3
            jmp #complete

ct_direct_3
            shr     Data, #Z_D0
            and     Data, #$FF
            shl     Data, #16
            or      CTFRQ, Data
            movs    ct_state, #ct_direct_4
            jmp #complete

ct_direct_4
            shr     Data, #Z_D0
            and     Data, #$FF
            shl     Data, #24
            or      CTFRQ, Data
            mov     frqb, CTFRQ
            movs    ct_state, #ct_start
            jmp #complete

' - - - - - - - - - - - - - - - -



' - - - - Complete: reenable the Z80 clock and wait until the Z80 stops asserting A7 and !IORQ
complete
            mov     ctra, z80CTR           ' reenable Z80 clock
            waitpne bus_pattern, bus_mask  ' Wait until end of pins.
            jmp     #loop

' - - - - Jumping off point for catching unexpected IO requests, just dump Data for now
unhandled
            call #dumpdata
            jmp #complete


' Outputs Data to the terminal in hex.  This is generally the state of ina (the bus)
' on IORQ, but some handlers might shove debugging into Data prior to calling dumpdata
' or Data may be filled with a different bus state in the middle of a handler.
dumpdata
            ' D E B U G - - - - - - - - - - - - - - - - - - - - -
            mov     TMP, Data
            shr     TMP, #28
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #24
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #20
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #16
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #12
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #8
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #4
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, Data
            shr     TMP, #0
            and     TMP, #15
            add     TMP, #48
            cmp     TMP, #58  wc
    if_nc   add     TMP, #7
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            mov     TMP, #32
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  TMP, TailPtr  ' only copies lowest byte
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr
            ' D E B U G - - - - - - - - - - - - - - - - - - - - -
dumpdata_ret  ret



QueuePtr        long    0
HeadIndexPtr    long    0
TailIndexPtr    long    0
Capacity        long    0
CapacityMask    long    0
AYRegisters     long    0

z80CTR          long    0
z80FRQ          long    0  ' NCO FRQ register value, not "clock frequency"

CTCTR           long     0
CTFRQ           long     0

z80DIRRD        long    0
z80DIRWR        long    0

IOAddr          long    0

bus_pattern     long    1
bus_mask        long    1
Data            long    1
TailPtr         long    1
TailIndex       long    1

TMP             long     1

ay_state        res     1
AYRegPtr        res     1

ct_state        res     1
