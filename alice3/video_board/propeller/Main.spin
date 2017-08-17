{
' Main.spin
'
' The main code for the Alice 3 video board.
'
' Spins off some cogs to do the real work.
}
CON

    ' Multiply crystal input by 16 to get system clock. Our crystal is 5 MHz, so
    ' our system clock is 80 MHz, the maximum that the Propeller can handle.
    _clkmode = xtal1 + pll16x
  
    ' Specify our crystal frequency in Hz. This (multiplied by 16 for the PPLL) is
    ' stored in _clkfreq and RAM (address zero), not used by the hardware.
    _xinfreq = 5_000_000

    ' we allocate one extra row than necessary because it makes scrolling easier
    ' since we can just copy row n+1 to row n, and use the extra row of spaces to
    ' clear the bottom row 
    cols = vgatext#cols
    rows = vgatext#rows
    chrs = cols * (rows+1)
    queue_capacity = 512
  
    audio_left = 6
    audio_right = 7

    CLK_PIN = 5
    PROP_READY_PIN = 0

OBJ
    vgatext       : "VGA_driver"
    bus_interface : "Bus_interface"
    terminal      : "Terminal"
    ay            : "AYcog"
    pst           : "Parallax Serial Terminal"
    'jdcs          : "JDCogSerial"

DAT

    BootMsg byte "Alice 3 A/V firmware", 13, 10, 0
    SerialBootMsg byte "Alice 3 propeller serial terminal", 13, 0  ', pst#NL
    ' E2, B2, G#3, all 3 channels tone/no noise, all 3 channels on envelope, ~1.3s envelope, rampdown
    BootSnd byte $7B, $01, $FD, $00, $96, $00,  $00,  $38,  $10,$10,$10,  $A1,$13,  $09,  $00,$00

VAR
    ' sync long - written to -1 by VGA driver after each screen refresh
    long  sync
    ' screen buffer - aligned to long to make scroll/clearing easier
    long screen[chrs/4]
    ' row colors
    word  colors[rows]
    ' cursor control bytes
    byte  cx0, cy0, cm0, cx1, cy1, cm1
  
    byte  queue[queue_capacity]
    long  queue_head
    long  queue_tail

    long  AYregisters

    long  serial_rx
    long  serial_tx

    byte  char

PUB start | i, j, addr, data
    ' Sound the sound system
    ay.resetRegisters                                    'Reset all AY registers
    AYRegisters := ay.start(audio_right, audio_left)     'Start the emulated AY chip in one cog
    ay.updateRegisters(@BootSnd)

    ' Start the Prop debugging terminal, uses the prop plug
    pst.Start(115_200)
    pst.Str(@SerialBootMsg)

    ' Start JDCogSerial on the prop plug
    'serial_rx := jdcs.start(|<31, |<30, 115_200)
    'serial_tx := serial_rx + 4
    'long[serial_tx] := ">"
    'repeat i from 0 to strsize(@SerialBootMsg)
    '    repeat while long[serial_tx] => 0
    ''    'char := byte[SerialBootMsg++]
    '    long[serial_tx] := byte[SerialBootMsg + i + 4]
    'long[serial_tx] := "<"

    ' Configure cursor shapes and blinking.
    cm0 := %011
    cm1 := %000
    cy0 := 0
  
    ' Set up colors.
    repeat i from 0 to rows - 1
      colors[i] := %%0100_3330

    ' Fill screen with spaces.
    repeat i from 0 to chrs - 1
      screen.byte[i] := 32

    ' Start vga text driver.
    vgatext.start(8, @screen, @colors, @cx0, @sync)

    ' Start the terminal.
    queue_head := 0
    queue_tail := 0
    terminal.start(@queue, @queue_head, @queue_tail, queue_capacity, @screen, @cx0, @cy0)

    ' Start bus interface and Z-80 clock
    bus_interface.start(@queue, @queue_tail, queue_capacity, AYRegisters, @screen, @colors, cols, serial_tx)

    ' Write welcome message.
    print(@BootMsg)

    ' Tell the ARM that we're ready.
    dira[PROP_READY_PIN] := 1
    outa[PROP_READY_PIN] := 0  ' Active low.
  
    repeat ' Wait forever.

PRI print(s)
    repeat strsize(s)
        queue[queue_tail] := byte[s]
        s += 1
        queue_tail += 1
