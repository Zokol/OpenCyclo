#include <Arduino.h>
#include <TM1637Display.h>
#include "Timer.h"

// Module connection pins (Digital Pins)
#define CLK 5
#define DIO 4
#define SENSOR 2

// The amount of time (in milliseconds) between tests
#define INIT_DELAY   500
#define LOGO_DELAY   1000
#define N_PULSES     3

const uint8_t SEG_DONE[] = {
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
	SEG_C | SEG_E | SEG_G,                           // n
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // E
	};

const uint8_t SEG_NULL[] = {
	SEG_G,                                           // -
	SEG_G,                                           // -
	SEG_G,                                           // -
	SEG_G                                           // -
	};

const uint8_t SEG_ERR[] = {
	SEG_A | SEG_G | SEG_D,                           // -
	SEG_A | SEG_G | SEG_D,                           // -
	SEG_A | SEG_G | SEG_D,                           // -
	SEG_A | SEG_G | SEG_D                            // -
	};

const uint8_t SEG_CADENCE[] = {
	SEG_A | SEG_F | SEG_G | SEG_E | SEG_D,           // C
	SEG_A | SEG_F | SEG_B | SEG_G | SEG_E | SEG_C,   // A
	SEG_B | SEG_C | SEG_D | SEG_E | SEG_G,           // d
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
	SEG_G | SEG_E | SEG_C,                           // n
	SEG_A | SEG_F | SEG_G | SEG_E | SEG_D,           // C
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
	SEG_G,                                           // -
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // M
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // T
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G,           // E
	SEG_A | SEG_D | SEG_E | SEG_F | SEG_G            // R
	};


volatile unsigned long pulses[] = {0, 0, 0};
volatile int rpm = 0;
volatile boolean pulses_full = false;

TM1637Display display(CLK, DIO);

void setup()
{
  attachInterrupt(0, revolution, FALLING);
}

void loop()
{
  int k;
  uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
  display.setBrightness(0x0f);
  
  // All segments on
  display.setSegments(data);
  delay(INIT_DELAY);
  display.setBrightness(0x0f);
  display.setSegments(SEG_NULL);
  
  while(true){
    delay(1000);
  }
}

void revolution(){
  // Check if pulses-array contains zeros
  if(!pulses_full){
    for(int i = 0; i < N_PULSES; i++){
      if(pulses[i] == 0){
        // Found zero from array, add current time in place of the found zero and exit
        pulses[i] = millis();
        return;
      }
    }
    // Didn't find zeros, setting full-flag high so next iteration doesn't have to check if the array is full.
    pulses_full = true;
  }
  else{
    // Move last measurements one step left
    for(int i = 0; i < N_PULSES - 1; i++){
      pulses[i] = pulses[i+1];
    }
    // Set last measurement to current time
    pulses[N_PULSES - 1] = millis();
    
    // Calc average of pulse distances
    long sum = 0;
    for(int i = 0; i < N_PULSES - 1; i++){
      sum += pulses[i+1] - pulses[i];
      //display.showNumberDec(sum);
    }
    
    long avg = sum / N_PULSES;
    //display.showNumberDec(avg);
    
    if(avg < 200){
      display.setSegments(SEG_ERR);
    }
    else{
      // Calc RPM from average distance
      display.showNumberDec(60000 / avg);
    }
    
  }
}
