

; to be included by rom.asm

SHITAGAIN
	LD	HL, SHITSTR
	CALL	LCDPRINT

	LD	HL, SHOOTSTR
	CALL	LCDPRINT

	JP	SHITAGAIN

SHITSTR		DB	"This shit works", 10, 0
SHOOTSTR	DB	"Shoot my dog", 10, 0
