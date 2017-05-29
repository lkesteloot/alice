#ifndef __AWESOME_H__
#define __AWESOME_H__

// Interface to the FPGA GPU.

#include <stdint.h>

#define AWESOME_MAX_BRIGHTNESS 1000

// Draw type, for awesome_draw() function:
#define AWESOME_DRAW_TRIANGLES 0
#define AWESOME_DRAW_LINES 1
#define AWESOME_DRAW_POINTS 2
#define AWESOME_DRAW_LINE_STRIP 3
#define AWESOME_DRAW_TRIANGLE_STRIP 5
#define AWESOME_DRAW_TRIANGLE_FAN 6

typedef struct {
    // These are all private. Do not access.
    volatile uint32_t *gpo;
    volatile uint32_t *gpi;
    volatile uint64_t *command_buffer;
    int home_button;
    int rasterizer_buffer_number;
} Awesome;

// Initializes the library, opening the connection to the FPGA.
// Exits the program with a message if something goes wrong.
void awesome_init(Awesome *awesome);

// Address of start of command buffer.
volatile uint64_t *awesome_get_command_buffer(Awesome *awesome);

// Call this before the drawing routines.
void awesome_init_frame(Awesome *awesome);

// Call these after the drawing routines.
void awesome_start_rasterizing(Awesome *awesome);
void awesome_wait_for_end_of_rasterization(Awesome *awesome); // Optional.
void awesome_wait_for_end_of_processing(Awesome *awesome);

// Return whether the home button is currently being pressed.
int awesome_get_home_button(Awesome *awesome);

// Set the brightness from 0 to AWESOME_MAX_BRIGHTNESS.
void awesome_set_brightness(Awesome *awesome, int brightness);
void awesome_disable_brightness(Awesome *awesome);

// Commands for the rasterizer.
void awesome_clear(volatile uint64_t **p, uint8_t red, uint8_t green, uint8_t blue);
void awesome_zclear(volatile uint64_t **p, uint16_t z);
void awesome_czclear(volatile uint64_t **p, uint8_t red,
	uint8_t green, uint8_t blue, uint16_t z);
void awesome_pattern(volatile uint64_t **p,
	uint64_t pattern0,
	uint64_t pattern1,
	uint64_t pattern2,
	uint64_t pattern3);
void awesome_draw(volatile uint64_t **p, int type, int count,
	int z_enable, int pattern_enable);
void awesome_vertex(volatile uint64_t **p, int x, int y, int z,
	uint8_t red, uint8_t green, uint8_t blue);
void awesome_swap(volatile uint64_t **p);
void awesome_end(volatile uint64_t **p);

#endif /* __AWESOME_H__ */
