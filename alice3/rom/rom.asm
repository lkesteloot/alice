
; This is the preloader. It is located at address 0 of the ROM and runs when the
; machine is cold-booted. It is responsible for copying the loader to RAM and
; jumping to it.

; Defines loader:
#include "loader_address.asm"

        org     0

        ; Disable interrupts.
        di

        ; Set up the stack at the top of RAM.
        ld      sp, 0

        ; Display message.
        ld      hl, msg
msglp:  ld      a, (hl)
        cp      a               ; See if it is a nul (\0).
        jp      z, msgend
        out     (128), a
        inc     hl
        jp      msglp
msgend:

        ; Copy loader to RAM.
        ld      hl, ldrdat
        ld      bc, ldrcnt
        ld      de, loader
        ldir                    ; Copy BC bytes from HL to DE.

        ; Jump to loader.
        jp      loader

msg:    defm    'Alice 3 preloader'
        defb    10,0

; Defines ldrdat and ldrcnt:
#include "loader_data.asm"

	end
