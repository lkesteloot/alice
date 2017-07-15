
; This CP/M loader lives in the first two sectors of the disk. It loads
; the rest of CP/M into memory and jumps to it.

; Where CP/M lives.
CPM_ADDR EQU 0E400H

; Where BIOS lives.
BIOS_ADDR EQU 0FA00H

; The port we use to interact with the pic.
PICPORT EQU     0

        ; We are loaded at 0x100.
        org     0100H

        ; Disable interrupts.
        di

        ; Set up the stack at the top of RAM.
        ld      sp, 0

        ; Display message.
        ld      hl, msg
        call    print

        ; Load 44 sectors from disk, starting at sector 2.
        ld      a, 0            ; Disk 0
        ld      de, 2           ; Sector 2
        ld      hl, CPM_ADDR    ; Buffer

        ; Loop to load sectors.
        ld      b, 44
cpmlp:
        push    bc
        ld      bc, 0           ; Track 0
        call    ldsector
        pop     bc
        inc     de
        djnz    cpmlp           ; Do "b" times.

        ; Load 8 sectors from disk, starting at sector 0 on track 1.
        ld      a, 0            ; Disk 0
        ld      de, 0           ; Sector 2
        ld      hl, BIOS_ADDR   ; Buffer

        ; Loop to load sectors.
        ld      b, 8
bioslp:
        push    bc
        ld      bc, 1           ; Track 1
        call    ldsector
        pop     bc
        inc     de
        djnz    bioslp          ; Do "b" times.

        ; Inform user that we are about to jump.
        ld      hl, jmpmsg
        call    print

        ; Jump to CP/M.
        jp      BIOS_ADDR

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
        push    de
        push    ix

        ; Send command to load sector.
        push    bc
        ld      c, PICPORT

        push    af
        ld      a, 6
        out     (c), a          ; "Read sector with 2-byte checksum"
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
        or      a
        jp      z, ldwait

        ; Check for failure (success = 1).
        ld      b, 1
        cp      b
        jp      nz, ldfail

        ; Save buffer pointer
        push    hl

        ; Load 128 bytes.
        ld      b, 128
        ld      c, PICPORT
        inir                        ; Read B bytes from C into HL

        ; Restore buffer pointer
        pop    hl

        ; Loop over buffer to calculate checksum
        ld      b, 128          ; 128 bytes
        ld      ix, 0           ; set 16-bit counter to 0
        ld      de, 0           ; set temp byte to 0

sumlp:
        ld      e, (hl)         ; e = next byte
        add     ix, de          ; add to checksum
        inc     hl              ; increment hl to next buffer location
        djnz	sumlp

        push    hl

        in      l, (c)          ; get packet checksum low
        in      h, (c)          ; get packet checksum high
        defm    0ddh, 05dh      ; ld      e,ixl
        defm    0ddh, 054h      ; ld      d,ixh

        scf                     ; set carry flag
        ccf                     ; and complement, thus clearing carry flag
        sbc     hl, de          ; hl -= de (and set Z flag if zero)
        jp      nz, ldfailsum   ; so if hl != de, fail.

        pop     hl
        pop     ix
        pop     de
        pop     bc
        pop     af

        ret

        ; ------------------------------------------
        ; Inform user that sector load failed and halt.
ldfail: ld      hl, ldfailmsg
        call    print
        halt

        ; ------------------------------------------
        ; Inform user that sector load failed and halt.
ldfailsum:
        ld      hl, ldfailsummsg
        call    print
        halt

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

        ; ------------------------------------------
        ; Constants.

msg:    defm    'Alice 3 CP/M loader'
        defb    13,10,0

jmpmsg: defm    'About to jump to CP/M'
        defb    13,10,0

ldfailmsg:
        defm    'Sector load failed'
        defb    13,10,0

ldfailsummsg:
        defm    'Sector sum!'
        defb    13,10,0

	end
