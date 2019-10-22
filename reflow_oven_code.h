#ifndef reflow_oven_code_h
#define reflow_oven_code_h

#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <SPI.h>

#include "PMODTC1.h"
#include "oven_interp.h"
#include "oven_UI.h"

// Pinouts
#define RELAY 2
#define TEST 9 // useful if you want to do some timing measurements
#define CS 14 // chip select for thermocouple module
#define BTNR 15 // A1, PC1, PCINT9, PCI1
#define BTNM 16 // A2, PC2, PCINT10, PCI1
#define BTNL 17 // A3, PC3, PCINT11, PCI1
// SCK on 13
// MISO on 12
// MOSI on 11
// SS on 10

// I2C addresses
#define LCD_ADDR 0x3F // may vary for different LCD modules

// macros
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) // used to set bits in registers
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) // used to clear bits in registers

// Bit masks
#define L_MASK (_BV(PINC3)) // Mask for left button
#define M_MASK (_BV(PINC2)) // Mask for middle button
#define R_MASK (_BV(PINC1)) // Mask for right button
#define F_MASK (1) // Mask for buttons flag

// Constants
#define ALENGTH (7) // number of points in a single profile
#define ADEPTH (2) // number of profiles
#define DEBOUNCE_TIME 10 // debounce delay in ms

// Arrays
// initial temp and time profiles were based off of the datasheet for the solder paste
// adjusted based on results from experimentation
const int temp_array[ADEPTH][ALENGTH] = {
  {25, 90, 130, 138, 165, 138, 25},
  {25, 150, 175, 217, 249, 217, 25}
};
const unsigned long time_array[ADEPTH][ALENGTH] = {
  {0, 90000UL, 180000UL, 210000UL, 250000UL, 251000UL, 252000UL},
  {0, 110000UL, 200000UL, 250000UL, 300000UL, 301000UL, 302000UL}
};

#endif
