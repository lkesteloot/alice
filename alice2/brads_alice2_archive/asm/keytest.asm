#include "library.asm"

;
; Main program
;
MAIN
	LD	IX, RET1
	JP	LCDINIT
RET1

	LD	HL, HELLOSTR
	LD	IX, RET2
	JP 	LCDPRINT
RET2

	LD	HL, 2		; 2 seconds wait
	LD	IX, RET3
	JP	WAITSECS
RET3
	; write all 

;
; A    - output
; IX   - return
;
KBDWRITE
	; ?
	JP (IX)

;
; In:
;    IX   - return address
; Out:
;    A    - input
;
KBDREAD
	; ?
	JP (IX)


HELLOSTR	DB	"Keytest program", 10
		DB	"Will try all 255", 10
		DB	"output bytes,", 10
		DB	"then will read.", 0

	END
