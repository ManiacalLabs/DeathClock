#include <FastLED.h>
#include "char.h"

FASTLED_USING_NAMESPACE

#define DATA_PIN    SPI_DATA
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    7*7

#define OFF CRGB(0,0,0)

CRGB leds[NUM_LEDS];

#define RAINBOW_COUNT 10
CRGB rainbow[RAINBOW_COUNT];

#define BRIGHTNESS          32
#define FRAMES_PER_SECOND  120

void setup() {
  Serial.begin(115200);
  delay(500); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  fill_rainbow(rainbow, RAINBOW_COUNT, 0, 256 / RAINBOW_COUNT);
}

void set_seg(uint8_t chr_i, uint8_t seg_i, CRGB color) {
   fill_solid(leds + (chr_i*7) + (seg_i*7), 7, color);
}

void set_char_raw(uint8_t i, byte c, CRGB color, bool transparent) {
  static byte x;
  static CRGB color_val;
  for(x=0; x < 7; x++){
    if(transparent){
      if((c & _BV(x)) == 0){
        set_seg(i, x, OFF);
      }
    }
    else {
      set_seg(i, x, (c & _BV(x)) ? color : OFF);
    }
  }
}

void set_char_raw(uint8_t i, byte c, CRGB color) {
  set_char_raw(i, c, color, false);
}

void set_char(uint8_t i, char c, CRGB color, bool transparent) {
  static byte c_val = 0;

  c_val = byte(c);
  if (c_val < 32 || c_val > 122) { return; } // out of range
  c_val = _chars[c_val - CHAR_OFFSET];

  set_char_raw(i, c_val, color, transparent);
}

void set_char(uint8_t i, char c, CRGB color) {
  set_char(i, c, color, false);
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
  // c = wheel(((256 / msg_l) * msg_i) % 256);
  c = rainbow[msg_i % RAINBOW_COUNT];
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

#define Y_RAINBOW_COUNT MATRIX_Y
CRGB y_rainbow[Y_RAINBOW_COUNT];
uint8_t m_y_count = 0;

void fill_y_rainbow(){
  static uint8_t x, y;
  fill_rainbow(y_rainbow, Y_RAINBOW_COUNT, m_y_count, 256 / Y_RAINBOW_COUNT);

  for(y=0; y<MATRIX_Y; y++){
    for(x=0; x<MATRIX_X; x++){
      if(_char_matrix[y][x] >= 0){
        leds[_char_matrix[y][x]] = y_rainbow[y];
      }
    }
  }
}

void do_matrix_y() {
  fill_y_rainbow();
  m_y_count+=5;

  FastLED.show();
  FastLED.delay(50);
}

#define X_RAINBOW_COUNT MATRIX_X
CRGB x_rainbow[X_RAINBOW_COUNT];
uint8_t m_x_count = 0;

void do_matrix_x() {
  static uint8_t x, y;
  fill_rainbow(x_rainbow, X_RAINBOW_COUNT, m_x_count, 256 / X_RAINBOW_COUNT);

  for(y=0; y<MATRIX_Y; y++){
    for(x=0; x<MATRIX_X; x++){
      if(_char_matrix[y][x] >= 0){
        leds[_char_matrix[y][x]] = x_rainbow[x];
      }
    }
  }

  m_x_count+=5;

  FastLED.show();
  FastLED.delay(50);
}

uint8_t drn_frame = 0;
String rmsg = "0123456789ABCDEF";
uint8_t rmsg_i = 0;
uint8_t rmsg_l = rmsg.length();
bool do_rainbow_numbers() {
  fill_y_rainbow();
  m_y_count += 3;
  set_char(0, rmsg[rmsg_i], OFF, true);


  drn_frame++;

  if(drn_frame >= 20) {
    drn_frame = 0;
    rmsg_i++;
    if(rmsg_i >= rmsg.length()) {
      rmsg_i = 0;
      return true;
    }
  }

  FastLED.show();
  FastLED.delay(25);
  return false;
}

int8_t snake_i = 0;
int8_t snake_dir = 1;
bool do_snake()
{
  static uint8_t i;
  fill_y_rainbow();
  m_y_count += 3;

  for(i=0; i<NUM_LEDS; i++){
    if(i == snake_i){ leds[i] = leds[i]; } //basically a no-op
    else if(i == (snake_i - 1*snake_dir)){ leds[i] = leds[i].nscale8_video(192); }
    else if(i == (snake_i - 2*snake_dir)){ leds[i] = leds[i].nscale8_video(128); }
    else if(i == (snake_i - 3*snake_dir)){ leds[i] = leds[i].nscale8_video(96); }
    else if(i == (snake_i - 4*snake_dir)){ leds[i] = leds[i].nscale8_video(64); }
    else { leds[i] = OFF; }
  }

  FastLED.show();
  FastLED.delay(50);

  snake_i+=snake_dir;
  if(snake_i >= NUM_LEDS + 6){snake_dir = -1; snake_i = NUM_LEDS;}
  if(snake_i < -5){snake_dir = 1; snake_i = 0; return true;}
  return false;
}

void fill_row(int8_t row, CRGB color){
  static int8_t x;
  for(x=0; x<MATRIX_X; x++){
    leds[_char_matrix[row][x]] = color;
  }
}

int8_t y_wipe_i = 0;
int8_t y_wipe_dir = 1;
bool do_y_wipe(){
  static uint8_t y;
  fill_rainbow(y_rainbow, Y_RAINBOW_COUNT, m_y_count, 256 / Y_RAINBOW_COUNT);

  for(y=0; y<MATRIX_Y; y++){
    if(y == y_wipe_i){ fill_row(y, y_rainbow[y]); }
    else if(y == (y_wipe_i - 1*snake_dir)){ fill_row(y, y_rainbow[y].nscale8_video(192)); }
    else if(y == (y_wipe_i - 2*snake_dir)){ fill_row(y, y_rainbow[y].nscale8_video(128)); }
    else if(y == (y_wipe_i - 3*snake_dir)){ fill_row(y, y_rainbow[y].nscale8_video(96)); }
    else if(y == (y_wipe_i - 4*snake_dir)){ fill_row(y, y_rainbow[y].nscale8_video(64)); }
    else { fill_row(y, OFF); }
  }

  FastLED.show();
  FastLED.delay(50);

  y_wipe_i+=y_wipe_dir;
  if(y_wipe_i >= MATRIX_Y + 7){y_wipe_dir = -1; y_wipe_i = MATRIX_Y;}
  if(y_wipe_i < -6){y_wipe_dir = 1; y_wipe_i = 0; return true; }
  return false;
}

void fill_col(int8_t col, CRGB color){
  static int8_t y;
  for(y=0; y<MATRIX_Y; y++){
    leds[_char_matrix[y][col]] = color;
  }
}

int8_t x_wipe_i = 0;
int8_t x_wipe_dir = 1;
bool do_x_wipe(){
  static uint8_t x;
  fill_rainbow(x_rainbow, X_RAINBOW_COUNT, m_x_count, 256 / X_RAINBOW_COUNT);

  for(x=0; x<MATRIX_X; x++){
    if(x == x_wipe_i){ fill_col(x, x_rainbow[x]); }
    else if(x == (x_wipe_i - 1*snake_dir)){ fill_col(x, x_rainbow[x].nscale8_video(192)); }
    else if(x == (x_wipe_i - 2*snake_dir)){ fill_col(x, x_rainbow[x].nscale8_video(128)); }
    else if(x == (x_wipe_i - 3*snake_dir)){ fill_col(x, x_rainbow[x].nscale8_video(96)); }
    else if(x == (x_wipe_i - 4*snake_dir)){ fill_col(x, x_rainbow[x].nscale8_video(64)); }
    else { fill_col(x, OFF); }
  }

  FastLED.show();
  FastLED.delay(50);

  x_wipe_i+=x_wipe_dir;
  if(x_wipe_i >= MATRIX_X + 7){x_wipe_dir = -1; x_wipe_i = MATRIX_X;}
  if(x_wipe_i < -6){x_wipe_dir = 1; x_wipe_i = 0; return true;}
  return false;
}

int8_t anim_i = 3;
void loop() {
  static bool res = false;
  switch(anim_i) {
    case 0:
      res = do_y_wipe();
      break;
    case 1:
      res = do_x_wipe();
      break;
    case 2:
      res = do_snake();
      break;
    case 3:
      res = do_rainbow_numbers();
      break;
  }

  if(res){
    anim_i++;
    if(anim_i > 3){anim_i = 0;}
  }
  // do_numbers();
  // do_raw_vals();
  // do_matrix_y();
  // do_matrix_x();
  // do_rainbow_numbers();
  // do_snake();
  // do_y_wipe();
  // do_x_wipe();
}
