#ifndef oven_UI_h
#define oven_UI_h

#include <Arduino.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "PMODTC1.h"
#include "reflow_oven_code.h"

#define MANUAL_SLOPE (1500) // ms per degree
#define HYST_VAL (-2); // in Celsius

// UI variables
enum state
{
  SELECT,
  MANUAL,
  PROFILE
};

const char nav_string[] = "<      OK      >";
const char cancel_string[] = "      BACK";

// returns true if relay should be on; false otherwise
bool UI_state_machine(PMODTC1_data data, char *str1, char *str2, volatile byte *buttons);

#endif
