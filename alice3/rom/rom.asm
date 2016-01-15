
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

        ; Jump to further processing
        jp      more1

        org 08h
rst08vec:
        push hl
        ld hl, rst08msg
        jp handlerst

        org 10h
rst10vec:
        push hl
        ld hl, rst10msg
        jp handlerst

        org 18h
rst18vec:
        push hl
        ld hl, rst18msg
        jp handlerst

        org 20h
rst20vec:
        push hl
        ld hl, rst20msg
        jp handlerst

        org 28h
rst28vec:
        push hl
        ld hl, rst28msg
        jp handlerst

        org 30h
rst30vec:
        push hl
        ld hl, rst30msg
        jp handlerst

        org 38h
rst38vec:
        push hl
        ld hl, rst38msg
        jp handlerst

handlerst:
        ; we assume an RST was a fatal instruction because
        ; we dont expect to ever see one before CP/M boots

        push af         ; save whatever was in AF
        push bc         ; save whatever was in bc

        push hl         ; save HL containing the reset number string

        ld hl, rstmsg
        call print

        pop hl          ; restore HL containing the reset number string
        call print

        ld hl, frommsg
        call print

        ld hl, 7        ; high byte of PC pushed in RST
        add hl, sp 
        ld b, (hl)

        ld hl, 6        ; low byte of PC pushed in RST
        add hl, sp 
        ld c, (hl)

        dec bc          ; subtract 1 yielding the actual RST address
                        ; high byte in A

        ld a, b         ; get high byte into A
        call prbyte

        ld a, c         ; get low byte into A
        call prbyte

        ld      hl, eolmsg
        call    print

        ; pop bc          ; restore bc
        ; pop af          ; restore AF
        ; pop hl          ; restore HL pushed in RST vector

        halt

        ; ------------------------------------------
        ; Print an 8-bit value (register A) in hex.
prbyte: push    af

        ; High nybble.
        srl     a
        srl     a
        srl     a
        srl     a
        call    prnyb
        pop     af

        ; Low nybble.
        call    prnyb

        ret

        ; ------------------------------------------
        ; Print a 4-bit value (register A) in hex. Ignores
        ; the top nybble of A.
prnyb:  push    hl
        push    de
        push    af

        ; Look up in hex table.
        and     0x0F
        ld      hl, bin2hex
        ld      d, 0
        ld      e, a
        add     hl, de
        ld      a, (hl)
        out     (128), a

        pop     af
        pop     de
        pop     hl

        ret

more1:

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

        ; ------------------------------------------
        ; Print a nul-terminated string at HL.
print:  push    af
        push    hl

prtlp:  ld      a, (hl)
        or      a               ; See if it is a nul (\0).
        jp      z, prtend
        out     (128), a
        inc     hl
        jp      prtlp

prtend: pop     hl
        pop     af
        ret


msg:    defm    'Alice 3 preloader'
        defb    13,10,0

ramfailmsg: defm    'RAM check failed'
        defb    13,10,0

rampassmsg: defm    'RAM check passed'
        defb    13,10,0

; Messages to help in RST handler
rstmsg:        defm    'RST'
                defb    0
frommsg:        defm    ' from '
                defb    0
rst08msg:       defm    '08'
                defb    0
rst10msg:       defm    '10'
                defb    0
rst18msg:       defm    '18'
                defb    0
rst20msg:       defm    '20'
                defb    0
rst28msg:       defm    '28'
                defb    0
rst30msg:       defm    '30'
                defb    0
rst38msg:       defm    '38'
                defb    0

bin2hex:
        defm    '0123456789ABCDEF'

eolmsg:
        defb    13,10,0


; Defines ldrdat and ldrcnt:
#include "loader_data.asm"

	end
