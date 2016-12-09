// Very basic Spiffs example, writing 10 strings to SPIFFS filesystem, and then read them back
// For SPIFFS doc see : https://github.com/esp8266/Arduino/blob/master/doc/filesystem.md
// Compiled in Arduino 1.6.7. Runs OK on Wemos D1 ESP8266 board.

#include "FS.h"

bool result;
bool start;

void setup() {
  Serial.begin(9600);
  Serial.println("\n New Firmware has been uploaded!");
  Serial.println("\n Starting SPIFF File System..");
  delay(3000);
  SPIFFS.begin();
  File f = SPIFFS.open("/f.txt", "r");
      if (!f) {
          Serial.println("file open failed");
      } else { 
          Serial.println("====== Reading from SPIFFS file =======");
          // Read From File
          String s=f.readStringUntil('\n');
          Serial.print("Customer Data To Send: ");
          Serial.println(s);
          f.close();
      }
} 

void loop() {
  
}

