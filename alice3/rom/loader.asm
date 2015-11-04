
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
        call    print

        ; Replace ROM with RAM.
        ld      hl, 0
        ld      (hl), 0

        ; Check that enabling RAM has worked.
        ld      b, 0
        ld      (hl), b
        ld      a, (hl)
        cp      b
        jp      nz, ramfail

        ld      b, 0FFH
        ld      (hl), b
        ld      a, (hl)
        cp      b
        jp      nz, ramfail

        ; Inform user that RAM succeeded.
        ld      hl, ramgoodmsg
        call    print

        ; Load boot sector from disk.


        ; Jump to boot sector.

        halt    ; XXX remove

        ; Inform user that RAM failed and halt.
ramfail: ld      hl, rambadmsg
        call    print
        halt

        ; Print a nul-terminated string at HL.
print:  push    af
        push    hl

prtlp:  ld      a, (hl)
        cp      a               ; See if it is a nul (\0).
        jp      z, prtend
        out     (128), a
        inc     hl
        jp      prtlp

prtend: pop     hl
        pop     af
        ret

msg:    defm    'Alice 3 loader'
        defb    10,0

ramgoodmsg:
        defm    'RAM successfully swapped'
        defb    10,0

rambadmsg:
        defm    'RAM failed to swap'
        defb    10,0

	end
