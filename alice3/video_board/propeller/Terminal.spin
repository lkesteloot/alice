
' Given a sequence of bytes, writes them to the screen.
'
' Uses a wait-free queue to communicate with the bus interface.

CON

  ' Sync these with whatever was chosen in VGA_driver.spin.
  COLS = 80
  ROWS = 40
  LONGS_PER_ROW = COLS / 4

  ' For the State variable:
  STATE_NORMAL = 0
  STATE_GOT_ESCAPE = 1
  STATE_GOT_BRACKET = 2
  
  MAX_ATTRIBUTES = 5

VAR
  long Stack[20]
  long State
  long Attributes[MAX_ATTRIBUTES]
  long AttributeCount
  byte CharAttribute

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
  '
  ' VT-100 sequences supported:
  '
  '    <ESC>[Y;XH       Move to cursor location <X,Y> (1-based).
  '                         No data means home (upper-left).
  '    <ESC>[C;...;Cm   Change color to C. Supported values of C are:
  '                         0 or none:      Reset
  '                         7:              Inverse video
  '    <ESC>[K          Erase to end of line.

  ' Start on a new cog.
  cognew(Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr), @Stack[0])
  
PRI Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr) | Data, CursorX, CursorY, HeadIndex, I

  State := STATE_NORMAL
  CharAttribute := 0

  repeat
    ' Wait until the queue is not empty.
    repeat until long[HeadIndexPtr] <> long[TailIndexPtr]
      ' Nothing.

    ' Fetch the next byte from the queue.
    HeadIndex := long[HeadIndexPtr]
    Data := byte[QueuePtr][HeadIndex]

    ' Advance the head pointer.
    long[HeadIndexPtr] := (HeadIndex + 1) // Capacity

    ' Fetch the current cursor position.
    CursorX := byte[CursorXPtr]
    CursorY := byte[CursorYPtr]
    
    if State == STATE_NORMAL
      if Data == 27
        State := STATE_GOT_ESCAPE
      elseif Data == 13
        ' Carriage return.
        CursorX := 0
      elseif Data == 10
        ' Line feed.
        CursorY += 1
      elseif Data == 8
        ' Backspace
        if CursorX > 0
          CursorX -= 1
          byte[ScreenPtr][CursorY*COLS + CursorX] := 32 | CharAttribute
      else
        ' Visible character.
        byte[ScreenPtr][CursorY*COLS + CursorX] := Data | CharAttribute
        CursorX += 1
    elseif State == STATE_GOT_ESCAPE
      if Data == "["
        State := STATE_GOT_BRACKET
        AttributeCount := 1
        Attributes[AttributeCount - 1] := 0
      else
        State := STATE_NORMAL
    elseif State == STATE_GOT_BRACKET
      if Data => "0" and Data =< "9"
        Attributes[AttributeCount - 1] := Attributes[AttributeCount - 1]*10 + Data - "0"
      elseif Data == ";"
        if AttributeCount < MAX_ATTRIBUTES
          AttributeCount += 1
          Attributes[AttributeCount - 1] := 0
      elseif Data == "H"
        if AttributeCount == 2
          CursorX := Attributes[1] - 1
          CursorY := Attributes[0] - 1
        State := STATE_NORMAL
      elseif Data == "m"
        repeat I from 0 to AttributeCount - 1
          if Attributes[I] == 0
            CharAttribute := 0
          elseif Attributes[I] == 7
            CharAttribute := $80
        State := STATE_NORMAL
      elseif Data == "K"
        repeat I from CursorX to COLS - 1
          byte[ScreenPtr][CursorY*COLS + I] := 32  ' With CharAttribute?
        State := STATE_NORMAL
      else
        ' Ignore unknown code.
        State := STATE_NORMAL

    if CursorX => COLS
      ' Wrap cursor at end of row.
      CursorX := 0
      CursorY += 1
    if CursorY => ROWS
      ' Scroll up.
      CursorY := ROWS - 1
      longmove(ScreenPtr, ScreenPtr + COLS, LONGS_PER_ROW*(ROWS - 1))
      longfill(ScreenPtr + COLS*(ROWS - 1), $20202020, LONGS_PER_ROW)  ' With CharAttribute?
  
    ' Update cursor.
    byte[CursorXPtr] := CursorX
    byte[CursorYPtr] := CursorY
