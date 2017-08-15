{{

Terminal emulation code, PropASM version, highly influenced by LK's Spin version.

* Uses a wait-free queue to communicate with the bus interface.
* Uses a state machine to simplify processing (thanks LK!)
* Uses a jumptable for non-printable char handling for speed/simplicity
* Handles default params for CSI codes
* Robust against malformed CSI codes
* Unhandled codes are shown in inverse glory
    * non-printable characters are displayed as inverse hex digits
    * VT52-type ESC codes are displayed as inverse hex digits without the leading ESC
    * VT100-type CSI codes ("0x1B[...") are displayed as inverse chars without the leading ESC

The following codes are supported (even if some are nop):

ASCII  VT-52  VT-100  MYZ80
  X                           08 - backspace
  X                           0A - line feed
                        X     0C - cursor right
  X                           0D - carriage return
                        X     1A - clear screen
                        X     1E - cursor home
                        X     1B = r c - move cursor to row and column from values encoded in ASCII
         X                    1B * - clear screen and home cursor
         X                    1B E - clear screen and home cursor
         X                    1B G - exit graphics mode (nop)
         X                    1B J - clear screen
         X                    1B K - clear EOL
                        X     1B N - end inverse text
                        X     1B T - clear EOL
         X                    1B Y r c - set row and column from values encoded in ASCII
         X                    1B ) - start inverse text  (VT100 uses this for charset?)
         X                    1B ( - end inverse text    (VT100 uses this for charset?)
         X                    1B < - full ANSI mode (nop)
         X                    1B > - exit alternate keypad (nop)
                 X            1B [ n J - erase in display ; only n=2 (clear display) implemented
                 X            1B [ rr ; cc H - move cursor to row and column encoded in decimal
                 X            1B [ rr ; cc f - move cursor to row and column encoded in decimal
                 X            1B [ n m - select graphic rendition (only normal/inverse supported)

TODO:

* Use a jumptable for ESC and CSI codes once we know we have enough space (no mode codes)
* Unhandled VT52-type ESC codes should show leading ESC
* More codes:
    * 09 - TAB (interpret this as cursor_x+=8?)
    * 16 - toggle inverse, needed by mbasic
    * 1B 3F n - VT52 "application" code (see "VT100 Programming Reference Card" PDF)
    * 1B 48 - VT52 graphics control mode
    * 1B [ 0 K - VT100 EL (erase in line)

}}

CON
    CSI_BUFFER_DEPTH = 16
    CSI_FLAG_QUESTION_MARK = 1

OBJ
 
  vgatext : "VGA_driver"

VAR
    long rowoffset[vgatext#rows]


PUB start(QueuePtr, HeadIndexPtr, TailIndexPtr, Capacity, ScreenPtr, CursorXPtr, CursorYPtr) | i
    ' set up a lookup table of pointers to where rows start in the screen buffer
    repeat i from 0 to vgatext#rows
        rowoffset[i] := ScreenPtr + (i*vgatext#cols)

    queue_ptr    := QueuePtr
    head_ptr     := HeadIndexPtr
    tail_ptr     := TailIndexPtr
    cap          := Capacity
    cap_mask     := Capacity - 1

    screen       := ScreenPtr
    scroll_cnt   := ((vgatext#rows) * vgatext#cols) / 4
    cursor_ptr   := screen
    rows_ptr     := @rowoffset
    rows         := vgatext#rows
    cols         := vgatext#cols
    cursor_ctrl  := CursorXPtr

    cognew(@init, 0)

DAT
            org 0
init
            movs term_state, #state_normal

main
:queue_wait
            rdlong  tail, tail_ptr
            rdlong  head, head_ptr
            cmp     head, tail       wz
   if_z     jmp #:queue_wait

            ' get address of front of queue and read byte
            rdlong  head, head_ptr
            add     head, queue_ptr
            rdbyte  char, head

            ' advance queue head
            rdlong  head, head_ptr
            add     head, #1
            and     head, cap_mask
            wrlong  head, head_ptr

            jmp term_state
            jmp #main


state_normal
            ' if character is < 32 (" ") use the C0 jump function
            cmp     char, #$20    wc
    if_c    jmp     #c0

            ' otherwise just print it
            call    #outc
            jmp     #main


error
            call #errorc
            jmp  #normal


outc
            xor     char, inverse
            wrbyte  char, cursor_ptr
            add cursor_x,   #1
            call #fix_cursor
outc_ret    ret


errorc
            mov char_tmp, char      ' keep a copy
            ' first nybble
            shr   char, #4
            cmp   char, #10   wc
    if_c    add   char, #"0"     ' 0-9
    if_nc   add   char, #55      ' A-F  (55 == "A" - 10)
            or  char, #128
            call  #outc

            mov char, char_tmp

            ' second nybble
            and   char, #$0F
            cmp   char, #10   wc
    if_c    add   char, #"0"     ' 0-9
    if_nc   add   char, #55      ' A-F  (55 == "A" - 10)
            or  char, #128
            call  #outc
errorc_ret ret


fix_cursor
           'mov cols, #80
           cmp cursor_x, cols    wc, wz   ' if cursor_x >= cols: 
    if_ae  mov cursor_x, #0               '     cursor_x = 0
    if_ae  add cursor_y, #1               '     cursor_y += 1
           cmp cursor_y, rows    wc, wz   ' if cursor_y >= rows:
    if_ae  mov cursor_y, rows             '     cursor_y = rows
    if_ae  sub cursor_y, #1               '     cursor_y -= 1
    if_ae  call #scroll_up

            ' set cursor_ptr from cursor x/y via rowoffset table we populated above
            mov tmp, cursor_y
            shl tmp, #2
            add tmp, rows_ptr
            rdlong cursor_ptr, tmp
            add cursor_ptr, cursor_x

            ' set the video driver's cursor
            wrbyte cursor_x, cursor_ctrl
            add cursor_ctrl, #1
            wrbyte cursor_y, cursor_ctrl
            sub cursor_ctrl, #1
fix_cursor_ret  ret


scroll_up
            mov tmp, scroll_cnt
            mov src, screen
            add src, cols
            mov dst, screen
:copy
            rdlong  char, src
            wrlong  char, dst
            add src, #4
            add dst, #4
            djnz tmp, #:copy
scroll_up_ret  ret


'c0 jumptable dispatching
c0
            mov     tmp, char
            add     tmp, #c0_jumptable
            jmp     tmp

c0_jumptable
            jmp     #error        '00 - NUL
            jmp     #error        '01 - SOH
            jmp     #error        '02 - STX
            jmp     #error        '03 - ETX
            jmp     #error        '04 - EOT
            jmp     #error        '05 - ENQ
            jmp     #error        '06 - ACK
            jmp     #main         '07 - BEL
            jmp     #backspace    '08 - BS
            jmp     #error        '09 - HT
            jmp     #linefeed     '0A - LF
            jmp     #error        '0B - VT
            jmp     #cursor_right '0C - FF
            jmp     #carriage_return  '0D - CR
            jmp     #error        '0E - SO
            jmp     #error        '0F - SI
            jmp     #error        '10 - DLE
            jmp     #error        '11 - DC1
            jmp     #error        '12 - DC2
            jmp     #error        '13 - DC3
            jmp     #error        '14 - DC4
            jmp     #error        '15 - NAK
            jmp     #error        '16 - SYN
            jmp     #error        '17 - ETB
            jmp     #error        '18 - CAN
            jmp     #error        '19 - EM
            jmp     #clear_screen '1A - SUB
            jmp     #escape       '1B - ESC
            jmp     #error        '1C - FS
            jmp     #error        '1D - GS
            jmp     #cursor_home  '1E - RS
            jmp     #error        '1F - US


backspace
            ' can't backspace from 0,0
            cmp  cursor_y, #0    wz
   if_z     cmp  cursor_x, #0    wz
   if_z     jmp  #:backspace_done

            ' backspace on column 0, go to end of previous line
            cmp  cursor_x, #0    wz
   if_z     mov  cursor_x, cols
   if_z     sub  cursor_x, #1
   if_z     sub  cursor_y, #1
   if_z     jmp  #:backspace_done

            sub  cursor_x, #1
:backspace_done
            call #fix_cursor
            jmp  #normal

linefeed
            add  cursor_y, #1
            call #fix_cursor
            jmp  #normal

carriage_return
            mov  cursor_x,   #0
            call #fix_cursor
            jmp  #normal

inverse_on
            mov inverse, #128
            jmp #normal
inverse_off
            mov inverse, #0
            jmp #normal

cursor_right
            add cursor_x,   #1
            call #fix_cursor
            jmp #normal

cursor_home
            mov  cursor_x, #0
            mov  cursor_y, #0
            call #fix_cursor
            jmp  #normal

clear_screen
            mov tmp, scroll_cnt
            shl tmp, #2          ' scrolling uses longs, we need number of bytes
            mov dst, screen
            call #erase

            mov cursor_x, #0
            mov cursor_y, #0
            call #fix_cursor
            jmp #normal

clear_eol
            mov     tmp,  cols
            sub     tmp,  cursor_x
            mov     dst,  cursor_ptr
            call #erase
            jmp #normal

' fills tmp number chars with spaces, starting from dst
erase
            cmp     tmp, #0   wz
    if_z    jmp     #erase_ret
:loop
            ' TODO performance enhancement: if (tmp >=4) and (dst & 0x3 == 0), wrlong and dst+=4 instead
            wrbyte  fourspaces, dst
            add     dst, #1
            djnz    tmp, #:loop
erase_ret   ret


normal
            movs term_state, #state_normal
            jmp #main

escape
            movs term_state, #state_esc
            jmp  #main


' ESC codes
'     Most are one char, so we perform an action and return to normal processing
'     Some are multiple characters (ie. "="/"Y", "[") so we update the state machine
state_esc
            cmp  char, #"["  wz  ' Entering CSI param accumulation, multi-state
    if_z    rdlong  csi_start, head_ptr
    if_z    mov  csi_tmp,   #0
    if_z    mov  csi_stack, #0
    if_z    mov  csi_flags, #0
    if_z    mov  csi_params, #0
    if_z    movs term_state, #state_csi_collect
    if_z    jmp  #main

            cmp  char, #"="  wz  ' Position cursor, 2-chars (states) follow
    if_z    movs term_state, #state_esc_eq_row
    if_z    jmp #main
            cmp  char, #"Y"  wz  ' Position cursor, 2-chars (states) follow
    if_z    movs term_state, #state_esc_eq_row
    if_z    jmp #main

            cmp  char, #"*"  wz  ' Clear screen, home cursor
    if_z    jmp  #clear_screen

            cmp  char, #"E"  wz  ' Clear screen, home cursor
    if_z    jmp  #clear_screen

            cmp  char, #"G"  wz  ' Exit graphics mode
    if_z    jmp  #normal

            ' overridden to clear_screen instead since at least CATCHUM in vt-52 is expecting this
            cmp  char, #"J"  wz  ' Clear screen from cursor
    if_z    jmp  #clear_screen

            cmp  char, #"K"  wz  ' Clear to end of line
    if_z    jmp  #clear_eol

            cmp  char, #"T"  wz  ' Clear to end of line
    if_z    jmp  #clear_eol

            cmp  char, #"N"  wz  ' End reversed text
    if_z    jmp  #inverse_off

            cmp  char, #")"  wz  ' Start reversed text
    if_z    jmp  #inverse_on

            cmp  char, #"("  wz  ' End reversed text
    if_z    jmp  #inverse_off

            cmp  char, #"<"  wz ' Full ANSI mode (unlikely to be implemented)
    if_z    jmp  #normal

            cmp  char, #">"  wz ' Exit alternate keypad
    if_z    jmp  #normal

            movs term_state, #state_normal
            jmp  #error

state_esc_eq_row
            sub char, #" "
            mov  cursor_y, char
            movs term_state, #state_esc_eq_col
            jmp #main
state_esc_eq_col
            sub char, #" "
            mov  cursor_x, char
            call #fix_cursor
            jmp #normal


state_csi_collect
            ' digits
            cmp char, #"0"  wc
    if_nc   cmp char, #":"  wc
    if_c    jmp #csi_collect_digit

            cmp char, #"?"  wz
    if_z    or  csi_flags, CSI_FLAG_QUESTION_MARK
    if_z    jmp #main


            ' if csi_start and head are _NOT_ consecutive: push csi_tmp onto the stack
            '
            ' why?  if we are processing a non-digit/flag (";", ANSI command, or error) then we
            ' should push what we have onto the stack, except for one case: there are zero
            ' parameters,
            ' ione case: 
            
            mov tmp, csi_start
            add tmp, #1
            and tmp, cap_mask
            cmp tmp, head  wz
    if_nz   call #csi_push

            cmp char, #";"    wz
    if_z    jmp #main

            cmp  char, #"H"  wz
    if_z    jmp  #ansi_CUP_H

            cmp  char, #"f"  wz
    if_z    jmp  #ansi_CUP_f

            cmp  char, #"h"  wz
    if_z    jmp  #ansi_DECTCEM_show

            cmp  char, #"l"  wz
    if_z    jmp  #ansi_DECTCEM_hide

            cmp  char, #"m"  wz
    if_z    jmp  #ansi_SGR

            cmp  char, #"J"  wz
    if_z    jmp  #ansi_ED

'            cmp  char, #"K"  wz
'    if_z    jmp  #ansi_EL

            jmp  #csi_error

csi_push
           shl csi_stack, #8
           or  csi_stack, csi_tmp
           mov csi_tmp, #0
           add csi_params, #1
csi_push_ret ret


csi_pop
           mov csi_tmp, csi_stack
           and csi_tmp, #$FF
           shr csi_stack, #8
           sub csi_params, #1
csi_pop_ret ret


csi_collect_digit
            mov tmp, csi_tmp
            shl csi_tmp, #3   ' *8
            add csi_tmp, tmp  ' +1
            add csi_tmp, tmp  ' +1
            sub char, #"0"
            add csi_tmp, char
            jmp #main


' flush the CSI buffer to screen in inverse
csi_error
            mov     char, #219  ' inverse [
            call    #outc
            rdlong  head, head_ptr
:loop       mov     tmp, csi_start
            add     tmp, queue_ptr
            rdbyte  char, tmp
            ' the actual chars, not hex like errorc outputs
            or      char, #128
            call    #outc
            add     csi_start, #1
            and     csi_start, cap_mask
            cmp     csi_start, head    wz
   if_nz    jmp     #:loop

            jmp #normal


' ANSI Erase in Display (different from clear_screen/eol)
ansi_ED
            ' no params
            cmp csi_params, #1   wc
    if_c    jmp #ansi_ED_eod

            ' param 0
            call #csi_pop
            cmp csi_tmp, #0
    if_z    jmp #ansi_ED_eod

            ' param 1
            cmp csi_tmp, #1
    if_z    jmp #ansi_ED_bod

            ' param 2+
            jmp #ansi_ED_screen

ansi_ED_eod ' clear to end of display          (not implemented yet)
ansi_ED_bod ' clear from beginning of display  (not implemented yet)
ansi_ED_screen 'clear entire display
            mov tmp, scroll_cnt
            shl tmp, #2          ' scrolling uses longs, we need number of bytes
            mov dst, screen
            call #erase
            jmp #normal


ansi_DECTCEM_show
            add cursor_ctrl, #2
            mov tmp, #%011  ' cursor on, blink fast
            wrbyte tmp, cursor_ctrl
            sub cursor_ctrl, #2
            jmp #normal

ansi_DECTCEM_hide
            add cursor_ctrl, #2
            mov tmp, #0
            wrbyte tmp, cursor_ctrl
            sub cursor_ctrl, #2
            jmp #normal


' ANSI Select Graphic Rendition - we only support normal or non-normal (inverse)
ansi_SGR
            ' if there's any params, and if they are not zero, set inverse
            cmp csi_params, #1   wc
    if_nc   call #csi_pop
    if_nc   cmp csi_tmp, #0      wz
    if_nc_and_nz  mov inverse, #128
    if_nc_and_nz  jmp #normal
            ' otherwise, set 0
            mov inverse, #0
    if_z    jmp #ansi_ED_eod


' ANSI Cursor Position
ansi_CUP_H
            cmp csi_params, #1   wc
    if_c    mov cursor_x, #0
    if_nc   call #csi_pop
    if_nc   sub csi_tmp, #1
    if_nc   mov cursor_x, csi_tmp

            cmp csi_params, #1   wc
    if_c    mov cursor_y, #0
    if_nc   call #csi_pop
    if_nc   sub csi_tmp, #1
    if_nc   mov cursor_y, csi_tmp

            call #fix_cursor
            jmp #normal

' ANSI Cursor Position alternative (CATCHUM requires this weird version with -30, maybe wrong?)
ansi_CUP_f
            cmp csi_params, #1   wc
    if_c    mov cursor_x, #0
    if_nc   call #csi_pop
    if_nc   sub csi_tmp, #30
    if_nc   mov cursor_x, csi_tmp

            cmp csi_params, #1   wc
    if_c    mov cursor_y, #0
    if_nc   call #csi_pop
    if_nc   sub csi_tmp, #30
    if_nc   mov cursor_y, csi_tmp

            call #fix_cursor
            jmp #normal

ansi_DEBUG
            or    char, #128
            call  #outc
            cmp   csi_params, #1   wc
    if_nc   call  #csi_pop
    if_nc   mov   char, csi_tmp
    if_nc   call  #errorc
            cmp   csi_params, #1   wc
    if_nc   call  #csi_pop
    if_nc   mov   char, csi_tmp
    if_nc   call  #errorc

            jmp   #normal


tmp          long    0
char         long    0
char_tmp     long    0
inverse      long    0
src          long    0
dst          long    0
csi_tmp      long    0
csi_flags    long    0
csi_stack    long    0
csi_params   long    0
queue_ptr    long    0
head_ptr     long    0
head         long    0
tail_ptr     long    0
tail         long    0
cap          long    0
cap_mask     long    0

screen       long    0
fourspaces   long    $20202020
'fourinverse  long    $A0A0A0A0
scroll_cnt   long    0
cursor_ptr   long    0
cursor_ctrl  long    0
rows_ptr     long    0
cursor_x     long    0
cursor_y     long    0
rows         long    0
cols         long    0
term_state   long    0

csi_start    long    0