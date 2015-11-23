;
;       modified by Donn Stewart, dstew@cpuville.com, for his Z-80 computer
;
;       modified by Brad Grantham, grantham@plunk.org, for an
;       idealized HW implementation in a Z-80 computer emulator,
;       September 2015.
;
;	skeletal cbios for first level of CP/M 2.0 alteration
;
; 20150922 grantham - Assemble cpm22.asm, then come here and set "bios"
; to whatever value BOOT has in the PRN output for cpm22.asm!

ccp:    equ     0E400h          ;base of ccp
bdos:   equ     0EC06h          ;bdos entry
bios:   equ     0FA00h          ;base of bios
cdisk:	equ	0004h		;address of current disk number 0=a,... l5=p
iobyte:	equ	0003h		;intel i/o byte
disks:	equ	04h		;number of disks in the system

;
; Alice 3 HW definitions
;
propeller_port:         equ     128     ; access to Propeller-hosted I/O
pic_port:               equ     0       ; access to PIC-hosted I/O
pic_poll_again:         equ     0       ; operation not complete on PIC
pic_success:            equ     1       ; operation succeeded on PIC
pic_failure:            equ     0FFh    ; operation succeeded on PIC
pic_ready:              equ     1       ; operation succeeded on PIC
pic_notready:           equ     0FFh    ; operation succeeded on PIC
pic_cmd_read:           equ     1       ; READ sector
pic_cmd_write:          equ     2       ; WRITE sector
pic_cmd_const:          equ     3       ; CONST
pic_cmd_conin:          equ     4       ; CONIN - DOES NOT BLOCK, POLL CONST
pic_cmd_serout:         equ     5       ; SEROUT
pic_cmd_read_sum:       equ     6       ; READ sector and give checksum
pic_cmd_write_sum:      equ     7       ; WRITE sector but verify sum first


	org	bios		;origin of this program
nsects:	equ	($-ccp + 127)/128	;warm start sector count

;
;	jump vector for individual subroutines
;
	JP	boot	;cold start
wboote:	JP	wboot	;warm start
	JP	const	;console status
	JP	conin	;console character in
	JP	conout	;console character out
	JP	list	;list character out
	JP	punch	;punch character out
	JP	reader	;reader character out
	JP	home	;move head to home position
	JP	seldsk	;select disk
	JP	settrk	;set track number
	JP	setsec	;set sector number
	JP	setdma	;set dma address
	JP	read	;read disk
	JP	write	;write disk
	JP	listst	;return list status
	JP	sectran	;sector translate
;
; XXX grantham
; fixed data tables for fake hw disks
; no translations
;
;	disk Parameter header for disk 00
dpbase:	defw	0000h, 0000h
	defw	0000h, 0000h
	defw	dirbf, dpblk
	defw	chk00, all00
;	disk parameter header for disk 01
	defw	0000h, 0000h
	defw	0000h, 0000h
	defw	dirbf, dpblk
	defw	chk01, all01
;	disk parameter header for disk 02
	defw	0000h, 0000h
	defw	0000h, 0000h
	defw	dirbf, dpblk
	defw	chk02, all02
;	disk parameter header for disk 03
	defw	0000h, 0000h
	defw	0000h, 0000h
	defw	dirbf, dpblk
	defw	chk03, all03
;
dpblk:	;disk parameter block for all disks.
	defw	64		;SPT - sectors per track
        defm	6		;BSH - block shift factor
	defm	0x3f		;BLM - block mask
	defm	3		;EXM - extent mask
	defw	1023		;DSM - disk size-1 in blocks
	defw	1023		;DRM - directory max
	defm	255		;AL0 - alloc 0
	defm	255		;AL1 - alloc 1
	defw	0		;CKS - check size
	defw	2		;OFF - track offset

;
;       messages
;
SIGNON: DEFM    '64K CP/M 2.2 for the Alice 3'
        DEFB    13,10,0
GOCCPMSG: DEFM    'GOCCP!'
        DEFB    13,10,0
BADSUM: DEFM    'SECTOR SUM MISMATCH!'
        DEFB    13,10,0


;	end of fixed tables
;    

;
;       borrowed by Brad from Udo Monks bios-qpm.asm
;       print a 0 terminated string to console device
;       pointer to string in HL
;
PRTMSG: 
        PUSH    AF
        PUSH    BC
PRTMSGLOOP:
        LD      A,(HL)
        OR      A
        JP      Z, PRTMSGRET
        LD      C,A
        CALL    conout
        INC     HL
        JP      PRTMSGLOOP
PRTMSGRET:
        POP     BC
        POP     AF
        RET

;	individual subroutines to perform each function
boot:	;simplest case is to just perform parameter initialization
        LD      SP,80H          ;use space below buffer for stack
        LD      HL,SIGNON       ;print message
        CALL    PRTMSG
	XOR	a		;zero in the accum
	LD	(iobyte),A	;clear the iobyte
	LD	(cdisk),A	;select disk zero
	JP	gocpm		;initialize and go to cp/m
;
wboot:	;simplest case is to read the disk until all sectors loaded
	LD	SP, 80h		;use space below buffer for stack
	LD 	c, 0		;select disk 0
	call	seldsk
	call	home		;go to track 00
;
	LD 	b, nsects	;b counts * of sectors to load
	LD 	c, 0		;c has the current track number
	LD 	d, 2		;d has the next sector to read
;	note that we begin by reading track 0, sector 2 since sector 1
;	contains the cold start loader, which is skipped in a warm start
	LD	HL, ccp		;base of cp/m (initial load point)
load1:	;load	one more sector
	PUSH	BC		;save sector count, current track
	PUSH	DE		;save next sector to read
	PUSH	HL		;save dma address
	LD 	c, d		;get sector address to register C
        LD      b, 0
	call	setsec		;set sector address from register C
	pop	BC		;recall dma address to b, C
	PUSH	BC		;replace on stack for later recall
	call	setdma		;set dma address from b, C
;
;	drive set to 0, track set, sector set, dma address set
	call	read
	CP	00h		;any errors?
	JP	NZ,wboot	;retry the entire boot if an error occurs
;
;	no error, move to next sector
	pop	HL		;recall dma address
	LD	DE, 128		;dma=dma+128
	ADD	HL,DE		;new dma address is in h, l
	pop	DE		;recall sector address
	pop	BC		;recall number of sectors remaining, and current trk
	DEC	b		;sectors=sectors-1
	JP	Z,gocpm		;transfer to cp/m if all have been loaded
;
;	more	sectors remain to load, check for track change
	INC	d
	LD 	a,d		;sector=65?, if so, change tracks
	CP	65
	JP	C,load1		;carry generated if sector<65
;
;	end of	current track,	go to next track
	LD 	d, 1		;begin with first sector of next track
	INC	c		;track=track+1
;
;	save	register state, and change tracks
	PUSH	BC
	PUSH	DE
	PUSH	HL
        LD      B, 0
	call	settrk		;track address set from register c
	pop	HL
	pop	DE
	pop	BC
	JP	load1		;for another sector
;
;	end of	load operation, set parameters and go to cp/m
gocpm:
	LD 	a, 0c3h		;c3 is a jmp instruction
	LD	(0),A		;for jmp to wboot
	LD	HL, wboote	;wboot entry point
	LD	(1),HL		;set address field for jmp at 0
;
	LD	(5),A		;for jmp to bdos
	LD	HL, bdos	;bdos entry point
	LD	(6),HL		;address field of Jump at 5 to bdos
;
	LD	BC, 80h		;default dma address is 80h
	call	setdma
;
	; ei			;enable the interrupt system
	LD	A,(cdisk)	;get current disk number
	cp	disks		;see if valid disk number
	jp	c,diskok	;disk valid, go to ccp
	ld	a,0		;invalid disk, change to disk 0
diskok:	
        ; PUSH    HL
        ; LD      HL,GOCCPMSG       ;print message
        ; CALL    PRTMSG
        ; POP     HL
        LD 	c, a		;send to the ccp
	JP	ccp		;go to cp/m for further processing

; wait until PIC is ready
pic_poll:
	in 	a,(pic_port)	;get status
        and     0ffh
	jp 	z,pic_poll	;loop until success or failure
        ret


const:	;console status, return 0ffh if character ready, 00h if not
	ld	a,pic_cmd_const
	out 	(pic_port), a   ; request status
        call    pic_poll
	cp	pic_ready	; compare with success
        ret     z               ; if equal, return (a is already 0xff)
        ld      a, 0            ; otherwise set a to 0 (fail) and return
	ret

conin:	;console character into register a
	ld	a,pic_cmd_const
	out 	(pic_port), a   ; request status
        call    pic_poll
        cp      pic_ready       ; compare with ready
        jp      nz, conin       ; if not ready, request again

	ld	a,pic_cmd_conin
	out 	(pic_port), a   ; get character
        call    pic_poll        ; Okay to assume this will succeed
	in 	a,(pic_port)	; get char
	AND	07fh		; strip parity bit
	ret

conout:	;console character output from register c
	ld	a,c		;get the char
	out	(propeller_port),a		;out to port
	; ld	a,pic_cmd_conout
	; out	(pic_port),a		;out to port
	; ld	a,c		;get the char
	; out	(pic_port),a		;out to port
	ret

list:	;list character from register c
	LD 	a, c	  	;character to register a
	ret		  	;null subroutine

listst:	;return list status (0 if not ready, 1 if ready)
	XOR	a	 	;0 is always ok to return
	ret

punch:	;punch	character from	register C
	LD 	a, c		;character to register a
	ret			;null subroutine

reader:	;reader character into register a from reader device
	LD     a, 1ah		;enter end of file for now (replace later)
	AND    7fh		;remember to strip parity bit
	ret

;
;	i/o drivers for the disk follow
;	for now, we will simply store the parameters away for use
;	in the read and write	subroutines
;
home:	;move to the track 00	position of current drive
;	translate this call into a settrk call with Parameter 00
	LD     bc, 0		;select track 0
	call   settrk
	ret			;we will move to 00 on first read/write

seldsk:	;select disk given by register c
	LD	HL, 0000h	;error return code
	LD 	a, c
	LD	(diskno),A
	CP	disks		;must be between 0 and 3
	RET	NC		;no carry if 4, 5,...
;	disk number is in the proper range
;	defs	10		;space for disk select
;	compute proper disk Parameter header address
	LD	A,(diskno)
	LD 	l, a		;l=disk number 0, 1, 2, 3
	LD 	h, 0		;high order zero
	ADD	HL,HL		;*2
	ADD	HL,HL		;*4
	ADD	HL,HL		;*8
	ADD	HL,HL		;*16 (size of each header)
	LD	DE, dpbase
	ADD	HL,DE		;hl=,dpbase (diskno*16) Note typo here in original source.
	ret

settrk:	;set track given by register c
	LD	(track),BC
	ret

setsec:	;set sector given by register c
	LD	(sector),BC
	ret

sectran:
	;translate the sector given by bc using the
	;translate table given by de
	EX	DE,HL		;hl=.trans
	ADD	HL,BC		;hl=.trans (sector)
	ret			;debug no translation

setdma:	;set	dma address given by registers b and c
	LD	(dmaad),BC	;save the address
	ret

;Read one CP/M sector from disk.
;Return a 00h in register a if the operation completes properly, and 01h if an error occurs during the read.
;Disk number in 'diskno'
;Track number in 'track'
;Sector number in 'sector'
;Dma address in 'dmaad' (0-65535)

read:
        push    bc
        push    de
        push    hl
        push    ix

read_again:

	ld	a,pic_cmd_read_sum
	out 	(pic_port), a   ; request read

        ld      a,(diskno)
	out 	(pic_port), a   ; disk byte

        ld      a,(sector + 0)
	out 	(pic_port), a   ; sector lo
        ld      a,(sector + 1)
	out 	(pic_port), a   ; sector hi

        ld      a,(track + 0)
	out 	(pic_port), a   ; track lo
        ld      a,(track + 1)
	out 	(pic_port), a   ; track hi

        call    pic_poll
	cp	pic_success	; compare with success
        JP      z,read_continue ; if success, continue reading

        ; failure
        ld      a, 01H
        jp      read_return

read_continue:
        
	ld	hl,(dmaad)	; memory location to place data read from disk
        ld	b,128		; size of CP/M sector
        ld	c,pic_port

        ld      a,0
        ld      ix, 0         ; set counter to 0
        ld      de, 0         ; set addend to 0

read_loop:
        in      a,(pic_port)    ; get sector byte
        ld	(hl),a		; put in memory
        ld      e, a
        add     ix, de          ; add to checksum
        inc     hl              ; increment hl to next buffer location
        djnz	read_loop

        in      l, (c)          ; get packet checksum low
        in      h, (c)          ; get packet checksum high
        defm    0ddh, 05dh          ; ld      e,ixl
        defm    0ddh, 054h          ; ld      d,ixh

        scf                     ; set carry flag
        ccf                     ; and complement, thus clearing carry flag
        sbc     hl, de          ; hl -= de (and set Z flag if zero)
        jp      z, read_success


        PUSH    HL
        LD      HL,BADSUM       ;print message
        CALL    PRTMSG
        POP     HL
        JP      read_again

        ; success
read_success:
        ld      a, 0

read_return:
        pop    ix
        pop    hl
        pop    de
        pop    bc
        ret

;Write one CP/M sector to disk.
;Return a 00h in register a if the operation completes properly, and 0lh if an error occurs during the read or write
;Disk number in 'diskno'
;Track number in 'track'
;Sector number in 'sector'
;Dma address in 'dmaad' (0-65535)
write:
	ld	a,pic_cmd_write
	out 	(pic_port), a   ; request write

        ld      a,(diskno)
	out 	(pic_port), a   ; disk byte

        ld      a,(sector + 0)
	out 	(pic_port), a   ; sector L
        ld      a,(sector + 1)
	out 	(pic_port), a   ; sector H

        ld      a,(track + 0)
	out 	(pic_port), a   ; track L
        ld      a,(track + 1)
	out 	(pic_port), a   ; track H

	ld	hl,(dmaad)	; memory location to place data to disk
        ld	b,128		; size of CP/M sector
        ld	c,pic_port

        ; outir                 ; magic Z80 looping OUT instruction
write_loop: ; I think this whole loop could be an OUTIR instruction
        ld	a,(hl)		; get from memory
        out     (c),a           ; write sector byte to disk
        inc     hl              ; hl to next buffer byte
        djnz	write_loop

        call    pic_poll
	cp	pic_success	; compare with success
        JP      z,write_success  ; if success, continue

        ; failure
        ld      a, 01H
        ret

write_success:
        ; success
        ld      a, 0
        ret

;	the remainder of the cbios is reserved uninitialized
;	data area, and does not need to be a Part of the
;	system	memory image (the space must be available,
;	however, between"begdat" and"enddat").
;
track:	defs	2		;two bytes for expansion
sector:	defs	2		;two bytes for expansion
dmaad:	defs	2		;direct memory address
diskno:	defs	1		;disk number 0-15
;
;	scratch ram area for bdos use
begdat:	equ	$	 	;beginning of data area
dirbf:	defs	128	 	;scratch directory area
all00:	defs	31	 	;allocation vector 0
all01:	defs	31	 	;allocation vector 1
all02:	defs	31	 	;allocation vector 2
all03:	defs	31	 	;allocation vector 3
chk00:	defs	16		;check vector 0
chk01:	defs	16		;check vector 1
chk02:	defs	16	 	;check vector 2
chk03:	defs	16	 	;check vector 3
;
enddat:	equ	$	 	;end of data area
datsiz:	equ	$-begdat;	;size of data area
hstbuf: ds	256		;buffer for host disk sector
	end
