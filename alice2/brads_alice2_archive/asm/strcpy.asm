
;
; char *strcpy(char *dst, char *src) -- just like C
;


_strcpy
	PUSH	IY
	LD	IY, 0
	ADD	IY, SP
	PUSH	AF
	PUSH	IX

	LD	L, (IY + 4)	; destination into IX
	LD	H, (IY + 5)
	PUSH	HL
	PUSH	HL
	POP	IX

	LD	L, (IY + 6)	; source into HL
	LD	H, (IY + 7)

STRCPY_NEXT_BYTE
	LD	A, (HL)
	LD	(IX), A
	INC	HL
	INC	IX

	CP	0
	JP	NZ, STRCPY_NEXT_BYTE

	POP	HL		; return the destination

	POP	IX
	POP	AF
	POP	IY

	RET

