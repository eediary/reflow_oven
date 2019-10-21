#include "PMODTC1.h"
//#define DEBUG
static float filt_array[FILT_LEN];
static byte filt_index = 0;
static bool filt_init = false;

// Performs SPI transaction and writes binary data to pointers
static void get_SPI_data(int CS, int16_t *TC_data1, int16_t *TC_data2){
  digitalWrite(CS, LOW);
  *TC_data1 = SPI.transfer16(0);
  *TC_data2 = SPI.transfer16(0);
  digitalWrite(CS, HIGH);
}

// Updates provided PMODTC1 structure with thermocouple module data
void PMODTC1_update(int CS, PMODTC1_data *data){
  uint8_t temp_error;
  int16_t TC_data1, TC_data2;
  float oven_temp_raw;
  
  // get binary data from SPI
  get_SPI_data(CS, &TC_data1, &TC_data2);
  
  // get and filter oven temp
  oven_temp_raw = ((TC_data1) >> 2) * TC_CONV;
  
  // Initailize filter if necessary
  if(!filt_init){
    for(int i = 0; i < FILT_LEN; i++){
      filt_array[i] = oven_temp_raw;
    }
    filt_init = true;
  }
  
  // Update filter with raw val
  filt_array[filt_index++] = oven_temp_raw;
  filt_index = filt_index % 5;
  
  // write data to provided struct
  data->oven_temp = (filt_array[0] + filt_array[1] + filt_array[2] + filt_array[3] + filt_array[4])/5;
  data->ref_temp = ((TC_data2) >> 4) * TEMP_CONV;
  temp_error = TC_data1 & ERROR_MASK;
  data->error_flag = (temp_error<< 3) | TC_data2 & (ERROR_MASK | SCV_MASK | SCG_MASK | OC_MASK);
  
  #ifdef DEBUG
  Serial.println("Filt array: " + 
    String(filt_array[0]) + ", " + 
    String(filt_array[1]) + ", " + 
    String(filt_array[2]) + ", " + 
    String(filt_array[3]) + ", " + 
    String(filt_array[4]));
  #endif
}
