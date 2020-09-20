/*
----------------------------------
        Encoder Functions
----------------------------------
*/

#ifndef KNOB_h
#define KNOB_h

Encoder myEnc(2, 3);
class Knob_Control {
  public:
    void read() {
      long newPosition = myEnc.read();
      // welcome to "if" hell..... it sux here

      // position of encoder has changed
      if (newPosition != oldPosition) {

        // leds on adjustable mode and encoder click pressed
        if (!digitalRead(FUNCT_KEY) && LED.getLEDMode() == 0) {
          byte brightness = LED.getBrightness();
          byte brightnessIncriment = LED.getIncriment();
          byte lowest = 255 - brightnessIncriment;
          // turn right
          if (newPosition > oldPosition) {
            // turn up the brightness
            if (brightness > 0 && brightness <= lowest) {
              LED.setBrightness(brightness - brightnessIncriment);
              newValue = true;
            }
          }
          // turn left
          if (newPosition < oldPosition) {
            // turn down the brightness
            if (brightness >= 0 && brightness < lowest) {
              LED.setBrightness(brightness + brightnessIncriment);
              newValue = true;
            }
          }
        }
        // function button not pressed
        else {
          // turn right
          if (newPosition > oldPosition) {
            // press buttons
            if (buttons[19].key) {
              Keyboard.press(buttons[19].key);
            }
            if (buttons[17].key) {
              Keyboard.press(buttons[17].key);
            }
            Keyboard.press(buttons[15].key);
            // Consumer.write(MEDIA_VOL_UP);
          }
          // turn left
          if (newPosition < oldPosition) {
            // press buttons
            if (buttons[20].key) {
              Keyboard.press(buttons[20].key);
            }
            if (buttons[18].key) {
              Keyboard.press(buttons[18].key);
            }
            Keyboard.press(buttons[16].key);
            // Consumer.write(MEDIA_VOL_DOWN);
          }
        }
        oldPosition = newPosition;
        Keyboard.releaseAll();
      }
      // when idle if brightness value has changed update the config file
      else {
        if (newValue) {
          newValue = false;
          DATA.saveConfig();
        }
      }
    }
  private:
    long oldPosition  = -999;
    bool newValue = false;
};

Knob_Control   KNOB;
#endif
