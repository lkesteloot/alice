
;
; STRCPY - Copies HL into IX.
;
; Leaves IX at terminating 0.  No other
; registers are modified.
;
; 
;

STRCPY
	PUSH	HL
	PUSH	AF

STRCPY_LOOP
	LD	A, (HL)
	LD	(IX), A
	INC	HL
	INC	IX

	CP	0
	JP	NZ, STRCPY_LOOP

	DEC	IX

	POP	AF
	POP	HL
	RET

;----------------------------------------
