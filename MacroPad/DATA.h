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
        return;
      }
      File cFile = SD.open(filename);
      DynamicJsonDocument doc(objectCapacity);
      DeserializationError error = deserializeJson(doc, cFile);
      if (error) {
        cFile.close();
        return;
      }
      LED.setBrightness(doc["brightness"]);
      LED.setLEDMode(doc["LED_MODE"]);
      for (byte i = 0; i < 21; i++) {
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
        return;
      }
      if (serializeJson(createJSON(), cFile) == 0) {

      }
      cFile.close();
    }

  private:
    const size_t objectCapacity = JSON_ARRAY_SIZE(21) + JSON_OBJECT_SIZE(3) + 40;
    char receivedChars[150]; // array for catching the JSON string from Serial
    boolean newData = false; // flag for detecting when new data is complete
    byte lastI = 0; // index of the last button requested by UI
    const char *filename = "/config.txt";
    byte lstMode = LED.getLEDMode();  // by getting current mode from LED class we avoid unneeded save on bootup

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
            if (ndx >= 150) {
              ndx = 150 - 1;
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

    // processs incomming JSON object from UI
    void processData() {
      if (newData == true) {
        newData = false;
        DynamicJsonDocument doc(objectCapacity);
        DeserializationError error = deserializeJson(doc, receivedChars);
        if (error) {
          newData = false;
          return;
        }
        if (doc["LED_MODE"] != lstMode) {
          lstMode = doc["LED_MODE"];
          LED.setLEDMode(doc["LED_MODE"]);
        }
        for (byte i = 0; i < 21; i++) {
          buttons[i].key = doc["buttons"][i];
        }
        saveConfig();
      }
    }

    // create a JSON object of the current data
    DynamicJsonDocument createJSON() {
      DynamicJsonDocument doc(objectCapacity);
      doc["brightness"] = LED.getBrightness();
      doc["LED_MODE"] = LED.getLEDMode();
      JsonArray data = doc.createNestedArray("buttons");
      for (byte i = 0; i < 21; i++) {
        data.add(buttons[i].key);
      }
      // Update sd card data
      if (doc["LED_MODE"] != lstMode) {
        lstMode = doc["LED_MODE"];
        saveConfig();
      }
      return doc;
    }
};

Data_Controler   DATA;
#endif
