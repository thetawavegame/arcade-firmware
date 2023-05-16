#include <arduino-timer.h>
#include <Adafruit_NeoPixel.h>

auto timer_1 = timer_create_default();
//auto timer_2 = timer_create_default();

// available patterns for leds
enum LEDPattern {
  off, // LED fully off
  on, // LED fully on
  fade, // fade between on and off
};

// represents a button with an LED
struct LEDButton {
  int pin; // pin the LED is attached to
  LEDPattern pattern; // pattern the LED is performing
  uint8_t brightness; // used for fade pattern
  int8_t fade;
};

// initialize led buttons
LEDButton p1_green = {11, off, 0, 5};
LEDButton p1_purple = {13, off, 0, 5};
LEDButton p2_green = {9, off, 0, 5};
LEDButton p2_purple = {10, off, 0, 5};
LEDButton red = {5, off, 0, 5};
int right_led_strip = 3;
int right_idx = 0;
int right_update_flag = false;
Adafruit_NeoPixel right_strip(30, right_led_strip, NEO_GRB + NEO_KHZ800);
int left_led_strip = 6;
int left_idx = 0;
int left_update_flag = false;
Adafruit_NeoPixel left_strip(30, left_led_strip, NEO_GRB + NEO_KHZ800);


// create buffer with one byte for each button
const byte numBytes = 5;
byte recievedBytes[numBytes];

// track whether data was just populated
boolean newData = false;

// timer period for fade pattern
int timer_period = 30000;

void setup() {
  right_strip.begin();
  right_strip.show();
  right_strip.setBrightness(50);
  left_strip.begin();
  left_strip.show();
  left_strip.setBrightness(50);

  //setup timer
  timer_1.every(30, updatePatterns);
  timer_1.every(100, updateColorWipe);

  // setup serial
  Serial.begin(115200);
}

void loop() {
  timer_1.tick();
  //timer_2.tick();

  // receive bytes from game
  recvWithEndMarker();

  // update LED patterns
  updateLEDs();

  // update right neopixel strip
  if (right_update_flag) {
    if (right_idx < 30) {
      right_strip.setPixelColor(right_idx, right_strip.Color(0,255,0));         //  Set pixel's color (in RAM)
      right_strip.show();
      right_idx ++;
    }else if (right_idx >= 30 && right_idx < 60) {
      right_strip.setPixelColor(right_idx - 30, right_strip.Color(255,0,255));         //  Set pixel's color (in RAM)
      right_strip.show();
      right_idx ++;
    } else {
      right_idx = 0;
    }
    right_update_flag = false;
  }

  // update left neopixel strip
  if (left_update_flag) {
    if (left_idx < 30) {
      left_strip.setPixelColor(left_idx, left_strip.Color(0,255,0));         //  Set pixel's color (in RAM)
      left_strip.show();
      left_idx ++;
    }else if (left_idx >= 30 && left_idx < 60) {
      left_strip.setPixelColor(left_idx - 30, left_strip.Color(255,0,255));         //  Set pixel's color (in RAM)
      left_strip.show();
      left_idx ++;
    } else {
      left_idx = 0;
    }
    left_update_flag = false;
  }
}

void recvWithEndMarker() {
  // tracks index of bytes received 
  static byte idx = 0;

  // byte representing the end of a serial message
  byte endMarker = 255;

  byte rb;

  // read bytes and add to buffer
  if (Serial.available() > 0) {
    while (Serial.available() && newData == false) {
      rb = Serial.read();

      if (rb != endMarker) {
        recievedBytes[idx] = rb;
        idx ++;

      } else {
        idx = 0;
        newData = true;
      }
    }
  }
}

void updateLEDs() {

  // update patterns if new data available
  if (newData == true) {
  
    p1_green.pattern = (LEDPattern)recievedBytes[0];
    p1_purple.pattern = (LEDPattern)recievedBytes[1];
    p2_green.pattern = (LEDPattern)recievedBytes[2];
    p2_purple.pattern = (LEDPattern)recievedBytes[3];
    red.pattern = (LEDPattern)recievedBytes[4];  

    newData = false;
  }

  
}

void updatePattern(LEDButton &led_button) {
  if (led_button.pattern == off) { // off pattern
    analogWrite(led_button.pin, 0);
    // reset fade params
    led_button.brightness = 0;
    led_button.fade = 5;
  } else if (led_button.pattern == on) { // on pattern
    analogWrite(led_button.pin, 255);
    // reset fade params
    led_button.brightness = 0;
    led_button.fade = 5;
  } else if (led_button.pattern == fade) { // fade pattern
    led_button.brightness += led_button.fade;
    analogWrite(led_button.pin, led_button.brightness);
    if (led_button.brightness >= 255 || led_button.brightness <= 0) {
      led_button.fade *= -1;
    }
  }
}

// update all led button patterns
void updatePatterns(void *) {
  updatePattern(p1_green);
  updatePattern(p1_purple);
  updatePattern(p2_green);
  updatePattern(p2_purple);
  updatePattern(red);

  return true;
}

void updateColorWipe(void *) {
  //Serial.println(right_idx);
  right_update_flag = true;
  left_update_flag = true;

  return true;
}
