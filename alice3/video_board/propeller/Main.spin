' Main.spin
'
' The main code for the Alice 3 video board.
'
' Spins off some cogs to do the real work.

CON

  ' Multiply crystal input by 16 to get system clock. Our crystal is 5 MHz, so
  ' our system clock is 80 MHz, the maximum that the Propeller can handle.
  _clkmode = xtal1 + pll16x
  
  ' Specify our crystal frequency in Hz. This (multiplied by 16 for the PPLL) is
  ' stored in _clkfreq and RAM (address zero), not used by the hardware.
  _xinfreq = 5_000_000

  cols = vgatext#cols
  rows = vgatext#rows
  chrs = cols * rows
  queue_capacity = 512
  
  audio_left = 6
  audio_right = 7


  CLK_PIN = 5
  PROP_READY_PIN = 0

OBJ
 
  vgatext : "VGA_driver"
  bus_interface : "Bus_interface"
  terminal : "Terminal"
  ay : "AYcog"

DAT

  BootMsg byte "Alice 3 A/V firmware", 13, 10, 0
  ' E2, B2, G#3, all 3 channels tone/no noise, all 3 channels on envelope, ~1.3s envelope, rampdown
  BootSnd byte $7B, $01, $FD, $00, $96, $00,  $00,  $38,  $10,$10,$10,  $A1,$13,  $09,  $00,$00

VAR

  'sync long - written to -1 by VGA driver after each screen refresh
  long  sync
  'screen buffer - could be bytes, but longs allow more efficient scrolling
  long  screen[cols*rows/4]
  'row colors
  word  colors[rows]
  'cursor control bytes
  byte  cx0, cy0, cm0, cx1, cy1, cm1
  
  byte  queue[queue_capacity]
  long  queue_head
  long  queue_tail

  long  AYregisters

PUB start | i, j, addr, data
  ' Sound the sound system
  ay.resetRegisters                                    'Reset all AY registers
  AYRegisters := ay.start(audio_right, audio_left)     'Start the emulated AY chip in one cog
  ay.updateRegisters(@BootSnd)


  ' Configure cursor shapes and blinking.
  cm0 := %011
  cm1 := %000
  cy0 := 0
  
  ' Set up colors.
  repeat i from 0 to rows - 1
    colors[i] := %%0100_1310

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
  bus_interface.start(@queue, @queue_head, @queue_tail, queue_capacity, AYRegisters)

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
