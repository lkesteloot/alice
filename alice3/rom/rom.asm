
; This is the preloader. It is located at address 0 of the ROM and runs when the
; machine is cold-booted. It is responsible for copying the loader to RAM and
; jumping to it.

; Defines loader:
#include "loader_address.asm"

; We assume RAM goes until 0FFFFH.
ramfirst equ     08000H

        org     0

        ; Disable interrupts.
        di

        ; Set up the stack at the top of RAM.
        ld      sp, 0

        ; Display message. Do not use a subroutine for this
        ; because that would require RAM for the stack, and
        ; we are not yet sure that RAM works.
        ld      hl, msg
msglp:  ld      a, (hl)
        or      a               ; See if it is a nul (\0).
        jp      z, msgend
        out     (128), a
        inc     hl
        jp      msglp
msgend:

        ; Test RAM. First write all of memory.
        ld      hl, ramfirst
        sub     a
memlp:
        ld      (hl), a
        inc     a
        cp      0FBH            ; 251 is prime.
        jp      nz, memnoreset
        sub     a
memnoreset:
        inc     hl
        ; If H is 0, then we have wrapped around.
        ld      b, a            ; Save A
        sub     a               ; Clear A
        cp      h               ; Check H
        jp      z, memdone
        ld      a, b            ; Restore A
        jp      memlp
memdone:

        ; Now verify all of memory.
        ld      hl, ramfirst
        sub     a
memlp2:
        ld      b, (hl)
        cp      b
        jp      nz, ramnotok

        inc     a
        cp      0FBH            ; 251 is prime.
        jp      nz, memnoreset2
        sub     a
memnoreset2:
        inc     hl
        ; If H is 0, then we have wrapped around.
        ld      b, a            ; Save A
        sub     a               ; Clear A
        cp      h               ; Check H
        jp      z, memdone2
        ld      a, b            ; Restore A
        jp      memlp2
memdone2:
        jp      ramok

ramnotok:
	; RAM not okay. Store the offending address at 0 and 0x8000,
	; store unexpected value at 2 and 0x8002, then print message
	; and halt.  Debugger can examine those addresses.  Specifically,
	; IO monitor "1k" command will dump the low 1K.
        ;
        ld      (0), hl
        ld      (08000H), hl
        ld      a, b
        ld      (2), a
        ld      (08002H), a

        ld      hl, ramfailmsg
msglp2: ld      a, (hl)
        or      a               ; See if it is a nul (\0).
        jp      z, msgend2
        out     (128), a
        inc     hl
        jp      msglp2
msgend2:
        halt

ramok:
        ; RAM OK. Print message and continue.
        ld      hl, rampassmsg
msglp3: ld      a, (hl)
        or      a               ; See if it is a nul (\0).
        jp      z, msgend3
        out     (128), a
        inc     hl
        jp      msglp3
msgend3:

        ; Copy loader to RAM.
        ld      hl, ldrdat
        ld      bc, ldrcnt
        ld      de, loader
        ldir                    ; Copy BC bytes from HL to DE.

        ; Jump to loader.
        jp      loader

msg:    defm    'Alice 3 preloader'
        defb    13,10,0

ramfailmsg: defm    'RAM check failed'
        defb    13,10,0

rampassmsg: defm    'RAM check passed'
        defb    13,10,0

; Defines ldrdat and ldrcnt:
#include "loader_data.asm"

	end
