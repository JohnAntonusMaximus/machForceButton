#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

#define debounce 50 // ms debounce period to prevent flickering when pressing or releasing the button
#define holdTime 7000 // ms hold period: how long to wait for press+hold event

WiFiManager wifiManager;
int TRIGGER_PIN = 15;

// Button variables
int buttonVal = 0; // value read from button
int buttonLast = 0; // buffered value of the button's previous state
long btnDnTime; // time the button was pressed down
long btnUpTime; // time the button was released
boolean ignoreUp = false; // whether to ignore the button release because the click+hold was triggered


void setup() {
    digitalWrite(TRIGGER_PIN, LOW);
    pinMode(TRIGGER_PIN, INPUT);
    Serial.begin(115200);
    WiFiManagerParameter custom_text("<div style=\"text-align: center;\"><img src=\"http://res.cloudinary.com/ha3eifngn/image/upload/v1480980127/MachForce_Logo_fvngw9.png\" height=\"75px\"></div>");
    wifiManager.addParameter(&custom_text);
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setMinimumSignalQuality(10);
    wifiManager.setConfigPortalTimeout(60);
    wifiManager.autoConnect("MachForce", "1234567");
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
}

void loop() {
      // Read the state of the button
      buttonVal = digitalRead(TRIGGER_PIN);
      
      // Test for button pressed and store the down time
      if (buttonVal == HIGH && buttonLast == LOW && (millis() - btnUpTime) > long(debounce))
      {
      btnDnTime = millis();
      }
      
      // Test for button release and store the up time
      if (buttonVal == LOW && buttonLast == HIGH && (millis() - btnDnTime) > long(debounce)){
        if (ignoreUp == false){
          event1();
        } else { 
          ignoreUp = false;
        }
      btnUpTime = millis();
      }
      
      // Test for button held down for longer than the hold time
      if (buttonVal == HIGH && (millis() - btnDnTime) > long(holdTime))
      {
      event2();
      ignoreUp = true;
      btnDnTime = millis();
      }
      
      buttonLast = buttonVal;
}


void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void event1(){
    Serial.println("Sending Message to MachForce for customer Service");
}

void event2(){
   wifiManager.resetSettings();
   ESP.restart();
}
