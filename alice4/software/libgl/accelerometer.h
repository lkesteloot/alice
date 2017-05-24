#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

int accelerometer_init();
void accelerometer_read(float *theta_y, float *theta_x);

#endif /* _ACCELEROMETER_H_ */
