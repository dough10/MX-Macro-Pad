/*
----------------------------------
          BUTTONS!!!!
----------------------------------
*/
#ifndef BUTTONS_h
#define BUTTONS_h

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
      byte led_mode = LED.getLEDMode();
      bool functKeyPress = !digitalRead(FUNCT_KEY);
      // if the if is the if
      if (state && functKeyPress) {
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
      if (functKeyPress) {
        return;
      }
      state ? Keyboard.press(key) : Keyboard.release(key);
      pressed = state;
      // on press LED mode brightness control
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
#endif
