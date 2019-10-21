#ifndef PMODTC1_h
#define PMODTC1_h

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>

// Thermocouple module masks
#define TC_MASK (0xFFFC)      // Bits 31~18, signed 14 bits
#define TEMP_MASK (0xFFF0)    // Bits 15~4, signed 12 bits
#define ERROR_MASK (0x0001)   // Bit 16, active high
#define SCV_MASK (0x0004)     // Bit 2, active high
#define SCG_MASK (0x0002)     // Bit 1, active high
#define OC_MASK (0x0001)      // Bit 0, active high

// Thermocouple constants
#define TC_CONV (0.25)
#define TEMP_CONV (0.0625)

// Filter properties
#define FILT_LEN (5)

struct PMODTC1_data{
  float oven_temp;
  float ref_temp;
  uint8_t error_flag;
};

// Updates provided PMODTC1 structure with thermocouple module data
void PMODTC1_update(int CS, PMODTC1_data *data);

#endif
