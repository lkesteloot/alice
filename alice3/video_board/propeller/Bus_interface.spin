{{

This code has two roles, it runs the Z80 clock via CTRA and
interfaces with the Z80 I/O bus to provide some peripherals. 

It does the latter by watching for IORQ==0 && A7==1, and then using
A0/A1/A2 to determine which I/O port ("OUT 128..135, n") is being
accessed.  In some cases where the message is only a single byte
long it processes it immediately; in other cases handlers are broken
up into state machines so that they can handle a command followed
by parameters.

* OUT 128 - Enqueues bytes to the VGA/VT100 drivers
* OUT 129 - Sets registers in the AY-3-8910 emulator ; two modes:
    * OUT 129, reg_number (0..15) ; OUT 129, reg_value
    * OUT 129, 16 ; OUT 129, reg0_value ; OUT 129, reg1_value ; ... ; OUT 129, reg15_value
* IN  130 - Reads custom timer
* OUT 130 - Sets custom timer ; two modes, direct (0) and "per second":
    * OUT 130, 0 ; OUT 130, byte_0 ; OUT 130, byte_1 ; OUT 130, byte_2 ; OUT 130, byte_3
    * OUT 130, 1 ; OUT 130, ticks_per_sec  (!not yet implemented!)
* OUT 131 - Write directly to the character buffer
    * Set X, set Y, set count, then write count bytes
* OUT 132 - Write directly to the color buffer
    * Set Y, write color byte
    * color byte is arranged %F0RRGGBB, two bits per color, bit 6 ignored, bit 7 is fg/bg (0/1)

TODO:
* ct_persecond handler (stubbed, but "per second" math not implemented)
    * Send 1 to start, then a byte of how many ticks per second:  OUT 130, 1;  OUT 130, 60;
* charset setting handlers
* serial terminal output (mirroring, per-char, etc.)
    * this is commented-out in the code below, it should probably be moved over to Terminal.spin
* CP/M programs that test each capability of this bus interface
    * cpuspeed.com
    * settimer.com
    * ztunes.com
    * setcolor.com
    * Something that uses INIR/OTIR to hit these state machines as fast as possible
* Code cleanup
    * "QueuePtr := QueuePtrParam" => "queue_ptr := QueuePtr"
    * In almost all handlers the first instruction is "shr Data, #Z_D0" usually followed by "and Data, #$FF", this can probably be moved to the main loop
    * I don't think those longs need to be initialized to 1
    * "when the assembler encounters a RES, it reserves space in cog memory, but not in hub memory where the program is stored" (tricks and traps pg7)
    * Do we actually need to know where the head is for the terminal queue?
    * Do we need to retrieve the tailidx, or can we just work from our local copy and update the tailidxptr as needed?
        * No other queue reader will be updating it.  Specifically "rdlong  TailIndex, TailIndexPtr" can probably go
    * Lots of double-shifting and and'ing, probably not necessary
    * Use test against INA directly where it makes sense: "test  bus_pattern, ina  wz  ' ina MUST be source, not dest"
    * That dumpdebug function sucks
    * Lots of camelcase... ew

Future ideas:

* New video modes:  a>SETVIDEO 320x240.DRV
* Firmware emulation drivers:  a>EMULATE PACMAN.EMU  or a>EMULATE COLECO.EMU SMURFS.ROM

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


PUB start(QueuePtrParam, {HeadIndexPtrParam,} TailIndexPtrParam, CapacityParam, AY, screen, colors, cols, serial_tx)

    QueuePtr := QueuePtrParam
    'HeadIndexPtr := HeadIndexPtrParam
    TailIndexPtr := TailIndexPtrParam
    Capacity := CapacityParam
    CapacityMask := Capacity - 1
    AYRegisters := AY
    dfb_screen_ptr := screen
    dfc_screen_ptr := colors
    dfb_cols := cols << 8  ' preparing for the bitwise multiply below

    tx := serial_tx

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
            movs    ay_state, #ay_start    ' initialize "ay" state machine pointer
            movs    ct_state, #ct_start    ' initialize "ct" state machine pointer
            movs    dfb_state, #dfb_start  ' initialize "dfb" state machine pointer
            movs    dfc_state, #dfc_start  ' initialize "dfc" state machine pointer

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

            'mov     ctra, #0    ' pause Z80 clock
            mov     Data, ina   ' immediately read all input pins

            mov     IOAddr, Data
            shr     IOAddr, #Z_A0
            and     IOAddr, #7
            test    Data, #(|<Z_WR)      wz
    if_z    add     IOAddr, #out_handlers  ' WR line was low, which is *active*, this is a write ("OUT port, n")
    if_nz   add     IOAddr, #in_handlers   ' WR line was high, which is *inactive*, this is a read ("IN port")
            shr     Data, #Z_D0            ' The shr and and ended up as the first two instructions on every
            and     Data, #$FF             ' handler so they've been moved to the main loop
            jmp     IOAddr

out_handlers
            jmp     #terminal_in   '0 - enquque byte for the terminal
            jmp     ay_state       '1 - jump to the address *ay_state
            jmp     ct_state       '2 - ditto
            jmp     dfb_state      '3 - ditto
            jmp     dfc_state      '4
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
'clean            shr     Data, #Z_D0
'clean            and     Data, #$FF

            ' Write to queue.
            rdlong  TailIndex, TailIndexPtr
            mov     TailPtr, TailIndex
            add     TailPtr, QueuePtr
            wrbyte  Data, TailPtr  ' only copies lowest byte

            ' Advance queue.
            add     TailIndex, #1
            and     TailIndex, CapacityMask
            wrlong  TailIndex, TailIndexPtr

            ' Send the byte to the terminal
'tx_wait     rdlong  TMP,tx
'            shl     TMP,#1  WC
'     if_nc  jmp     #tx_wait
'            wrlong  Data,tx

            cmp     Data, #$07  wz  ' ASCII BEL
    if_z    call    #ay_BEL

            jmp #complete
' - - - -

' - - - - Direct framebuffer state machine
dfb_start
dfb_recv_x
'clean            shr     Data, #Z_D0
'clean            and     Data, #255
            mov     dfb_write_ptr, dfb_screen_ptr
            add     dfb_write_ptr, Data
            movs    dfb_state, #dfb_recv_y
            jmp #complete

dfb_recv_y
'clean            shr     Data, #Z_D0
'clean            and     Data, #255
            ' Data *= dfb_cols
            ' 8-bit multiply adapted from Prop manual, Appendix B
            ' would be 1/3rd faster if unrolled
            ' dfb_cols is already << 8
            mov     TMP, #8
            shr     Data, #1        wc  ' initial LSB multiplier into c
:loop  if_c add     Data, dfb_cols  wc  ' if c was set add dfb_cols, wc will always clear c
            rcr     Data, #1        wc  ' next LSB multiplier into c, shift product
            djnz    TMP,  #:loop        ' loop until done (16 times)

            add     dfb_write_ptr, Data
            movs    dfb_state, #dfb_recv_count
            jmp #complete

dfb_recv_count
'clean            shr     Data, #Z_D0
'clean            and     Data, #255
            mov     dfb_count, Data
            movs    dfb_state, #dfb_recv_chars
            jmp #complete

dfb_recv_chars
            ' Could be faster if it did the writes one long at a time,
            ' but that would probably mean splitting this into four...
'clean            shr     Data, #Z_D0
            wrbyte  Data, dfb_write_ptr  ' only copies lowest byte
            add     dfb_write_ptr, #1
            sub     dfb_count, #1       wz
    if_z    movs    dfb_state, #dfb_start
            jmp #complete

' Color update state machine handlers

dfc_start
'clean            shr     Data, #Z_D0
            and     Data, #$3F  ' only rows 0..63
            shl     Data, #1  ' 16-bit words for each color
            mov     dfc_write_ptr, dfc_screen_ptr
            add     dfc_write_ptr, Data
            movs    dfc_state, #dfc_set_color
            jmp #complete

dfc_set_color
'clean            shr     Data, #Z_D0
            test    Data, #$80      wz
    if_nz   add     dfc_write_ptr, #1  ' Foreground or background?
            and     Data, #$3F  ' only 6 bits of color...
            shl     Data, #2    ' ... in the top 6 bits
            wrbyte  Data, dfc_write_ptr  ' only copies lowest byte
            movs    dfc_state, #dfc_start
            jmp #complete


' - - - -

' - - - - AY-3-8910 register update state machine

ay_start
'clean            shr     Data, #Z_D0
'clean            and     Data, #255
            cmp     Data, #16       wc
            ' All 16 registers at once
    if_nc   mov     AYRegPtr, #0
    if_nc   movs    ay_state, #ay_allatonce
    if_nc   jmp #complete
            ' One register at a time
            mov     AYRegPtr, Data
            movs    ay_state, #ay_oneatatime
            jmp #complete

ay_oneatatime
'clean            shr     Data, #Z_D0
            add     AYRegPtr, AYRegisters
            wrbyte  Data, AYRegPtr
            movs    ay_state, #ay_start
            jmp #complete

ay_allatonce
'clean            shr     Data, #Z_D0
            mov     TMP, AYRegPtr
            add     TMP, AYRegisters
            wrbyte  Data, TMP
            add     AYRegPtr, #1
            cmp     AYRegPtr, #16  wz
   if_z     movs    ay_state, #ay_start
            jmp #complete

ay_BEL
            mov    AYRegPtr, AYRegisters
            wrlong ay_BEL0, AYRegPtr
            add    AYRegPtr, #4
            wrlong ay_BEL1, AYRegPtr
            add    AYRegPtr, #4
            wrlong ay_BEL2, AYRegPtr
            add    AYRegPtr, #4
            wrlong ay_BEL3, AYRegPtr
ay_BEL_ret  ret

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
            'mov     ctra, #0        ' pause Z80 clock
            mov     dira, z80DirWR  ' change outa pins back to mask/prevent writing to data bus
            mov     outa, #0        ' bring the data bus back low
            jmp #complete

ct_start
'clean            shr     Data, #Z_D0
'clean            and     Data, #255
            cmp     Data, #1       wc
    if_nc   movs    ct_state, #ct_persecond  ' OUT 130, 1
    if_c    movs    ct_state, #ct_direct_1   ' OUT 130, 0
            jmp #complete

ct_persecond
'clean            shr     Data, #Z_D0
'clean            and     Data, #255
            ' calculate CTFRQ based on {Data}/s ... table lookup, even from hub, might be fastest
            ' mov     frqb, CTFRQ
            movs    ct_state, #ct_start
            jmp #complete

ct_direct_1
'clean            shr     Data, #Z_D0
'clean            and     Data, #$FF
            mov     CTFRQ, Data
            movs    ct_state, #ct_direct_2
            jmp #complete

ct_direct_2
'clean            shr     Data, #Z_D0
'clean            and     Data, #$FF
            shl     Data, #8
            or      CTFRQ, Data
            movs    ct_state, #ct_direct_3
            jmp #complete

ct_direct_3
'clean            shr     Data, #Z_D0
'clean            and     Data, #$FF
            shl     Data, #16
            or      CTFRQ, Data
            movs    ct_state, #ct_direct_4
            jmp #complete

ct_direct_4
'clean            shr     Data, #Z_D0
'clean            and     Data, #$FF
            shl     Data, #24
            or      CTFRQ, Data
            mov     frqb, CTFRQ
            movs    ct_state, #ct_start
            jmp #complete

' - - - - - - - - - - - - - - - -



' - - - - Complete: reenable the Z80 clock and wait until the Z80 stops asserting A7 and !IORQ
complete
            mov     ctra, z80CTR           ' reenable Z80 clock
            waitpne bus_pattern, bus_mask  ' wait until z80 is ready
            jmp     #loop

' - - - - Jumping off point for catching unexpected IO requests, just dump Data for now
unhandled
            mov debug, Data
            call #dumpdebug
            jmp #complete


' Outputs debug to the terminal in hex.  This is generally the state of ina (the bus)
' on IORQ, but some handlers might shove debugging into Data prior to calling dumpdebug
' or Data may be filled with a different bus state in the middle of a handler.
dumpdebug
            ' D E B U G - - - - - - - - - - - - - - - - - - - - -
            mov     TMP, debug
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

            mov     TMP, debug
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

            mov     TMP, debug
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

            mov     TMP, debug
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

            mov     TMP, debug
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

            mov     TMP, debug
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

            mov     TMP, debug
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

            mov     TMP, debug
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
dumpdebug_ret  ret



QueuePtr        long    0
'HeadIndexPtr    long    0
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

debug           long    1
TMP             long     1

ay_state        long     1
AYRegPtr        long     1
'AY BEL: $FD,$00, $BA,$00, $8E,$00,  $00,  $38,  $10,$10,$10,  $A1,$07,  $09,  $00,$00
ay_BEL0         long     $FD, $00, $BA, $00
ay_BEL1         long     $8E, $00, $00, $38
ay_BEL2         long     $10, $10, $10, $A1
ay_BEL3         long     $07, $09, $00, $00

ct_state        long     1


dfb_state       long     1  ' direct frame buffer state machine "next state" pointer
dfb_screen_ptr  long     1
dfb_cols        long     1
dfb_rows        long     1
dfb_write_ptr   long     1
dfb_count       long     1

dfc_state       long     1
dfc_screen_ptr  long     1
dfc_write_ptr   long     1

tx              long     1