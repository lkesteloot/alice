;written by Donn Stewart, dstew@cpuville.com, for his Z-80 computer
;modified by Brad Grantham, grantham@plunk.org, for Alice 3 Z-80 computer
;
;Copies the memory image of CP/M loaded at E400h onto track 0 of disk 1
;Load and run from emulator using debugger
;Uses calls to BIOS
;XXX grantham; Alice 2 fake disk has 64 sectors per track, so write memory to 56 sectors starting at track 0, sector 2
ccp:    equ     0E400h          ;base of ccp
bdos:   equ     0EC06h          ;bdos entry
bios:   equ     0FA00h          ;base of bios
conout:  equ     bios + 0ch  ;console out
seldsk: equ     bios + 01bh  ;pass disk no. in c
setdma: equ     bios + 024h  ;pass address in bc
settrk: equ     bios + 01eh  ;pass track in reg C
setsec: equ     bios + 021h  ;pass sector in reg c
write:  equ     bios + 02ah  ;write one CP/M sector to disk
        org     0800h   ;First byte in RAM when memory in configuration 0
        ld      c,00h   ;CP/M disk a
        call    seldsk
;Write track 0, sectors 2 to 57
        ld      a,2     ;starting sector
        ld      (sector),a
        ld      hl,ccp       ;start of CCP
        ld      (address),hl
        ld      c,0     ;CP/M track lo
        ld      b,0     ;CP/M track hi
        call    settrk
wr_trk_loop:  ld      a,(sector)
        ld      c,a     ;CP/M sector lo
        ld      b,0     ;CP/M sector hi
        call    setsec
        ld      bc,(address)    ;memory location
        call    setdma
        call    write
        ld      a,(sector)
        cp      57      ;done:
        jp      z,done      ;done
        inc     a       ;no, next sector
        ld      (sector),a
        ld      hl,(address)
        ld      de,128
        add     hl,de
        ld      (address),hl
        jp      wr_trk_0_loop

done:   ld      C, 'd'
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
stop:   jp      stop
sector: db      00h
address:        dw      0000h
        end
