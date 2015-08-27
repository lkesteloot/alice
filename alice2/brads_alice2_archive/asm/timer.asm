
;
; int get_timer_counter();
;

_get_timer_counter
        PUSH    AF
        LD      A, (TIMER_COUNTER)
        LD      L, A
        LD      H, 0
        POP     AF

	RET

