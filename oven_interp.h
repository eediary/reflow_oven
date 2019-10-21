#ifndef oven_interp_h
#define oven_interp_h

#include <Arduino.h>

// resets timer used by interpolator
void reset_timer(void);
// returns timer value
unsigned long get_time(void);
// performs linear interpolation using matrixes
float m_lin_interp(int alength, const unsigned long *time_array, const int *temp_array);
// performs linear interpolation between two points
float lin_interp(unsigned long t1, unsigned long t2, int temp1, int temp2);

#endif
