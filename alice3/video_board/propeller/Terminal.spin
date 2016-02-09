
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
  STATE_ERROR = 3
  STATE_MYZ80_GOT_EQUAL = 4
  STATE_MYZ80_GOT_ROW = 5
  
  MAX_ATTRIBUTES = 5
  MAX_ERROR_BUFFER_LENGTH = 30

VAR
  long Stack[40]
  long State
  long Attributes[MAX_ATTRIBUTES]
  long AttributeCount
  long CursorX
  long CursorY
  byte CharAttribute
  byte ErrorBuffer[MAX_ERROR_BUFFER_LENGTH]
  byte ErrorBufferLength
  byte MyZ80Row

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
  
PRI Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr) | Data, HeadIndex, I

  State := STATE_NORMAL
  CharAttribute := 0
  ErrorBufferLength := 0

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
        AddErrorChar("^")
        AddErrorChar("[")
      elseif Data == 13
        ' Carriage return.
        CursorX := 0
        FixCursor(ScreenPtr)
      elseif Data == 10
        ' Line feed.
        CursorY += 1
        FixCursor(ScreenPtr)
      elseif Data == 8
        ' Backspace
        if CursorX > 0
          CursorX -= 1
          byte[ScreenPtr][CursorY*COLS + CursorX] := 32 | CharAttribute
          FixCursor(ScreenPtr)
      else
        ' Visible character.
        WriteCharacter(ScreenPtr, Data | CharAttribute)
    elseif State == STATE_GOT_ESCAPE
      AddErrorChar(Data)
      if Data == "["
        State := STATE_GOT_BRACKET
        AttributeCount := 1
        Attributes[AttributeCount - 1] := 0
      elseif Data == "T"
        ' MyZ80: Clear to end of line.
        ClearEol(ScreenPtr)
        State := STATE_NORMAL
      elseif Data == "*"
        ' MyZ80: Clear screen, home cursor.
        Cls(ScreenPtr)
        State := STATE_NORMAL
      elseif Data == ")"
        ' MyZ80: Start reversed text.
        CharAttribute := $80
        State := STATE_NORMAL
      elseif Data == "("
        ' MyZ80: End reversed text.
        CharAttribute := 0
        State := STATE_NORMAL
      elseif Data == "="
        ' MyZ80: Position cursor.
        State := STATE_MYZ80_GOT_EQUAL
      else
        State := STATE_ERROR
    elseif State == STATE_GOT_BRACKET
      AddErrorChar(Data)
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
          FixCursor(ScreenPtr)
        State := STATE_NORMAL
      elseif Data == "m"
        repeat I from 0 to AttributeCount - 1
          if Attributes[I] == 0
            CharAttribute := 0
          elseif Attributes[I] == 7
            CharAttribute := $80
        State := STATE_NORMAL
      elseif Data == "K"
        ClearEol(ScreenPtr)
        State := STATE_NORMAL
      else
        ' Unknown code.
        State := STATE_ERROR
    elseif State == STATE_MYZ80_GOT_EQUAL
      ' MyZ80: This is the row plus $20.
      MyZ80Row := Data
      State := STATE_MYZ80_GOT_ROW
    elseif State == STATE_MYZ80_GOT_ROW
      ' MyZ80: Move cursor. Parameters are row and column, one byte
      ' each, offset by $20.
      CursorX := Data - $20
      CursorY := MyZ80Row - $20
      FixCursor(ScreenPtr)
      State := STATE_NORMAL
        
    if State == STATE_ERROR
      ' Got something we can't parse. Dump it so that the user can
      ' figure out what's missing from this parser.
      repeat I from 0 to ErrorBufferLength - 1
        WriteCharacter(ScreenPtr, ErrorBuffer[I] | $80)
      State := STATE_NORMAL

    if State == STATE_NORMAL
      ErrorBufferLength := 0

    ' Update cursor.
    byte[CursorXPtr] := CursorX
    byte[CursorYPtr] := CursorY

' Add a character to the buffer of characters we're parsing. If we
' get an error, we can dump this to the screen. Otherwise we
' erase it.
PRI AddErrorChar(Char)
  if ErrorBufferLength < MAX_ERROR_BUFFER_LENGTH
    ErrorBuffer[ErrorBufferLength] := Char
    ErrorBufferLength += 1
    
' Write a character to the screen. Advances the cursor and fixes it.
PRI WriteCharacter(ScreenPtr, Char)
  byte[ScreenPtr][CursorY*COLS + CursorX] := Char
  CursorX += 1
  FixCursor(ScreenPtr)

' Make sure the cursor is on-screen. Wrap or scroll if necessary.
PRI FixCursor(ScreenPtr)
  ' Wrap cursor at end of row.
  if CursorX => COLS
    CursorX := 0
    CursorY += 1

  ' Scroll up.
  if CursorY => ROWS
    CursorY := ROWS - 1
    longmove(ScreenPtr, ScreenPtr + COLS, LONGS_PER_ROW*(ROWS - 1))
    longfill(ScreenPtr + COLS*(ROWS - 1), $20202020, LONGS_PER_ROW)  ' With CharAttribute?

' Clear to end of line.
PRI ClearEol(ScreenPtr) | I
  repeat I from CursorX to COLS - 1
    byte[ScreenPtr][CursorY*COLS + I] := 32  ' With CharAttribute?

' Clear screen and home cursor.
PRI Cls(ScreenPtr) | X, Y
  repeat Y from 0 to ROWS - 1
    repeat X from 0 to COLS - 1
      byte[ScreenPtr][Y*COLS + X] := 32 ' With CharAttribute?
  CursorX := 0
  CursorY := 0