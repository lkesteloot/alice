
; This is the loader. It is copies to RAM by the preloader (see rom.asm).
; It is responsible for loading the first few sectors of the disk and
; running them.

; Defines loader:
#include "loader_address.asm"

; The port we use to interact with the pic.
PICPORT EQU     0

; Where we load the boot sectors to.
BOOT    EQU     0100H

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

        ; Load boot sector 0 from disk.
        ld      a, 0            ; Disk 0
        ld      de, 0           ; Sector 0
        ld      bc, 0           ; Track 0
        ld      hl, BOOT        ; Buffer
        call    ldsector

        ; Load boot sector 1 from disk.
        inc     de
        call    ldsector

        ; Inform user that we are about to jump.
        ld      hl, jmpmsg
        call    print

        ; Jump to boot sector.
        jp      BOOT

        ; ------------------------------------------
        ; Load a sector from disk.
        ; A = disk number
        ; DE = sector number
        ; BC = track number
        ; HL = buffer address with 128 bytes. This register
        ;      is left one byte past the end of the buffer.
        ;
        ; Halts on error.
ldsector:
        push    af
        push    bc

        ; Send command to load sector.
        push    bc
        ld      c, PICPORT

        push    af
        ld      a, 1
        out     (c), a          ; "Read sector"
        pop     af
        out     (c), a          ; Disk
        out     (c), e          ; Sector (LSB)
        out     (c), d          ; Sector (MSB)
        pop     de              ; Was "bc"
        out     (c), e          ; Track (LSB)
        out     (c), d          ; Track (MSB)

        ; Poll until we get a non-zero result.
ldwait:
        in      a, (PICPORT)
        cp      a
        jp      z, ldwait

        ; Check for failure (success = 1).
        ld      b, 1
        cp      b
        jp      nz, ldfail

        ; Load 128 bytes.
        ld      b, 128
        ld      c, PICPORT
        inir                        ; Read B bytes from C into HL

        pop     bc
        pop     af

        ret

        ; ------------------------------------------
        ; Inform user that RAM failed and halt.
ramfail: ld      hl, rambadmsg
        call    print
        halt

        ; ------------------------------------------
        ; Inform user that sector load failed and halt.
ldfail: ld      hl, ldfailmsg
        call    print
        halt

        ; ------------------------------------------
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

        ; ------------------------------------------
        ; Constants.

msg:    defm    'Alice 3 loader'
        defb    10,0

jmpmsg: defm    'About to jump to boot loader'
        defb    10,0

ramgoodmsg:
        defm    'RAM successfully swapped'
        defb    10,0

rambadmsg:
        defm    'RAM failed to swap'
        defb    10,0

ldfailmsg:
        defm    'Sector load failed'
        defb    10,0

	end
