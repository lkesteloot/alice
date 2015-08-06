
;
; char *gets(char *buffer) -- just like C
;


_gets
	PUSH	IY
	LD	IY, 0
	ADD	IY, SP

	LD	L, (IY + 4)	; buffer into HL
	LD	H, (IY + 5)

	PUSH	HL
	CALL	GETS
	POP	HL		; so that return value is buffer

	POP	IY

	RET
