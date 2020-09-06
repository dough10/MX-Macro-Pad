/*
 *  Macro Keys
 *
 * led light brightness values are reverse 0 max and 255 off
 *
 *
 */



// --------------------------------------------
// Key Bind defaults
// --------------------------------------------

#define BUTTON_KEY1 240 // button 1
#define BUTTON_KEY2 241 // button 2
#define BUTTON_KEY3 242 // button 3
#define BUTTON_KEY4 243 // button 4
#define BUTTON_KEY5 244 // button 5
#define BUTTON_KEY6 0 // button 1 modifier
#define BUTTON_KEY7 0 // button 2 modifier
#define BUTTON_KEY8 0 // button 3 modifier
#define BUTTON_KEY9 0 // button 4 modifier
#define BUTTON_KEY10 0 // button 5 modifier
#define BUTTON_KEY11 215 // encoder right
#define BUTTON_KEY12 216 // encoder left
#define BUTTON_KEY13 0 // encoder right modifier
#define BUTTON_KEY14 0 // encoder left modifier

// --------------------------------------------
// Pin definitions
// --------------------------------------------

#define BUTTON_PIN1 A0
#define BUTTON_PIN2 A1
#define BUTTON_PIN3 A2
#define BUTTON_PIN4 A3
#define BUTTON_PIN5 A4

// --------------------------------------------
// Lib Imports
// --------------------------------------------

//#include <HID-Project.h>
#include <Keyboard.h>
#include <EEPROM.h>
#include <Encoder.h>

// encoder button pin
const uint8_t funct_key = A5;

// --------------------------------------------
// Lighting mode functions
// --------------------------------------------

class LED_Controller {
  public:
    // variable for changeing the LEDS light modes
    int LED_MODE = 2;

    // array of PWM pins with leds connected
    const int leds[5] = {
      9, // button 1 led
      10, // button 2 led
      11, // button 3 led
      5, // button 4 led
      13, // button 5 led
    };
    // on press led mode
    int brightnesses[5] = {
      255, // button 1 led brightness
      255, // button 2 led brightness
      255, // button 3 led brightness
      255, // button 4 led brightness
      255, // button 5 led brightness
    };
    // always on light mode
    int brightness = 0; // adjustable brightness value
    const int brightnessIncriment = 5; // 1 encoder click = this value in change of led brightness

    // change the mode of the LEDs
    void setLEDMode(int mode) {
      LED_MODE = mode;
      EEPROM.update(15, LED_MODE);
      Serial.println("?" + String(LED_MODE));
    }

    // function to set all LED values @ once
    void makeLight(int value) {
      for (int i = 0; i < 5; i++) {
        analogWrite(leds[i], value);
      }
    }

    // switch handler for changeing of led function
    void doWork() {
      switch (LED_MODE) {
        case 0:
          makeLight(brightness);
          break;
        case 1:
          onPressLEDMODE();
          break;
        case 2:
          makeLight(fadeBrightness);
          fadeBrightness = fadeBrightness + fadeAmount;
          if (fadeBrightness <= 0 || fadeBrightness >= 255) {
            fadeAmount = -fadeAmount;
          }
          break;
        case 3:
          knightRider();
          break;
        default:
          makeLight(255);
          break;
      }
    }

  private:
    // knight rider LED mode
    int currentLED = 3; // index of the led in the array currently having it's brightness value changed
    int krIncriment = 51; // changeing this value changes the speed of leds
    int krBrightness = 255 - krIncriment; // default brightness value
    int changeBy = -1; // value to incriment led array index

    // knight rider Mode
    void knightRider() {
      analogWrite(leds[currentLED], krBrightness);
      if (krBrightness >= 255) {
        krIncriment = -krIncriment;
        if (currentLED >= 4 || currentLED <= 0) {
          changeBy = -changeBy;
        }
        currentLED = currentLED + changeBy;
      }
      if (krBrightness <= 0) {
        krIncriment = -krIncriment;
      }
      krBrightness = krBrightness + krIncriment;
    }

    // on press led mode
    int clickIncriment = 1;   // raising this value speeds up the led fade out
    void onPressLEDMODE() {
      for (int i = 0; i < 5; i++) {
        analogWrite(leds[i], brightnesses[i]);
        if (brightnesses[i] < 255) {
          brightnesses[i] = brightnesses[i] + clickIncriment;
        }
      }
    }
    // breath LED Mode
    int fadeBrightness = 1;
    int fadeAmount = 1;

};

LED_Controller   LED;

// --------------------------------------------
// BUTTONS!!!!
// --------------------------------------------

// Button helper class for handling press/release and debouncing
class button {
  public:
    uint8_t key;
    const uint8_t pin;
    button(uint8_t k, uint8_t p) : key(k), pin(p) {}
    void press(boolean state) {
      if (key == 0) {
        return;
      }
      // debounce button press
      if (state == pressed || (millis() - lastPressed  <= debounceTime)) {
        return; // Nothing to see here, folks
      }
      if (state && !digitalRead(funct_key)) {
        // button 1 sets LEDs to adjustable brightness mode
        if (pin == A0 && LED.LED_MODE != 0) {
          LED.setLEDMode(0);
          return;
        }
        // button 2 sets LEDs to light on keypress and fade out after
        if (pin == A1 && LED.LED_MODE != 1) {
          LED.setLEDMode(1);
          LED.makeLight(255);
          return;
        }
        // button 3 sets LEDs to Fade / Breath mode
        if (pin == A2 && LED.LED_MODE != 2) {
          LED.setLEDMode(2);
          return;
        }
        // knight rider mode
        if (pin == A3 && LED.LED_MODE != 3) {
          LED.setLEDMode(3);
          LED.makeLight(255);
          return;
        }
        // LED off mode
        if (pin == A4 && LED.LED_MODE != 4) {
          LED.setLEDMode(4);
          LED.makeLight(255);
          return;
        }
      }
      lastPressed = millis();
      // stop key press if function key held
      if (!digitalRead(funct_key)) {
        return;
      }
      state ? Keyboard.press(key) : Keyboard.release(key);
      pressed = state;
      if (state && LED.LED_MODE == 1) {
        if (pin == A0) {
          LED.brightnesses[0] = 0;
        }
        if (pin == A1) {
          LED.brightnesses[1] = 0;
        }
        if (pin == A2) {
          LED.brightnesses[2] = 0;
        }
        if (pin == A3) {
          LED.brightnesses[3] = 0;
        }
        if (pin == A4) {
          LED.brightnesses[4] = 0;
        }
      }
    }

    void update() {
      press(!digitalRead(pin));
    }

  private:
    const long debounceTime = 60;
    unsigned long lastPressed;
    boolean pressed = 0;
} ;

// Button objects, organized in array
button buttons[] = {
  {BUTTON_KEY1, BUTTON_PIN1},
  {BUTTON_KEY2, BUTTON_PIN2},
  {BUTTON_KEY3, BUTTON_PIN3},
  {BUTTON_KEY4, BUTTON_PIN4},
  {BUTTON_KEY5, BUTTON_PIN5},
  {BUTTON_KEY6, BUTTON_PIN1},
  {BUTTON_KEY7, BUTTON_PIN2},
  {BUTTON_KEY8, BUTTON_PIN3},
  {BUTTON_KEY9, BUTTON_PIN4},
  {BUTTON_KEY10, BUTTON_PIN5},
  {BUTTON_KEY11, A5},
  {BUTTON_KEY12, A5},
  {BUTTON_KEY13, A5},
  {BUTTON_KEY14, A5},
};

const uint8_t NumButtons = sizeof(buttons) / sizeof(button);

// --------------------------------------------
// Serial and EEPROM Functions
// --------------------------------------------

class Data_Controler {
  public:
    void checkForData() {
      receiveData();
      processData();
    }

    // load all data from eeprom
    void loadData() {
      // brightness data
      int oldBrightness = EEPROM.read(0);
      if (oldBrightness) {
        LED.brightness = oldBrightness;
      }
      // load button data
      for (int i = 1; i < 15; i++) {
        char data = EEPROM.read(i);
        if (data) {
          buttons[i - 1].key = data;
        }
      }
      // load LED Mode
      char mode = EEPROM.read(15);
      if (mode) {
        LED.LED_MODE = mode;
      }
    }

  private:
    const byte numChars = 15; // max length of the array
    char receivedChars[15]; // array of recieved chars i.e. Numbers in this case
    boolean newData = false; // flag for detecting when naw data is complete
    int lastI = 0; // index of the last button requested by UI

    // called in main loop looking for data from UI
    void receiveData() {
      static boolean recvInProgress = false;
      static byte ndx = 0;
      char startMarker = '<';
      char endMarker = '>';
      char rc;
      while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
        if (recvInProgress == true) {
          if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= numChars) {
              ndx = numChars - 1;
            }
          }
          else {
            receivedChars[ndx] = '\0'; // terminate the string
            recvInProgress = false;
            ndx = 0;
            newData = true;
          }
        }
        else if (rc == startMarker) {
          recvInProgress = true;
        }
      }
    }

    // take the number respose from the UI serial connection parse it and return relavent data
    void processData() {
      if (newData == true) {
        int i = atoi(receivedChars);
        // getting current keybinds for UI
        if (i < 10 && i != 0) {
          lastI = i - 1;
          sendResponse(buttons[lastI + 5].key, buttons[lastI].key);
          newData = false;
          return;
        }
        // getting current encoder binds for UI
        if (i == 12 || i == 13) {
          lastI = i - 1;
          sendResponse(buttons[lastI + 1].key, buttons[lastI - 1].key);
          newData = false;
          return;
        }
        // saving keybinds
        // split off first number
        int mod = (i / 1000) % 10;
        // checking to see if a modifier key was sent
        if (mod > 0) {
          // get keys back from sent data
          int thousands = mod * 1000;
          int key = i - thousands;
          mod = mod + 127;
          // save the data
          if (lastI == 11 || lastI == 12) {
            saveEncoderData(lastI, mod, key);
          } else {
            saveButtonData(lastI, mod, key);
          }
          // send confirmation to app the save has been made
          sendResponse(mod, key);
        } else {
          // save the data
          if (lastI == 11 || lastI == 12) {
            saveEncoderData(lastI, 0, i);
          } else {
            saveButtonData(lastI, 0, i);
          }
          // send confirmation to app the save has been made
          sendResponse(0, i);
        }
        newData = false;
      }
    }

    // sends data back to UI
    void sendResponse(int mod, int key) {
      Serial.println(':' + String(mod) + ':' + String(key));
    }

    // saves data about encoder bind to eeprom
    void saveEncoderData(int index, int mod, int key) {
      buttons[index + 1].key = mod;
      EEPROM.update(index + 2, mod);
      buttons[index - 1].key = key;
      EEPROM.update(index, key);
    }

    // saves data about button bind to eeprom
    void saveButtonData(int index, int mod, int key) {
      buttons[index + 5].key = mod;
      EEPROM.update(index + 6, mod);
      buttons[index].key = key;
      EEPROM.update(index + 1, key);
    }
};

Data_Controler   DATA;


// --------------------------------------------
// Encoder Functions
// --------------------------------------------
Encoder myEnc(2, 3);
class Knob_Control {
  public:
    void read() {
      long newPosition = myEnc.read();
      if (newPosition != oldPosition) {
        // leds on adjustable mode and function pey pressed
        if (!digitalRead(funct_key) && LED.LED_MODE == 0) {
          int lowest = 255 - LED.brightnessIncriment;
          if (newPosition > oldPosition) {
            // turn up the brightness
            if (LED.brightness > 0 && LED.brightness <= lowest) {
              LED.brightness = LED.brightness - LED.brightnessIncriment;
              EEPROM.update(0, LED.brightness);
            }
          }
          if (newPosition < oldPosition) {
            // turn down the brightness
            if (LED.brightness >= 0 && LED.brightness < lowest) {
              LED.brightness = LED.brightness + LED.brightnessIncriment;
              EEPROM.update(0, LED.brightness);
            }
          }
        }
        // function button not pressed
        else {
          if (newPosition > oldPosition) {
            if (buttons[12].key) {
              Keyboard.press(buttons[12].key);
            }
            Keyboard.press(buttons[10].key);
          }
          if (newPosition < oldPosition) {
            if (buttons[13].key) {
              Keyboard.press(buttons[13].key);
            }
            Keyboard.press(buttons[11].key);
          }
        }
        oldPosition = newPosition;
        Keyboard.releaseAll();
      }
    }
  private:
    long oldPosition  = -999;
};

Knob_Control   KNOB;

// setup program
void setup() {
  // Safety check. Ground pin #1 (RX) to cancel keyboard inputs.
  pinMode(1, INPUT_PULLUP);
  if (!digitalRead(1)) {
    failsafe();
  }
  Serial.begin(9600);
  Keyboard.begin();
  DATA.loadData();
  for (int i = 0; i < 5; i++) {
    pinMode(LED.leds[i], INPUT);
  }
  pinMode(funct_key, INPUT_PULLUP);
  for (int i = 0; i < NumButtons; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
}

// main program loop
void loop() {
  if (Serial) {
    DATA.checkForData();
  }
  KNOB.read();
  // check if buttons are pressed
  for (int i = 0; i < 5; i++) {
    buttons[i + 5].update();
    buttons[i].update();
  }
  LED.doWork();
  Serial.println(LED.brightness);
}

//
void failsafe() {
  for (;;) {} // Just going to hang out here for awhile :D
}
