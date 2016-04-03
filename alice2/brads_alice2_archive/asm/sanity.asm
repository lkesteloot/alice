        ORG 0
MAIN
	LD	A, 0H
	LD	A, 0H
LOOP
	OUT     (0), A
	JP	LOOP
        END
