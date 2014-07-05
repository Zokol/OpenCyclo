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
#define CALC_INTERVAL 2500

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


volatile int count = 0;
volatile int rpm = 0;

TM1637Display display(CLK, DIO);
Timer t;

void setup()
{
  attachInterrupt(0, revolution, FALLING);
  t.every(CALC_INTERVAL, calcRPM);
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
  //display.setSegments(SEG_DONE);
  //delay(LOGO_DELAY);
  
  while(true){
    t.update();
    if(rpm < 1){
      display.setSegments(SEG_NULL);
    }
    else if(rpm > 300){
      display.setSegments(SEG_ERR);
    }
    else{
      display.showNumberDec(rpm);
    }
    delay(1000);
  }
}

void revolution(){
  count += 1;
}

void calcRPM(){
  float time = CALC_INTERVAL / 1000 / 60.0;
  rpm = count / time;
  count = 0;
}
