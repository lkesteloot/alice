
' Given a sequence of bytes, writes them to the screen.
'
' Uses a wait-free queue to communicate with the bus interface.

VAR
  long Stack[20]

PUB start(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr)

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

  ' Start on a new cog.
  cognew(Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr), @Stack[0])
  
PRI Process(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr) | Data, Cursor, HeadIndex
  Cursor := 0
  
  repeat
    ' Wait until the queue is not empty.
    repeat until long[HeadIndexPtr][0] <> long[TailIndexPtr][0]
      ' Nothing.

    ' Fetch the next byte from the queue.
    HeadIndex := long[HeadIndexPtr][0]
    Data := byte[QueuePtr][HeadIndex]

    ' Advance the head pointer.
    long[HeadIndexPtr][0] := (HeadIndex + 1) // Capacity

    ' Write the data to the screen.
    byte[ScreenPtr][Cursor] := Data
    if Cursor < 5000
      Cursor += 1