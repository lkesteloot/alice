#define SCREEN_ROWBYTES 22 
#define SCREEN_FIRST_VISIBLE_ROW 20
#define SCREEN_FIRST_VISIBLE_COLUMN 3
#define SCREEN_FIRST_VISIBLE_BYTE_OFFSET (SCREEN_ROWBYTES * SCREEN_FIRST_VISIBLE_ROW + SCREEN_FIRST_VISIBLE_COLUMN)

GFX_MEM        EQU     04000H          ; framebuffer
GFX_FB         EQU     GFX_MEM + SCREEN_FIRST_VISIBLE_BYTE_OFFSET ; start of visible region 

        ; GFX_BUF from rom.asm
GRAYBAR_PTR     EQU GFX_BUF + 0

;
; initialize graphics
;
GFX_INIT
	PUSH	AF
	PUSH	BC
	PUSH	DE
	PUSH	HL

        LD      HL, GFX_FB
        LD      (GRAYBAR_PTR), HL

	POP	HL
	POP	DE
	POP	BC
	POP	AF
        RET

;
;
;
GFX_GRAYBAR
	PUSH	AF
	PUSH	BC
	PUSH	DE
	PUSH	HL

        LD      C, 0FFH         ; C = 0xff

	LD	HL, (GRAYBAR_PTR)
                                ; HL = GRAYBAR_PTR
        LD      B, A            ; B = A

GRAYLP1
        LD      (HL), C         ; (HL) = C
	INC	HL              ; increment HL
        DJNZ    GRAYLP1         ; if(B--) goto GRAYLP1

	LD	HL, (GRAYBAR_PTR)
        LD      BC, SCREEN_ROWBYTES
        ADD     HL, BC
	LD	(GRAYBAR_PTR), HL

	POP	HL
	POP	DE
	POP	BC
	POP	AF
        RET

;
;
;
GFX_CLEAR

	PUSH	AF
	PUSH	BC
	PUSH	DE
	PUSH	HL

        LD      C, 0            ; C = 0;

	LD	A, 64 		; A = 64; // 64 * 256 bytes

	LD	HL, GFX_MEM

CLRLP1
	LD	B, 0		; b = 0; 256 bytes

CLRLP2
	LD	(HL), C         ; store 0 byte
	INC	HL              ; increment HL
	DJNZ	CLRLP2          ; inner loop continue

	DEC	A
	JP	NZ, CLRLP1      ; outer loop continue

	POP	HL
	POP	DE
	POP	BC
	POP	AF

	RET
