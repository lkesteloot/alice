
' Given a sequence of bytes, writes them to the screen.
'
' Uses a wait-free queue to communicate with the bus interface.

CON

  ' Sync these with whatever was chosen in VGA_driver.spin.
  COLS = 100
  ROWS = 50
  LONGS_PER_ROW = COLS / 4
  
VAR
  long Stack[20]

PUB start(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr)

  ' QueuePtr is a long that points to the first byte of the queue.
  ' 
  ' HeadIndexPtr is a long that points to a long, which is an index to
  ' the head of the queue. The head is the next place that a byte should
  ' be read. The index is a byte offset from the first byte of the queue.
  '
  ' TailIndexPtr is a long that points to a long, which is an index to
  ' the tail of the queue. The tail is the next place that a byte should
  ' be written. The index is a byte offset from the first byte of the queue.
  ' 
  ' Capacity is the number of bytes in the queue array.
  ' 
  ' ScreenPtr is a long that points to the first byte of the screen.
  ' 
  ' CursorXPtr and CursorYPtr are pointers to bytes that describe the location of
  ' the text cursor (0-based).

  ' Start on a new cog.
  cognew(Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr), @Stack[0])
  
PRI Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr) | Data, CursorX, CursorY, HeadIndex

  repeat
    ' Wait until the queue is not empty.
    repeat until long[HeadIndexPtr] <> long[TailIndexPtr]
      ' Nothing.

    ' Fetch the next byte from the queue.
    HeadIndex := long[HeadIndexPtr]
    Data := byte[QueuePtr][HeadIndex]

    ' Advance the head pointer.
    long[HeadIndexPtr] := (HeadIndex + 1) // Capacity

    ' Write the data to the screen.
    CursorX := byte[CursorXPtr]
    CursorY := byte[CursorYPtr]

    if Data == 13
      ' Carriage return.
      CursorX := 0
    elseif Data == 10
      ' Line feed.
      CursorY += 1
    elseif Data == 8
      ' Backspace
      if CursorX > 0
        CursorX -= 1
        byte[ScreenPtr][CursorY*COLS + CursorX] := 32
    else
      ' Visible character.
      byte[ScreenPtr][CursorY*COLS + CursorX] := Data
      CursorX += 1
    
    if CursorX => COLS
      ' Wrap cursor at end of row.
      CursorX := 0
      CursorY += 1
    if CursorY => ROWS
      ' Scroll up.
      CursorY := ROWS - 1
      longmove(ScreenPtr, ScreenPtr + COLS, LONGS_PER_ROW*(ROWS - 1))
      longfill(ScreenPtr + COLS*(ROWS - 1), $20202020, LONGS_PER_ROW)
  
    ' Update cursor.
    byte[CursorXPtr] := CursorX
    byte[CursorYPtr] := CursorY
