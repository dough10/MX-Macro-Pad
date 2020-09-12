/*

    Macro Keys

   led light brightness values are reverse 0 max and 255 off


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
#define BUTTON_KEY6 0 // button 1 modifier default
#define BUTTON_KEY7 0 // button 2 modifier default
#define BUTTON_KEY8 0 // button 3 modifier default
#define BUTTON_KEY9 0 // button 4 modifier default
#define BUTTON_KEY10 0 // button 5 modifier default
#define BUTTON_KEY11 215 // encoder right default
#define BUTTON_KEY12 216 // encoder left default
#define BUTTON_KEY13 0 // encoder right modifier default
#define BUTTON_KEY14 0 // encoder left modifier default

// --------------------------------------------
// Lib Imports
// --------------------------------------------

// i need <HID-Project.h> for media keys but it makes strange things happen.
// i can't find a glossary of key decimal values
// the key map i am using makes keyboard no work /shrug  (./src/keys.json has current key map)
//#include <HID-Project.h>
#include <Keyboard.h>
#include <Encoder.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>

// --------------------------------------------
// Lighting mode functions
// --------------------------------------------

class LED_Controller {
  public:
    // array of PWM pins
    const int leds[5] = {
      LED_PIN1,
      LED_PIN2,
      LED_PIN3,
      LED_PIN4,
      LED_PIN5,
    };

    // on press led mode
    int brightnesses[5] = {
      255, // button 1 led brightness
      255, // button 2 led brightness
      255, // button 3 led brightness
      255, // button 4 led brightness
      255, // button 5 led brightness
    };

    // change the mode of the LEDs
    void setLEDMode(int mode) {
      LED_MODE = mode;
    }

    // return LED_MODE
    int getLEDMode() {
      return LED_MODE;
    }

    // Set the brights of leds in mode 0
    void setBrightness(int b) {
      brightness = b;
    }

    // returns brightness of led in mode 0
    int getBrightness() {
      return brightness;
    }

    // return the incirment led will be changed by when turning encoder
    int getIncriment() {
      return brightnessIncriment;
    }

    // set all LED values @ once
    void makeLight(int value) {
      for (int i = 0; i < 5; i++) {
        analogWrite(leds[i], value);
      }
    }

    // switch handler for calling of led functions
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
    int LED_MODE = 0; // variable for changeing the LEDS light modes

    // knight rider Mode  https://www.youtube.com/watch?v=rhLejQ00sus your welcome
    int currentLED = 3; // index of the led in the array currently having it's brightness value changed
    int krIncriment = 51; // changeing this value changes the speed of leds
    int krBrightness = 255 - krIncriment; // default brightness value
    int changeBy = -1; // value to incriment led array index

    // always on light mode
    int brightness = 0; // adjustable brightness value
    const int brightnessIncriment = 5; // 1 encoder click = this value in change of led brightness

    // change lED and / or value
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

    // when you press it lights up WOW!!!
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
      int led_mode = LED.getLEDMode();
      // if the if is the if
      if (state && !digitalRead(FUNCT_KEY)) {
        // button 1 sets LEDs to adjustable brightness mode
        if (pin == A0 && led_mode != 0) {
          LED.setLEDMode(0);
          return;
        }
        // button 2 sets LEDs to light on keypress and fade out after
        if (pin == A1 && led_mode != 1) {
          LED.setLEDMode(1);
          LED.makeLight(255);
          return;
        }
        // button 3 sets LEDs to Fade / Breath mode
        if (pin == A2 && led_mode != 2) {
          LED.setLEDMode(2);
          return;
        }
        // knight rider mode
        if (pin == A3 && led_mode != 3) {
          LED.setLEDMode(3);
          LED.makeLight(255);
          return;
        }
        // LED off mode
        if (pin == A4 && led_mode != 4) {
          LED.setLEDMode(4);
          LED.makeLight(255);
          return;
        }
      }
      lastPressed = millis();
      // stop key press if function key held
      if (!digitalRead(FUNCT_KEY)) {
        return;
      }
      state ? Keyboard.press(key) : Keyboard.release(key);
      pressed = state;
      if (state && led_mode == 1) {
        switch (pin) {
          case A0:
            LED.brightnesses[0] = 0;
            break;
          case A1:
            LED.brightnesses[1] = 0;
            break;
          case A2:
            LED.brightnesses[2] = 0;
            break;
          case A3:
            LED.brightnesses[3] = 0;
            break;
          case A4:
            LED.brightnesses[4] = 0;
            break;
        };
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
  {BUTTON_KEY11, FUNCT_KEY},
  {BUTTON_KEY12, FUNCT_KEY},
  {BUTTON_KEY13, FUNCT_KEY},
  {BUTTON_KEY14, FUNCT_KEY},
};

const uint8_t NumButtons = sizeof(buttons) / sizeof(button);

// --------------------------------------------
// Serial and SD Card Functions
// --------------------------------------------

class Data_Controler {
  public:
    void init() {
      if (!SD.begin(CSPIN)) {
        Serial.println("SD Card initialization failed! using default configuration");
        return;
      }
      File cFile = SD.open(filename);
      const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(3) + 40;
      DynamicJsonDocument doc(capacity);
      DeserializationError error = deserializeJson(doc, cFile);
      if (error) {
        Serial.println(F("Failed to read file, using default configuration"));
        cFile.close();
        return;
      }
      LED.setBrightness(doc["brightness"]);
      LED.setLEDMode(doc["LED_MODE"]);
      for (int i = 0; i < 14; i++) {
        buttons[i].key = doc["buttons"][i];
      }
      cFile.close();
    }

    void checkForData() {
      receiveData();
      processData();
    }

    // make data great again
    void printJSON() {
      const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(3);
      DynamicJsonDocument doc(capacity);
      doc["brightness"] = LED.getBrightness();
      doc["LED_MODE"] = LED.getLEDMode();
      JsonArray data = doc.createNestedArray("buttons");
      for (int i = 0; i < 14; i++) {
        data.add(buttons[i].key);
      }
      serializeJson(doc, Serial);
      Serial.println();
    }

  private:
    const byte numChars = 15; // max length of the array
    char receivedChars[15]; // array of recieved chars i.e. Numbers in this case
    boolean newData = false; // flag for detecting when naw data is complete
    int lastI = 0; // index of the last button requested by UI
    const char *filename = "/config.txt";
    struct Config {
      int brightness;
      int LED_MODE;
      int buttons[14];
    };
    Config config;

    // looking for data from UI
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

    // this needs to receive JSON
    //
    // i should write functions to look for other start and end markers to better seperate data in place of using the maths (not a backend eng. lol not a frontend eng either)
    void processData() {
      if (newData == true) {
        int i = atoi(receivedChars);
        // getting current keybinds for UI
        if (i < 10 && i != 0) {
          lastI = i - 1;
          newData = false;
          return;
        }
        // getting current encoder binds for UI
        if (i == 12 || i == 13) {
          lastI = i - 1;
          newData = false;
          return;
        }
        // saving keybinds
        // split off first number
        int mod = (i / 1000) % 10;
        // if a modifier key was sent
        if (mod > 0) {
          // get keys from sent data
          int thousands = mod * 1000;
          int key = i - thousands;
          mod = mod + 127;
          // save the data
          if (lastI == 11 || lastI == 12) {
            setEncoderBind(lastI, mod, key);
          }
          else {
            setButtonBind(lastI, mod, key);
          }
        }
        // no modifier key
        else {
          // save the data
          if (lastI == 11 || lastI == 12) {
            setEncoderBind(lastI, 0, i);
          }
          else {
            setButtonBind(lastI, 0, i);
          }
        }
        newData = false;
      }
    }

    // save data about encoder
    void setEncoderBind(int index, int mod, int key) {
      buttons[index + 1].key = mod;
      buttons[index - 1].key = key;
      saveConfig();
    }

    // save data about button
    void setButtonBind(int index, int mod, int key) {
      buttons[index + 5].key = mod;
      buttons[index].key = key;
      saveConfig();
    }

    //
    void saveConfig() {
      SD.remove(filename);
      File cFile = SD.open(filename, FILE_WRITE);
      if (!cFile) {
        Serial.println(F("Failed to create file"));
        return;
      }
      const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(3);
      DynamicJsonDocument doc(capacity);
      doc["brightness"] = LED.getBrightness();
      doc["LED_MODE"] = LED.getLEDMode();
      JsonArray data = doc.createNestedArray("buttons");
      for (int i = 0; i < 14; i++) {
        data.add(buttons[i].key);
      }
      if (serializeJson(doc, cFile) == 0) {
        Serial.println(F("Failed to write to file"));
      }
      cFile.close();
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
      // welcome to "if" hell..... it sux here
      if (newPosition != oldPosition) {
        // leds on adjustable mode and function key pressed
        if (!digitalRead(FUNCT_KEY) && LED.getLEDMode() == 0) {
          int brightness = LED.getBrightness();
          int brightnessIncriment = LED.getIncriment();
          int lowest = 255 - brightnessIncriment;
          // turn right
          if (newPosition > oldPosition) {
            // turn up the brightness
            if (brightness > 0 && brightness <= lowest) {
              LED.setBrightness(brightness - brightnessIncriment);
            }
          }
          // turn left
          if (newPosition < oldPosition) {
            // turn down the brightness
            if (brightness >= 0 && brightness < lowest) {
              LED.setBrightness(brightness + brightnessIncriment);
            }
          }
        }
        // function button not pressed
        else {
          // turn right
          if (newPosition > oldPosition) {
            // press buttons
            if (buttons[12].key) {
              Keyboard.press(buttons[12].key);
            }
            Keyboard.press(buttons[10].key);
          }
          // turn left
          if (newPosition < oldPosition) {
            // press buttons
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

// get shit ready
void setup() {
  // Safety check. Ground pin #1 (RX) to cancel keyboard inputs.
  // this was in the tutorial i started with looked cool so i kept it :P
  pinMode(1, INPUT_PULLUP);
  if (!digitalRead(1)) {
    failsafe();
  }
  Serial.begin(9600);
  Keyboard.begin();
  for (int i = 0; i < 5; i++) {
    pinMode(LED.leds[i], INPUT);
  }
  pinMode(FUNCT_KEY, INPUT_PULLUP);
  for (int i = 0; i < NumButtons; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }
  DATA.init();
}

// loop de loop
void loop() {
  if (Serial) {
    DATA.checkForData();
  }
  KNOB.read();
  LED.doWork();
  // check if buttons are pressed
  for (int i = 0; i < 5; i++) {
    buttons[i + 5].update();
    buttons[i].update();
  }
  // update UI
  DATA.printJSON();
}

//
void failsafe() {
  for (;;) {} // Just going to hang out here for awhile :D
}
