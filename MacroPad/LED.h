/*
----------------------------------
     Lighting mode functions
----------------------------------
*/

#ifndef LED_h
#define LED_h

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
#endif
