
' Interfaces with the Z-80 bus.

CON

  ' First data pin.
  D_PINS = 16
  
  ' Control pins.
  Z_IORQ = 1  ' Active low.
  Z_WR = 2    ' Active low.
  Z_A7 = 3    ' Active high.

PUB start(QueuePtrParam, HeadIndexPtrParam, TailIndexPtrParam, CapacityParam)

  QueuePtr := QueuePtrParam
  HeadIndexPtr := HeadIndexPtrParam
  TailIndexPtr := TailIndexPtrParam
  Capacity := CapacityParam
  cognew(@busread, 0)

DAT

                        org 0
                        
busread                 ' We don't do any writes.
                        mov     dira, #%0
                        
                        ' We want IORQ and RW low and A7 high.
                        mov     State, #|<0
                        mov     Mask, #|<0
                        mov     State, #0
                        mov     Mask, #|<1
                        mov     State, #|<Z_A7
                        mov     Mask, #(|<Z_IORQ | |<Z_WR | |<Z_A7)

:loop                   ' Wait for IORQ, WR, and A7 to be asserted.
                        waitpeq State, Mask

                        ' Immediately read all input pins.
                        mov     Data, ina
                        
                        ' Shift data into lowest eight bits.
                        shr     Data, #D_PINS
                        
                        ' Keep lowest 8 bits.
                        and     Data, #$FF

                        ' Write to queue.
                        rdlong  TailIndex, TailIndexPtr
                        mov     TailPtr, TailIndex
                        add     TailPtr, QueuePtr
                        wrbyte  Data, TailPtr

                        ' Advance queue.
                        add     TailIndex, #1
                        cmp     TailIndex, Capacity         wz
          if_z          mov     TailIndex, #0
                        wrlong  TailIndex, TailIndexPtr                        

                        ' Wait until end of pins.
                        waitpne State,Mask

                        jmp     #:loop

QueuePtr  long          0
HeadIndexPtr long       0
TailIndexPtr long       0
Capacity  long          0

State     res           1
Mask      res           1
Data      res           1
TailPtr   res           1
TailIndex res           1