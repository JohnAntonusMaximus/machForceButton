#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>  
#include <WiFiManager.h>  
#include "FS.h"
#include <Arduino.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
#include "Base64.h"
#include "AES.h"


/////////////////////////
///Firmware Version//////
/////////////////////////

const String version = "1.0";

/////////////////////////
///Hardware Definitions//
/////////////////////////

#define USE_SERIAL Serial
int relayPin = 5;
int counter;
int ledRed = 14;
int ledGreen = 13;
int ledBlue = 12;
int ledWhite = 10;
int TRIGGER_PIN = 15;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;
volatile int watchDogCount = 0;

//////////////////////////////////
//////WiFi & Server Definitions///
//////////////////////////////////

WiFiManager wifiManager;
ESP8266WebServer server(80);

//////////////////////////////////////
//////////Customer Data Vars//////////
//////////////////////////////////////
boolean customerDetailsFlag;
String  customerDetails;
String  payload;

/////////////////////////////////////////
////HTML Content & Customer Data Vars////
/////////////////////////////////////////

String  customerDetailPage;
String  resultPage;
String  authErrorPage;
String  errorPage;
String  serverErrorPage;

/////////////////////////////////
///AES128 Encryption Defintions//
/////////////////////////////////

AES aes;
byte key[] = { 0x1B, 0x7F, 0x20, 0x12, 0xAE, 0x11, 0x1E, 0x88, 0xA6, 0xF9, 0x16, 0x66, 0x4F, 0xF8, 0x3C, 0x4F };
byte my_iv[N_BLOCK] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

/////////////////////////////////////////
/////////////MachForce API///////////////
/////////////////////////////////////////
const char* host = "http://machforce-api-multi.herokuapp.com";

void setup(void) {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, HIGH);
   
    Serial.begin(115200);
    Serial.println();
    Serial.print("Welcome To MachForce! Version Number: ");
    Serial.println(version);
    
    ///// Initiate Hardware ///////
    Serial.print("Hardware Inititiating..............");
    digitalWrite(TRIGGER_PIN, LOW);
    pinMode(TRIGGER_PIN, INPUT);
    pinMode(ledBlue, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledWhite, OUTPUT);
    digitalWrite(ledWhite, LOW);
    digitalWrite(ledRed, LOW);
    digitalWrite(ledGreen, LOW);
    digitalWrite(ledBlue, LOW);
    Serial.println("Hardware Inititiated!");
    //initiateHardware();
    Serial.println("CHECKPOINT!");

    digitalWrite(ledWhite, HIGH);

    ///// Initiate WiFi ///////
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setMinimumSignalQuality(10);
    wifiManager.setConfigPortalTimeout(30); // This will determine when the relay shuts off
    bool WiFiConnected = wifiManager.autoConnect("MachForce", "simians-eight"); // checkMillis needs to put in this function
    if(WiFiConnected == false){
      digitalWrite(relayPin, LOW);
    }
    // ^BLOCKS UNTIL TRUE --- Will automatically go into WiFi Manager if false //

    // Check For Factory Reset Button
    checkResetButton();
    
    //Connected to the WiFi
    Serial.println("Successfully Connected!");

    ///// Run OTA Firmware Updater ///////
    runUpdater();
    
///// STAGE 1 - SPIFF Format & Access Point ///////
    SPIFFS.begin();
   
    // Check SPIFFS File for customer data
    File f = SPIFFS.open("/f.txt", "r");
    if (!f) {
        Serial.println("File Open Failed");
    }
    Serial.print("File Size: ");
    Serial.println(f.size());
    if( f.size() == 0){
        customerDetailsFlag = false;
        // Enter web server mode   
        StageTwo();
    } else {
      payload = f.readStringUntil('\n');
      Serial.println("Customer Details Exist, Recorded As:");
      Serial.println(payload);
      f.close();
      shortPress();
    }
    
}

void loop() {
}

///// STAGE 2 - Configure Customer Details  ///////
void StageTwo(){

    //////////////////////////////
    //////Server Definitions//////
    //////////////////////////////

    MDNSResponder mdns;
    
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
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"customerName\" name=\"customerName\" pattern=\"[0-9A-Za-z&#92;-&#92;s]{1,40}\" maxlength=\"20\" required title=\"Customer name cannot exceed 40 characters and cannot contain symbols.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"cAccount\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Customer Account # </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"cAccount\" name=\"cAccount\" pattern=\"[0-9A-Za-z&#92;+&#92;-&#92;s]+\" maxlength=\"20\" required title=\"Entered data contains one or more illegal characters. Please do not use any symbols.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"callbackNumber\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Callback Number (Cell) </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"callbackNumber\" name=\"callbackNumber\" pattern=\"[0-9]{10}\" maxlength=\"10\" required title=\"Please provide a valid 10-digit mobile phone number.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"vAccount\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Vendor Account # </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"vAccount\" name=\"vAccount\" pattern=\"MF-[0-9A-Za-z]{14}\" maxlength=\"17\" required title=\"This is not a valid MachForce account number. Account numbers are case & length sensitive, and cannot contain symbols.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"groupId\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Group ID </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"groupId\" name=\"groupId\" pattern=\"[0-9A-Za-z]{9}\" maxlength=\"9\" required title=\"Please enter a valid group ID. Group ID is case sensitive and cannot contain symbols.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"productForService\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Product<br/>Name </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"productForService\" name=\"productForService\" pattern=\"[^;\'{}&#92;&quot;]+\" maxlength=\"20\" required title=\"Entered data contains one or more illegal characters. Please do not use any symbols.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<div class=\"form-group row\">";
    customerDetailPage += "<label for=\"modelNumber\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px; padding-top: 10px;\">Model # </label>";
    customerDetailPage += "<div class=\"col-xs-4\">";
    customerDetailPage += "<input class=\"form-control\" type=\"text\" id=\"modelNumber\" name=\"modelNumber\" maxlength=\"20\" pattern=\"[^;\'{}&#92;&quot;]+\" required title=\"Entered data contains one or more illegal characters. Please do not use any symbols.\">";
    customerDetailPage += "</div>";
    customerDetailPage += "</div>";
    customerDetailPage += "<button type=\"submit\" class=\"btn btn-primary\">Submit</button>";
    customerDetailPage += "</form>";
    customerDetailPage += "</div>"; // <--- panel-body
    customerDetailPage += "</div>"; // <--- panel panel-default
    customerDetailPage += "</div>"; // <--- row
    customerDetailPage += "</div>"; // <--- container
    customerDetailPage += "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.4/jquery.min.js\"></script>";
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

    authErrorPage += "<html>";
    authErrorPage += "<head>"; // <--- head
    authErrorPage += "<!-- Latest compiled and minified CSS -->";
    authErrorPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">";
    authErrorPage += "<!-- Optional theme -->";
    authErrorPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">";
    authErrorPage += "<!-- Latest compiled and minified JavaScript -->";
    authErrorPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>";
    authErrorPage += "</head>"; // <--- head
    authErrorPage += "<body>"; 
    authErrorPage += "<div class=\"container\">";
    authErrorPage += "<div class=\"\" style=\"padding-top: 50px;\">"; // <--- ??????
    authErrorPage += "<div class=\"row\">";
    authErrorPage += "<h1 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Whoops! Something Went Wrong!</h1>";
    authErrorPage += "<br/>";
    authErrorPage += "<h2 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">We couldn't <span style=\"font-weight: bold; color: dodgerblue;\">Authenticate</span> The Information You Provided. <BR>Please double-check all form fields and try again. All fields are case-sensitive</h2>";
    authErrorPage += "</div>";
    authErrorPage += "<div style=\"text-align: center;\">";
    authErrorPage += "<br/>";
    authErrorPage += "<br/>";
    authErrorPage += "<img src=\"http://res.cloudinary.com/ha3eifngn/image/upload/v1480980127/MachForce_Logo_fvngw9.png\" height=\"75px\">";
    authErrorPage += "</div>"; 
    authErrorPage += "</div>";  // <--- ?????
    authErrorPage += "</div>";  // <--- container
    authErrorPage += "</body>"; // <--- body
    authErrorPage += "</html>"; // <--- html 

    errorPage += "<html>";
    errorPage += "<head>"; // <--- head
    errorPage += "<!-- Latest compiled and minified CSS -->";
    errorPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">";
    errorPage += "<!-- Optional theme -->";
    errorPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">";
    errorPage += "<!-- Latest compiled and minified JavaScript -->";
    errorPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>";
    errorPage += "</head>"; // <--- head
    errorPage += "<body>"; 
    errorPage += "<div class=\"container\">";
    errorPage += "<div class=\"\" style=\"padding-top: 50px;\">"; // <--- ??????
    errorPage += "<div class=\"row\">";
    errorPage += "<h1 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Whoops! Something Went Wrong!</h1>";
    errorPage += "<br/>";
    errorPage += "<h2 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Nothing Was Sent. Please reset the device to factory settings and try again.</h2>";
    errorPage += "</div>";
    errorPage += "<div style=\"text-align: center;\">";
    errorPage += "<br/>";
    errorPage += "<br/>";
    errorPage += "<img src=\"http://res.cloudinary.com/ha3eifngn/image/upload/v1480980127/MachForce_Logo_fvngw9.png\" height=\"75px\">";
    errorPage += "</div>"; 
    errorPage += "</div>";  // <--- ?????
    errorPage += "</div>";  // <--- container
    errorPage += "</body>"; // <--- body
    errorPage += "</html>"; // <--- html

    serverErrorPage += "<html>";
    serverErrorPage += "<head>"; // <--- head
    serverErrorPage += "<!-- Latest compiled and minified CSS -->";
    serverErrorPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">";
    serverErrorPage += "<!-- Optional theme -->";
    serverErrorPage += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">";
    serverErrorPage += "<!-- Latest compiled and minified JavaScript -->";
    serverErrorPage += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>";
    serverErrorPage += "</head>"; // <--- head
    serverErrorPage += "<body>"; 
    serverErrorPage += "<div class=\"container\">";
    serverErrorPage += "<div class=\"\" style=\"padding-top: 50px;\">"; // <--- ??????
    serverErrorPage += "<div class=\"row\">";
    serverErrorPage += "<h1 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Whoops! Something Went Wrong!</h1>";
    serverErrorPage += "<br/>";
    serverErrorPage += "<h2 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">There was an error on the server. Please contact MachForce and report the problem.</h2>";
    serverErrorPage += "</div>";
    serverErrorPage += "<div style=\"text-align: center;\">";
    serverErrorPage += "<br/>";
    serverErrorPage += "<br/>";
    serverErrorPage += "<img src=\"http://res.cloudinary.com/ha3eifngn/image/upload/v1480980127/MachForce_Logo_fvngw9.png\" height=\"75px\">";
    serverErrorPage += "</div>"; 
    serverErrorPage += "</div>";  // <--- ?????
    serverErrorPage += "</div>";  // <--- container
    serverErrorPage += "</body>"; // <--- body
    serverErrorPage += "</html>"; // <--- html

    char localServer[] = "machforce";

    if(mdns.begin(localServer)){
       Serial.println("MDNS responder started");
    }
    
    server.on("/", [](){
      server.send(200,"text/html", customerDetailPage);
    });
    
    server.on("/customerDetails", handle_form);
    
    server.on("/setupResult", [](){
      server.send(200,"text/html", resultPage);
    }); 
    
    server.begin();
    
    Serial.println("HTTP server started");
    Serial.println("Customer configuration server started. Please navigate open a browser and navigate to: ");
    Serial.print(localServer);
    Serial.println(".local");
    digitalWrite(ledWhite, LOW);
    counter = 0;
    int relayInterval = 45;
    while(customerDetailsFlag == false && counter < relayInterval){ // the counter is what will determine when the relay shuts off
      checkMillis(ledBlue);
      server.handleClient();
    }
    if(counter >= relayInterval){
      digitalWrite(relayPin, LOW);
    }
    counter = 0;
    digitalWrite(ledBlue, LOW);
}

void checkResetButton(){
           //Read the state of the button
        int buttonVal = digitalRead(TRIGGER_PIN); 
        
        // Test for button held down for longer than the hold time
        if (buttonVal == HIGH){
          factoryReset();
        }
}

void initiateHardware(){
    Serial.print("Hardware Inititiating..............");
    digitalWrite(TRIGGER_PIN, LOW);
    pinMode(TRIGGER_PIN, INPUT);
    pinMode(ledBlue, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledWhite, OUTPUT);
    Serial.println("Hardware Inititiated!");
}

void checkMillis(int ledColor){
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }
    
    // set the LED with the ledState of the variable:
    digitalWrite(ledColor, ledState);
    ESP.wdtFeed();
    counter++;
    Serial.println(counter);
  }
  
}

void flashLight(int ledColor){
  digitalWrite(ledColor, HIGH);
  delay(100);
  digitalWrite(ledColor, LOW);
  delay(100);
  digitalWrite(ledColor, HIGH);
  delay(100);
  digitalWrite(ledColor, LOW);
  delay(100);
  digitalWrite(ledColor, HIGH);
  delay(100);
  digitalWrite(ledColor, LOW);
  delay(100);
  digitalWrite(ledColor, HIGH);
  delay(100);
  digitalWrite(ledColor, LOW);
  delay(100);
  digitalWrite(ledColor, HIGH);
  delay(100);
  digitalWrite(ledColor, LOW);
  delay(100);
}

void configModeCallback (WiFiManager *myWiFiManager) {
  wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void shortPress(){
  digitalWrite(ledWhite, HIGH);
  delay(500);
  digitalWrite(ledWhite, LOW);
  delay(500);

  digitalWrite(ledWhite, HIGH);
  delay(500);
  digitalWrite(ledWhite, LOW);
  delay(500);

  digitalWrite(ledWhite, HIGH);
  delay(500);
  digitalWrite(ledWhite, LOW);
  delay(500);

  digitalWrite(ledWhite, HIGH);
  delay(500);
  digitalWrite(ledWhite, LOW);
  delay(500);
  
  Serial.println("Service Requested! Sending...");
  Serial.println(payload);
  int len = payload.length();
  String lenString = String(len);
  HTTPClient http;
  char* path = "/post";

  http.begin("http://api.machforce.io/wrong");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(payload);
  if(httpCode == 200) {
    http.writeToStream(&Serial);
    http.end();
    digitalWrite(ledWhite, LOW);
    digitalWrite(ledGreen, HIGH);
    delay(5000);
    digitalWrite(ledGreen, LOW);
    digitalWrite(relayPin, LOW);
  } else {
    http.writeToStream(&Serial);
    http.end();
    digitalWrite(ledWhite, LOW);
    digitalWrite(ledRed, HIGH);
    delay(5000);
    digitalWrite(ledRed, LOW);
    digitalWrite(relayPin, LOW);
  }
}

void factoryReset(){
  // RESETS MACHFORCE DEVICE -> Clears WiFi settings and empties SPIFF file, starts from begginning into STAGE 1
   // Empty SPIFF Files here
  digitalWrite(ledRed, HIGH);
  digitalWrite(ledWhite,HIGH);
  Serial.print("Restoring to factory settings, please wait...");
  SPIFFS.format();
  wifiManager.resetSettings();
  Serial.println("Done! Restarting now.");
  digitalWrite(ledRed, LOW);
  digitalWrite(ledWhite,LOW);
  flashLight(ledWhite);
  ESP.restart();
}

void handle_form() { 
  customerDetails = "";
  payload         = "";                                      
  String customerName       = server.arg("customerName");
  String cAccount           = server.arg("cAccount");
  String callbackNumber     = server.arg("callbackNumber");
  String vAccount           = server.arg("vAccount");
  String groupId            = server.arg("groupId");
  String ForService         = server.arg("productForService");
  String modelNumber        = server.arg("modelNumber");
  String token              = String(ESP.getChipId());

  customerDetails += "{\"vAccount\": \""        + vAccount                      + "\",";
  customerDetails += "\"groupId\": \""          + groupId                       + "\",";
  customerDetails += "\"cAccount\": \""         + cAccount                      + "\",";
  customerDetails += "\"cName\": \""            + customerName                  + "\",";
  customerDetails += "\"callbackNumber\": \"+1" + callbackNumber                + "\",";
  customerDetails += "\"ForService\": \""       + ForService                    + "\",";
  customerDetails += "\"token\": \""            + token                         + "\",";
  customerDetails += "\"modelNumber\": \""      + modelNumber                   + "\"";
  customerDetails += "}";

  payload = AESencrypt(customerDetails);
  
  // Send HTTP Request to MachForce API to validate deviceId and valid inputs
  Serial.println("Authenticating Customer Details. Please Wait...");
  //Serial.println("Payload: " + payload);

  // Initiate TLS 1.2 Connection
  HTTPClient http;
  http.begin("http://machforce-api-multi.herokuapp.com/auth");
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(payload);
  
  if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP]POST... code: %d\n", httpCode);

            // Response from server
            if(httpCode == 200) {
                counter = 0;
                String response = http.getString();
                USE_SERIAL.println(response);
                http.end();
                USE_SERIAL.printf("Connection closed!");            
                // Write customer data to SPIFFS file
                File F = SPIFFS.open("/f.txt", "w");
                if(!F){
                  Serial.println("File not found! Cannot record customer details.");
                }
                Serial.println("====== Writing to SPIFFS file =========");
                F.print(payload);
                USE_SERIAL.printf("Customer Details Set To: ");
                Serial.println(payload);
                F.close();
                server.send(200, "text/html", resultPage);
                customerDetailsFlag = true;
                digitalWrite(relayPin, LOW);
            } else if (httpCode == 400){
                counter = 0;
                String response = http.getString();
                USE_SERIAL.println(response);
                http.end();
                USE_SERIAL.printf("Connection closed!");
                server.send(400, "text/html", authErrorPage);
            }
              else {
                counter = 0;
                String payload = http.getString();
                USE_SERIAL.println(payload);
                http.end();
                USE_SERIAL.printf("Connection closed!");
                counter = 0;
                server.send(500, "text/html", serverErrorPage);
              }
        } else {
            counter = 0;
            USE_SERIAL.printf("CONNECTION ERROR: Nothing was sent.");
        }
  }

  

  void runUpdater(){
      ESP.wdtDisable();
      //String hostHTTPS = "https://api.machforce.io/update/" + version;
      String hostUnsecured = "http://api.machforce.io/update/" + version;
      Serial.println("Running updater...");
      t_httpUpdate_return ret = ESPhttpUpdate.update(hostUnsecured);
      //t_httpUpdate_return  ret = ESPhttpUpdate.update(hostHTTPS,"","88:A1:3D:D9:F5:2A:3F:33:3B:59:91:B1:60:7C:E3:B6:7C:9A:EE:E3");

      Serial.print("UPDATER RETURNED: ");
      Serial.println(ret);

        switch(ret) {
            case HTTP_UPDATE_FAILED:
                USE_SERIAL.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
                break;

            case HTTP_UPDATE_NO_UPDATES:
                USE_SERIAL.println("HTTP_UPDATE_NO_UPDATES");
                break;

            case HTTP_UPDATE_OK:
                USE_SERIAL.println("HTTP_UPDATE_OK");
                break;
        }
}


String AESencrypt(String customerDetails){
    int stringLength  = customerDetails.length(); // Maximum Length for encoding 281 chars
    int b64Length     = stringLength * 8;
    int cipherLength  = b64Length * 3;
    char b64data[600];
    byte cipher[1200];
    byte iv[N_BLOCK];

    aes.set_key(key, sizeof(key));
    gen_iv(my_iv);

    // DEBUG ONLY - Print the IV
    Serial.println("Encryption activating...");
    Serial.println("Data Length: " + stringLength);
    
    base64_encode(b64data, (char *)my_iv,N_BLOCK);
    String IV = String(b64data);
    Serial.println("IV in Base 64: " + IV);
    Serial.println("Message To Encode: " + customerDetails);

    int b64len = base64_encode(b64data, (char *)customerDetails.c_str(),customerDetails.length());
    Serial.println("B64 Size: " + String(b64len));
    // Encryption Done Here
    aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, 128, my_iv);
    base64_encode(b64data, (char *)cipher, aes.get_size());
    String encryptedCustomerData = String(b64data);

    // DEBUG ONLY - Print the IV
    Serial.println("Encryption Done!");
    Serial.println("Cipher Size: " + String(aes.get_size()));
    Serial.println("Encrypted Data: " + encryptedCustomerData);

    String payload = "{ \"payload\": \"";
    payload += encryptedCustomerData + "\",";
    payload += "\"vector\": \" ";
    payload += IV + "\" }";

    aes.clean();

    return payload;
}

void gen_iv(byte *iv){
  for(int i=0; i < N_BLOCK; i++){
    iv[i]= (byte) getRnd();
  }
}

uint8_t getRnd(){
   uint8_t randomNumber = *(volatile uint8_t *) 0x3FF20E44;
   return randomNumber;
}


