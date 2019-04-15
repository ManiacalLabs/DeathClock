#include <FastLED.h>
#include "char.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    SPI_DATA
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    7*7

CRGB leds[NUM_LEDS];

#define BRIGHTNESS          32
#define FRAMES_PER_SECOND  120

void setup() {
  delay(500); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}

void set_char(uint8_t i, char c, CRGB color) {
  static byte c_val = 0;
  static byte x;
  c_val = byte(c);
  if (c_val < 32 || c_val > 122) { return; } // out of range
  c_val = _chars[c_val - CHAR_OFFSET];

  for(x=0; x < 7; x++){
    fill_solid(leds + (i*7) + (x*7), 7, (c_val & _BV(x)) ? color : CRGB(0,0,0));
    // leds[i*7 + x] = (c_val & _BV(x)) ? color : CRGB(0,0,0);
  }
}

String msg = "0123456789ABCDEFGHIJKLMOPQRSTUVWXYZ";
uint8_t msg_i = 0;
void loop() {

  set_char(0, msg[msg_i], CRGB::Red);
  FastLED.show();
  FastLED.delay(500);

  msg_i++;
  if(msg_i >= msg.length()) {
    msg_i = 0;
  }
}
