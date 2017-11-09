
// Interface to the L3GD20H gyro chip.

#ifndef _GYRO_H_
#define _GYRO_H_

// Initialize the I2C connection to the gyro chip. Returns whether
// the initialization was successful.
int gyro_init();

// Whether the gyro chip has data for us. Do not read unless this returns
// true.
int gyro_ready();

// Get the data in degress per second (dps).
void gyro_read(int *dx, int *dy, int *dz);

// Whether we've overrun the FIFO. If this returns true, you're not reading
// fast enough and lost some data.
int gyro_overrun();

#endif /* _GYRO_H_ */
