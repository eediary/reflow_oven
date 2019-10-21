// Prototype for Reflow Oven project
// Specifically for the user interface portion
// References:
// Arduino Pro Mini 3V3 pinout: https://cdn.sparkfun.com/assets/c/6/2/2/1/ProMini8MHzv2.pdf
// Arduino Pro Mini 3V3 Schematic: https://www.arduino.cc/en/uploads/Main/Arduino-Pro-Mini-schematic.pdf
// Project schematic: https://easyeda.com/editor#id=481eb80e627e49baa69d291e790bd23f
// Setting up interupts tutorial: https://www.youtube.com/watch?v=-rvaUgYVNuI (Tutorial: Pin Change Interrupts on the Arduino)
// sbi and cbi: https://arduino.stackexchange.com/questions/50423/sbi-and-cli-implementation (sbi() and cli() implementation)
// ATMEGA328 datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/ATmega48A-PA-88A-PA-168A-PA-328-P-DS-DS40002061A.pdf

// VERSION 1.0 (10/20/2019)

#include "reflow_oven_code.h"

#define DEBUG

// character arrays; should be same length as I2C width
char str1[] = "                ";
char str2[] = "                ";
char old_str1[] = "                ";
char old_str2[] = "                ";

// volatile variables for ISRs
volatile byte buttons_val = 0; // uses 3 LSbs to show button status
volatile bool update_flag = false; // flag indicating new temperature is available

// LiquidCrytal_PCF8574 object
static LiquidCrystal_PCF8574 lcd(LCD_ADDR);

// Thermocouple struct
static PMODTC1_data data;

void setup()
{
  int error;

  // Set up and initialize pins
  digitalWrite(RELAY, LOW);
  digitalWrite(CS, HIGH);
  digitalWrite(TEST, HIGH);
  // buttons need pull-ups
  pinMode(BTNL, INPUT_PULLUP);
  pinMode(BTNM, INPUT_PULLUP);
  pinMode(BTNR, INPUT_PULLUP);
  pinMode(RELAY, OUTPUT);
  pinMode(CS, OUTPUT);
  pinMode(TEST, OUTPUT);

  // Set up button interrupts
  sbi(PCICR, PCIE1); // enable interrupts on PCI1
  sbi(PCMSK1, PCINT11); // enables BTNL to trigger PCI1
  sbi(PCMSK1, PCINT10); // enables BTNM to trigger PCI1
  sbi(PCMSK1, PCINT9); // enables BTNR to trigger PCI1

  // Set up timer interrupt
  TCCR2A = 0; // clear TCCR2A register
  TCCR2B = 0; // clear TCCR2B register
  sbi(TCCR2A, WGM21); // Set counter into Clear Timer on Compare Match (CTC) mode
  sbi(TCCR2B, CS22); // Set clock divider to 1024
  sbi(TCCR2B, CS21);
  sbi(TCCR2B, CS20);
  OCR2A = 196; // Set max value of counter; 8 MHz / (1024 * 196) = 39.86 Hz
  sbi(TIMSK2, OCIE2A); // interrupt mask

  // Set up SPI
  SPI.begin();
  SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0));
  
  // Set up LCD
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(LCD_ADDR);
  error = Wire.endTransmission();
  lcd.begin(16, 2);
  lcd.setBacklight(255);
  lcd.home();

  // Welcome Message
  #ifdef DEBUG
  Serial.println("Debug menu");
  Serial.println("time, oven temp, set temp, ref temp, relay");
  #endif
  
} // setup()

void loop()
{
  // Get thermocouple data when needed, about every 25 ms
  if(update_flag){
    update_flag = false;
    PMODTC1_update(CS, &data);
  }
  
  // Call UI + control relay
  if(UI_state_machine(data, str1, str2, &buttons_val)){
    digitalWrite(RELAY, HIGH);
  } else{
    digitalWrite(RELAY, LOW);
  }
  
  // Only update screen when necessary to prevent screen flicker
  if(strcmp(str1, old_str1) || strcmp(str2, old_str2)){
    lcd.clear();
    lcd.print(str1);
    lcd.setCursor(0,1);
    lcd.print(str2);
  }
  strcpy(old_str1, str1);
  strcpy(old_str2, str2);

  delay(1);
} // loop()

// ISR for when any button is pressed
ISR (PCINT1_vect)
{
  // Interrupt triggers if any of the buttons are pressed
  // Set flag and read pins PINC1, PINC2 and PINC3
  buttons_val = (PINC & (L_MASK | M_MASK | R_MASK)) | F_MASK;
}

// ISR, telling system to sample TC Module every 25 ms
ISR (TIMER2_COMPA_vect)
{
  // set flag so it can be sampled in code
  update_flag = true;
}
