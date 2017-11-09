#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

int accelerometer_init();
void accelerometer_read(float *theta_y, float *theta_x);

// Each is -256 to 256.
int accelerometer_read_x();
int accelerometer_read_y();
int accelerometer_read_z();

#endif /* _ACCELEROMETER_H_ */
