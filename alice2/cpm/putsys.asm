;Copies the memory image of CP/M loaded at E400h onto tracks 0 and 1 of the first CP/M disk
;Load and run from ROM monitor
;Uses calls to BIOS, in memory at FA00h
;XXX grantham; Alice 2 fake disk has 64 sectors per track, but it's okay to leave 26 here
;Writes track 0, sectors 2 to 26, then track 1, sectors 1 to 25
bios	equ	0DE00h
ccp	equ	0C800h
conout:  equ     bios + 0ch  ;console out
seldsk: equ     bios + 01bh  ;pass disk no. in c
setdma: equ     bios + 024h  ;pass address in bc
settrk: equ     bios + 01eh  ;pass track in reg C
setsec: equ     bios + 021h  ;pass sector in reg c
write:  equ     bios + 02ah  ;write one CP/M sector to disk
monitor_warm_start:     equ     046Fh   ;Return to ROM monitor
        org     0800h   ;First byte in RAM when memory in configuration 0
        ld      c,00h   ;CP/M disk a
        call    seldsk
;Write track 0, sectors 2 to 26
        ld      a,2     ;starting sector
        ld      (sector),a
        ld      hl,ccp       ;start of CCP
        ld      (address),hl
        ld      c,0     ;CP/M track lo
        ld      b,0     ;CP/M track hi
        call    settrk
wr_trk_0_loop:  ld      a,(sector)
        ld      c,a     ;CP/M sector lo
        ld      b,0     ;CP/M sector hi
        call    setsec
        ld      bc,(address)    ;memory location
        call    setdma
        call    write
        ld      a,(sector)
        cp      26      ;done:
        jp      z,wr_trk_1      ;yes, start writing track 1
        inc     a       ;no, next sector
        ld      (sector),a
        ld      hl,(address)
        ld      de,128
        add     hl,de
        ld      (address),hl
        jp      wr_trk_0_loop
;Write track 1, sectors 1 to 25
wr_trk_1:       ld      c,1 ; track lo
        ld      b, 0 ; track hi
        call    settrk
        ld      hl,(address)
        ld      de,128
        add     hl,de
        ld      (address),hl
        ld      a,1
        ld      (sector),a
wr_trk_1_loop:  ld      a,(sector)
        ld      c,a     ;CP/M sector hi
        ld      b,0     ;CP/M sector lo
        call    setsec
        ld      bc,(address)    ;memory location
        call    setdma
        call    write
        ld      a,(sector)
        cp      25
        jp      z,done
        inc     a
        ld      (sector),a
        ld      hl,(address)
        ld      de,128
        add     hl,de
        ld      (address),hl
        jp      wr_trk_1_loop
        ld      C, 'd'
        call     conout
        ld      C, 'o'
        call     conout
        ld      C, 'n'
        call     conout
        ld      C, 'e'
        call     conout
        ld      C, '.'
        call     conout
        ld      C, 0Ah
        call     conout
done:   jp      done
sector: db      00h
address:        dw      0000h
        end
