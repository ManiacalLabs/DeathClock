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

void set_seg(uint8_t chr_i, uint8_t seg_i, CRGB color) {
   fill_solid(leds + (chr_i*7) + (seg_i*7), 7, color);
}

void set_char_raw(uint8_t i, byte c, CRGB color) {
  static byte x;
  for(x=0; x < 7; x++){
    set_seg(i, x, (c & _BV(x)) ? color : CRGB(0,0,0));
  }
}

void set_char(uint8_t i, char c, CRGB color) {
  static byte c_val = 0;

  c_val = byte(c);
  if (c_val < 32 || c_val > 122) { return; } // out of range
  c_val = _chars[c_val - CHAR_OFFSET];

  set_char_raw(i, c_val, color);
}

CRGB wheel(int WheelPos) {
  CRGB color;
  if (85 > WheelPos) {
   color.r=0;
   color.g=WheelPos * 3;
   color.b=(255 - WheelPos * 3);;
  }
  else if (170 > WheelPos) {
   color.r=WheelPos * 3;
   color.g=(255 - WheelPos * 3);
   color.b=0;
  }
  else {
   color.r=(255 - WheelPos * 3);
   color.g=0;
   color.b=WheelPos * 3;
  }
  return color;
}

String msg = "0123456789";
uint8_t msg_i = 0;
uint8_t msg_l = msg.length();

void do_numbers() {
  static CRGB c;
  c = wheel(((256 / msg_l) * msg_i) % 256);
  set_char(0, msg[msg_i], c);
  FastLED.show();
  FastLED.delay(500);

  msg_i++;
  if(msg_i >= msg.length()) {
    msg_i = 0;
  }
}

void do_raw_vals() {
  static CRGB c;
  for(uint8_t i=0; i<128; i++){
    c = wheel((2 * i) % 256);
    set_char_raw(0, i, c);
    FastLED.show();
    FastLED.delay(50);
  }


}

void loop() {
  do_numbers();
  // do_raw_vals();
}
