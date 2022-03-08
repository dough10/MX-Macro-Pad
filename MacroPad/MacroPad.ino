/*

    Macro Keys

    Project is based on the work of Dave Madison @ Partts not Included. https://www.partsnotincluded.com/diy-stream-deck-mini-macro-keyboard/
    Thank you Dave for the Excelant starting point.

   led light brightness values are reverse 0 max and 255 off


*/

// Original License

/*
 *  Project     'Stream Cheap' Mini Macro Keyboard
 *  @author     David Madison
 *  @link       partsnotincluded.com/electronics/diy-stream-deck-mini-macro-keyboard
 *  @license    MIT - Copyright (c) 2018 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

// --------------------------------------------
// Button Pin definitions
// --------------------------------------------

// cherry mx buttons
#define BUTTON_PIN1 A0
#define BUTTON_PIN2 A1
#define BUTTON_PIN3 A2
#define BUTTON_PIN4 A3
#define BUTTON_PIN5 A4
// encoder button pin
#define FUNCT_KEY A5

// --------------------------------------------
// LED Pin definitions
// --------------------------------------------

#define LED_PIN1 9
#define LED_PIN2 10
#define LED_PIN3 11
#define LED_PIN4 5
#define LED_PIN5 13

// --------------------------------------------
// SD Card Pin definitions
// --------------------------------------------

#define CSPIN 4

// --------------------------------------------
// Key Bind defaults
// --------------------------------------------

#define BUTTON_KEY1 240 // button 1 default
#define BUTTON_KEY2 241 // button 2 default
#define BUTTON_KEY3 242 // button 3 default
#define BUTTON_KEY4 243 // button 4 default
#define BUTTON_KEY5 244 // button 5 default
#define BUTTON_KEY6 0 // button 1 modifier 1 default
#define BUTTON_KEY7 0 // button 2 modifier 1 default
#define BUTTON_KEY8 0 // button 3 modifier 1 default
#define BUTTON_KEY9 0 // button 4 modifier 1 default
#define BUTTON_KEY10 0 // button 5 modifier 1 default
#define BUTTON_KEY11 0 // button 1 modifier 2 default
#define BUTTON_KEY12 0 // button 2 modifier 2 default
#define BUTTON_KEY13 0 // button 3 modifier 2 default
#define BUTTON_KEY14 0 // button 4 modifier 2 default
#define BUTTON_KEY15 0 // button 5 modifier 2 default
#define BUTTON_KEY16 215 // encoder right default
#define BUTTON_KEY17 216 // encoder left default
#define BUTTON_KEY18 0 // encoder right modifier default
#define BUTTON_KEY19 0 // encoder left modifier default
#define BUTTON_KEY20 0 // encoder left modifier default
#define BUTTON_KEY21 0 // encoder left modifier default

/*
----------------------------------
          Lib Imports
----------------------------------
*/

// i need <HID-Project.h> for media keys but it makes strange things happen.
// i can't find a glossary of key decimal values
// the key map i am using makes keyboard no work /shrug  (./src/keys.json has current key map)
// #include <HID-Project.h>
#include <Keyboard.h>
#include <Encoder.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>
#include "LED.h"
#include "BUTTONS.h"
#include "DATA.h"
#include "KNOB.h"

byte lastRan = millis();


// get shit ready
void setup() {
  // Safety check. Ground pin #1 (RX) to cancel keyboard inputs.
  // this was in the tutorial i started with looked cool so i kept it :D
  pinMode(1, INPUT_PULLUP);
  if (!digitalRead(1)) {
    failsafe();
  }
  Serial.begin(9600);
  // Consumer.begin();
  // BootKeyboard.begin();
  Keyboard.begin();
  for (byte i = 0; i < 5; i++) {
    pinMode(LED.leds[i], INPUT);
  }
  pinMode(FUNCT_KEY, INPUT_PULLUP);
  for (byte i = 0; i < NumButtons; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  DATA.init();
}

// loop de loop
void loop() {
  if (Serial) {
    DATA.checkForData();
  }
  // check if knob is being turned
  KNOB.read();
  // leds doing led stuff
  LED.doWork();
  // check if buttons are pressed
  // checks button and modifiers in same loop
  for (byte i = 0; i < 5; i++) {
    buttons[i + 10].update();
    buttons[i + 5].update();
    buttons[i].update();
  }
  // update UI / save to sd card if something has changed
  if ((millis() - lastRan  <= 100)) {
    return;
  }
  lastRan - millis();
  DATA.printJSON();
}

//
void failsafe() {
  for (;;) {} // Just going to hang out here for awhile :D
}
