/*
----------------------------------
   Serial and SD Card Functions
----------------------------------
*/
#ifndef DATA_h
#define DATA_h

class Data_Controler {

  public:
    void init() {
      if (!SD.begin(CSPIN)) {
        Serial.println(F("SD Card initialization failed! using default configuration"));
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
      serializeJson(createJSON(), Serial);
      Serial.println();
    }

    //write config to SD card
    void saveConfig() {
      SD.remove(filename);
      File cFile = SD.open(filename, FILE_WRITE);
      if (!cFile) {
        Serial.println(F("Failed to create file"));
        return;
      }
      if (serializeJson(createJSON(), cFile) == 0) {
        Serial.println(F("Failed to write to file"));
      }
      cFile.close();
    }

  private:
    char receivedChars[15]; // array of recieved chars i.e. Numbers in this case
    boolean newData = false; // flag for detecting when naw data is complete
    int lastI = 0; // index of the last button requested by UI
    const char *filename = "/config.txt";
    int lstMode = LED.getLEDMode();  // by getting current mode from LED class we avoid unneeded save on bootup

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
            if (ndx >= 15) {
              ndx = 15 - 1;
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
        // setting index of the button to me changed
        if (i < 14 && i != 0) {
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

    DynamicJsonDocument createJSON() {
      const size_t capacity = JSON_ARRAY_SIZE(14) + JSON_OBJECT_SIZE(3);
      DynamicJsonDocument doc(capacity);
      doc["brightness"] = LED.getBrightness();
      doc["LED_MODE"] = LED.getLEDMode();
      JsonArray data = doc.createNestedArray("buttons");
      for (int i = 0; i < 14; i++) {
        data.add(buttons[i].key);
      }
      // Update sd card data
      if (doc["LED_MODE"] != lstMode) {
        saveConfig();
        lstMode = doc["LED_MODE"];
      }
      return doc;
    }
};

Data_Controler   DATA;
#endif
