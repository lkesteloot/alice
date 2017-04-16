
typedef enum TouchscreenEvent
{
    TOUCHSCREEN_IDLE,
    TOUCHSCREEN_START,
    TOUCHSCREEN_DRAG,
    TOUCHSCREEN_STOP
} TouchscreenEvent;

int touchscreen_init();

TouchscreenEvent touchscreen_read(int *x, int *y, float *z);

