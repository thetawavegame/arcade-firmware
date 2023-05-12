#include <TimerOne.h>

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
LEDButton p1_green = {2, off, 0, 5};
LEDButton p1_purple = {3, off, 0, 5};
LEDButton p2_green = {4, off, 0, 5};
LEDButton p2_purple = {5, off, 0, 5};
LEDButton red = {6, off, 0, 5};

// create buffer with one byte for each button
const byte numBytes = 5;
byte recievedBytes[numBytes];

// track whether data was just populated
boolean newData = false;

// timer period for fade pattern
int timer_period = 30000;

void setup() {
  //setup timer
  Timer1.initialize(timer_period);
  Timer1.attachInterrupt(updatePatterns);

  // setup serial
  Serial.begin(115200);
}

void loop() {
  // receive bytes from game
  recvWithEndMarker();

  // update LED patterns
  updateLEDs();
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
void updatePatterns() {
  updatePattern(p1_green);
  updatePattern(p1_purple);
  updatePattern(p2_green);
  updatePattern(p2_purple);
  updatePattern(red);
}