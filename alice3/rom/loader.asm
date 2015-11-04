
; This is the loader. It is copies to RAM by the preloader (see rom.asm).
; It is responsible for loading the first few sectors of the disk and
; running them.

; Defines loader:
#include "loader_address.asm"

        org     loader

        ; Disable interrupts.
        di

        ; Set up the stack at the top of RAM.
        ld      sp, 0

        ; Display message.
        ld      hl, msg
msglp:  ld      a, (hl)
        or      a               ; See if it is a nul (\0).
        jp      z, msgend
        out     (128), a
        inc     hl
        jp      msglp
msgend:

        ; Load boot sector from disk.


        ; Jump to boot sector.

        halt    ; XXX remove

msg:    defm    'Alice 3 loader'
        defb    10,0

	end
