#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>  
#include <WiFiManager.h>  
#include "FS.h"

/////////////////////////
///Hardware Definitions//
/////////////////////////

#define debounce 50 // ms debounce period to prevent flickering when pressing or releasing the button
#define holdTime 7000 // ms hold period: how long to wait for press+hold event

const int button = 12;
const int ledRed = 13;
const int ledGreen = 14;
const int ledWhite = 11;

// Button Actions

int buttonVal     = 0;      // value read from button
int buttonLast    = 0;      // buffered value of the button's previous state
long btnDnTime;             // time the button was pressed down
long btnUpTime;             // time the button was released
boolean ignoreUp  = false;  // whether to ignore the button release because the click+hold was triggered

WiFiManager wifiManager;
int TRIGGER_PIN = 15;


String customerDetailPage = "";
String resultPage = "";
  

void setup() {
  
    Serial.begin(115200);

    ///// Initiate WiFi ///////
    
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setMinimumSignalQuality(10);
    wifiManager.setConfigPortalTimeout(60);
    wifiManager.autoConnect("MachForce", "1234567");
    // ^BLOCKS UNTIL TRUE^ //
    //if you get here you have connected to the WiFi
    Serial.println("Successfully Connected!");

     ///// Initiate Hardware ///////
    //initiateHardware();

    ///// STAGE 1 - SPIFF Format & Access Point ///////
    SPIFFS.begin();
    Serial.println("SPIFFS File Formatting, please wait...");
    SPIFFS.format();
    // Check SPIFFS File for customer data
    File f = SPIFFS.open("/f.txt", "w");
    if (!f) {
        Serial.println("File Open Failed");
    }
    Serial.print("File Size: ");
    Serial.println(f.size());
    if( f.size() == 0){
        Serial.println("====== Writing To SPIFFS file =======");
        f.print("Scabby Boogey Boo!");
        f.close();
    } else {
        Serial.println("====== FILE IS NOT EMPTY =======");
    }
    File F = SPIFFS.open("/f.txt", "r");
    if (!F) {
        Serial.println("File Open Failed");
    } else { 
        Serial.println("====== Reading from SPIFFS file =======");
        String s = F.readStringUntil('\n');
        Serial.println(s);
    }
    
}

void loop() {
    
      // Read the state of the button
      buttonVal = digitalRead(TRIGGER_PIN);
      
      // Test for button pressed and store the down time
      if (buttonVal == HIGH && buttonLast == LOW && (millis() - btnUpTime) > long(debounce)){
        btnDnTime = millis();
      }
      
      // Test for button release and store the up time
      if (buttonVal == LOW && buttonLast == HIGH && (millis() - btnDnTime) > long(debounce)){
        if (ignoreUp == false){
            shortPress();
        } else { 
            ignoreUp = false;
        }
        btnUpTime = millis();
      }
      
      // Test for button held down for longer than the hold time
      if (buttonVal == HIGH && (millis() - btnDnTime) > long(holdTime)){
        longHold();
        ignoreUp = true;
        btnDnTime = millis();
      }
      
      buttonLast = buttonVal;
}


void StageOne(){
//    SPIFFS.begin();
//    
//    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
//    digitalWrite(ledWhite,HIGH);
//    result = SPIFFS.format();
//    if(result == true){
//      digitalWrite(ledWhite, LOW);
//      digitalWrite(ledGreen, HIGH);
//      delay(200);
//      digitalWrite(ledGreen, HIGH);
//      delay(200);
//      digitalWrite(ledGreen, HIGH);
//    }
      
//    server.begin();
}

void StageTwo(){
    
    ///// Populate HTML For Client ///////
  
    customerDetailPage += "<html>"; 
    customerDetailPage += "<head>";
    customerDetailPage += "<!-- Latest compiled and minified CSS -->";
    customerDetailPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">";
    customerDetailPage += "<!-- Optional theme -->";
    customerDetailPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">";
    customerDetailPage += "<!-- Latest compiled and minified JavaScript -->";
    customerDetailPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>";
    customerDetailPage += "</head>";
    customerDetailPage += "<body>";
    customerDetailPage += "<div class=\"container\" id=\"container\" style=\"margin-left: 30px; line-height: 10px; position: relative; \">";
    customerDetailPage += "<div class=\"row\">";
    customerDetailPage += "<div class=\"cols-xs-12 col-md-9 pull-md-3 bd-content\" style=\"padding-bottom: 15px;\">";
    customerDetailPage += "<h1 style=\"font-family:\'Oswald\', sans-serif; padding-bottom 20px;\">Welcome To MachForce <span style=\"font-weight: bold; color: dodgerblue;\">Device</span> Setup</h1>";
    customerDetailPage += "</div>";
    customerDetailPage += "<br/><br/>";
    customerDetailPage += "<div class=\"cols-xs-12 col-md-9 pull-md-3 bd-content\">";
    customerDetailPage += "<p>Please enter customer information for button below.</p>";
    customerDetailPage += "</div>";
    customerDetailPage += "<br/><br/>";
    customerDetailPage += "<div class=\"panel panel-default\" style=\"width: 80%; margin-top: 5%;\">";
    customerDetailPage += "<div class=\"panel-body\">";
    customerDetailPage += "<form method=\"post\" action=\"customerDetails\">";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"customerName\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Customer Name </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"customerName\" name=\"customerName\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"cAccount\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Customer Account # </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"cAccount\" name=\"cAccount\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"callbackNumber\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Callback Number (Cell) </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"callbackNumber\" name=\"callbackNumber\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"vAccount\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Vendor Account # </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"vAccount\" name=\"vAccount\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"groupId\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Group ID </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"groupId\" name=\"groupId\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"productForService\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Product<br/>Name </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"productForService\" name=\"productForService\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"modelNumber\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px; padding-top: 10px;\">Model # </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"modelNumber\" name=\"modelNumber\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<button type=\"submit\" class=\"btn btn-primary\">Submit</button>";
    customerDetailPage += "</form>";
    customerDetailPage += "</div>"; // <--- panel-body
    customerDetailPage += "</div>"; // <--- panel panel-default
    customerDetailPage += "</div>"; // <--- row
    customerDetailPage += "</div>"; // <--- container
    customerDetailPage += "</body>"; // <--- body
    customerDetailPage += "</html>"; // <--- html
    
    
    resultPage += "<html>";
    resultPage += "<head>"; // <--- head
    resultPage += "<!-- Latest compiled and minified CSS -->";
    resultPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">";
    resultPage += "<!-- Optional theme -->";
    resultPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">";
    resultPage += "<!-- Latest compiled and minified JavaScript -->";
    resultPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>";
    resultPage += "</head>"; // <--- head
    resultPage += "<body>"; 
    resultPage += "<div class=\"container\">";
    resultPage += "<div class=\"\" style=\"padding-top: 50px;\">"; // <--- ??????
    resultPage += "<div class=\"row\">";
    resultPage += "<h1 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Thank You</h1>";
    resultPage += "<br/>";
    resultPage += "<h2 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Customer <span style=\"font-weight: bold; color: dodgerblue;\">Device</span> Has Now Been Configured</h2>";
    resultPage += "</div>";
    resultPage += "<div style=\"text-align: center;\">";
    resultPage += "<br/>";
    resultPage += "<br/>";
    resultPage += "<img src=\"http://res.cloudinary.com/ha3eifngn/image/upload/v1480980127/MachForce_Logo_fvngw9.png\" height=\"75px\">";
    resultPage += "</div>"; 
    resultPage += "</div>";  // <--- ?????
    resultPage += "</div>";  // <--- container
    resultPage += "</body>"; // <--- body
    resultPage += "</html>"; // <--- html  
}

void initiateHardware(){
    Serial.print("Hardware Inititiating..............");
    digitalWrite(TRIGGER_PIN, LOW);
    pinMode(button, INPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledWhite, OUTPUT);
    pinMode(TRIGGER_PIN, INPUT);
    Serial.println("Hardware Inititiated!");
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void shortPress(){
   // Wake Button up and send customer data to MachForce API for service request
   Serial.println("Sending Message to MachForce for customer Service");
}

void longHold(){
  // RESETS MACHFORCE DEVICE -> Clears WiFi settings and empties SPIFF file, starts from begginning into STAGE 1
   // Empty SPIFF Files here
   wifiManager.resetSettings();
   ESP.restart();
}

