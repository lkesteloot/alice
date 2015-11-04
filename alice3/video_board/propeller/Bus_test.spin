
' Test of the speed at which we can read from the Alice 2 data bus.

CON

  ' Multiply crystal input by 16 to get system clock. Our crystal is 5 MHz, so
  ' our system clock is 80 MHz, the maximum that the Propeller can handle.
  _clkmode = xtal1 + pll16x
  
  ' Specify our crystal frequency in Hz. This (multiplied by 16 for the PPLL) is
  ' stored in _clkfreq and RAM (address zero), not used by the hardware.
  _xinfreq = 5_000_000


PUB start | addr, data

  dira[3] := 1
  dira[1] := 1
  
  repeat until true
    waitcnt(clkfreq/4 + cnt)
    outa[3] := 1
    waitcnt(clkfreq/4 + cnt)
    outa[3] := 0
    
  repeat until true
    waitpeq(%1, %1, 0)
    outa[1] := 1
    addr := ina
    outa[1] := 0
    data := addr
    waitcnt(clkfreq + cnt)
    
  cognew(@asm_test, 0)
  
DAT

                        org 0
                        
asm_test                mov     dira, #%1010
                        mov     outa, #%0000

:loop
                        waitpeq Button, Button

                        xor     outa, #%10
                        mov     Addra, ina
                        xor     outa, #%10
                        mov     Dataa, Addra

                        xor     outa, #%1000
                        rdlong  Delay, #0       ' Read clock frequency.
                        add     Delay, cnt      ' Add now
                        waitcnt Delay, 0
                        xor     outa, #%1000

                        jmp     #:loop

Button    long          %0001
Addra     res           1
Dataa     res           1
Delay     res           1