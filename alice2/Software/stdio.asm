
;
; GETS - Inputs a string into the
;   buffer pointed to by HL.  String is
;   NULL-terminated.
;
; No registers are modified.
;

GETS
	PUSH	AF
	PUSH	HL
	PUSH	BC
	PUSH	DE
	PUSH	IX
	PUSH	IY

	PUSH	HL
	POP	IX	 ; IX = end of buffer
	LD	(IX), 0

GETS_LOOP ; wait for key
	LD	A, (NUMKEY)
	CP	0
	JP	NZ, GOT_KEY

	LD	A, (SHOULD_RUN)
	CP	0
	JP	Z, GETS_LOOP

	; comment this in for the next burn-in
	; LD	A, 0
	; LD	(SHOULD_RUN), A

	LD	HL, (CURRENT_ADDR)
	JP	(HL)

GOT_KEY
	DI
	LD	A, (NUMKEY)	; in case it changed before DI
	LD	IY, KEYBUF
	LD	B, A

GETS_COPY
	LD	A, (IY)

	CP	8  ; backspace
	JP	Z, BACKSPACE
	CP	127  ; delete
	JP	NZ, NOT_BACKSPACE

BACKSPACE
	; do not backspace past beginning of buffer
	PUSH	IX	; move IX into DE
	POP	DE
	SCF		; carry = 1
	CCF		; carry = 0
	PUSH	HL	; save HL
	SBC	HL, DE	; HL -= DE
	POP	HL	; restore HL
	JP	Z, NEXT_KEY ; do not backspace if IX == HL

	DEC	IX
	JP	NEXT_KEY

NOT_BACKSPACE
	LD	(IX), A
	INC	IX

NEXT_KEY
	CP	10 ; return (keyboard)
	JP	Z, GETS_DONE

	CP	13 ; return (maybe serial)
	JP	Z, GETS_DONE

	INC	IY

	DJNZ	GETS_COPY

	LD	A, 0
	LD	(NUMKEY), A
	EI

	LD	(IX), 0
	CALL	LCDPRINT

	JP	GETS_LOOP

GETS_DONE
	LD	A, 0
	LD	(NUMKEY), A
	EI

	LD	(IX), 0

	POP	IY
	POP	IX
	POP	DE
	POP	BC
	POP	HL
	POP	AF
	RET

;----------------------------------------
