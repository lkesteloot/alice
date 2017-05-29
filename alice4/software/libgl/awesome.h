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
    // Public. These are valid after each awesome_end_frame().
    double elapsed; // Time to render and swap last frame.
    double frame_time;	// Time to scanout a single frame.
    int frames; // Number of scanned-out frames this frame will take.
    double fps; // Effective frames per second.

    // These are all private. Do not access.
    volatile uint32_t *gpo;
    volatile uint32_t *gpi;
    volatile uint64_t *command_buffer;
    volatile uint64_t *p;
    int home_button;
} Awesome;

// Initializes the library, opening the connection to the FPGA.
// Exits the program with a message if something goes wrong.
void awesome_init(Awesome *awesome);

// Call this before and after all the drawing routines.
void awesome_start_frame(Awesome *awesome);
void awesome_end_frame(Awesome *awesome);

// Return whether the home button is currently being pressed.
int awesome_get_home_button(Awesome *awesome);

// Set the brightness from 0 to AWESOME_MAX_BRIGHTNESS.
void awesome_set_brightness(Awesome *awesome, int brightness);
void awesome_disable_brightness(Awesome *awesome);

// Commands for the rasterizer.
void awesome_clear(Awesome *awesome, uint8_t red, uint8_t green, uint8_t blue);
void awesome_zclear(Awesome *awesome, uint16_t z);
void awesome_czclear(Awesome *awesome, uint8_t red,
	uint8_t green, uint8_t blue, uint16_t z);
void awesome_pattern(Awesome *awesome,
	uint64_t pattern0,
	uint64_t pattern1,
	uint64_t pattern2,
	uint64_t pattern3);
void awesome_draw(Awesome *awesome, int type, int count,
	int z_enable, int pattern_enable);
void awesome_vertex(Awesome *awesome, int x, int y, int z,
	uint8_t red, uint8_t green, uint8_t blue);
void awesome_swap(Awesome *awesome);
void awesome_end(Awesome *awesome);

#endif /* __AWESOME_H__ */
