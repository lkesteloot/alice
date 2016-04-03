        ORG 0
MAIN

	; write 0 to 0x8000
	; write 0xCAFE to 0x8000
	; must read back 0xCAFE
	LD	BC, 0H
	LD	(8000H), BC
	LD	BC, 0cafeH
	LD	(8000H), BC
	LD	HL, (8000H)
        SCF
        CCF
	SBC	HL, BC
	JP	Z, YES
        JP      NO

YES
	LD	A, 0FFH
FASTLOOP
	OUT     (0), A
	JP	FASTLOOP


NO
	LD	A, 00H
SLOWLOOP
	OUT     (0), A
        NOP
        NOP
        NOP
	JP	SLOWLOOP

        END
