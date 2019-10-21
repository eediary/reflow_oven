#include "oven_interp.h"
static unsigned long start_time = 0;

// resets timer used by interpolator
void reset_timer(void){
  start_time = millis();
}

// returns timer value
unsigned long get_time(void){
  return millis() - start_time;
}

// performs linear interpolation using matrixes
// gives last value in temp array when time exceeds last value in time array
float m_lin_interp(int alength, const unsigned long *time_array, const int *temp_array){
  unsigned long cur_time = get_time();
  int index = 0;
  while((index < alength-1) && (cur_time > time_array[index + 1])){
    index++;
  }
  if(index >= alength - 1){
    return temp_array[alength-1];
  } else{
    return ((float)(temp_array[index + 1] * (cur_time - time_array[index])) + (temp_array[index] * (time_array[index + 1] - cur_time))) /(time_array[index+1] - time_array[index]);
  }
}

// performs linear interpolation between two points
// uses starting and ending time and temperature, using current time for calculations
// gives end temp if time exceeds end time
float lin_interp(unsigned long t1, unsigned long t2, int temp1, int temp2){
  unsigned long cur_time = get_time();
  if(cur_time > t2){
    return (float) temp2;
  } else{
    return ((float)(temp2 * (cur_time - t1)) + (temp1 * (t2 - cur_time))) /(t2 - t1);
  }
}
