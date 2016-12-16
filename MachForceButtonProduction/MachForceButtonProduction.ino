#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>  
#include <WiFiManager.h>  
#include "FS.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>

/////////////////////////
///Firmware Version/////
/////////////////////////

const String version = "1.0";

/////////////////////////
///Hardware Definitions//
/////////////////////////

#define USE_SERIAL Serial

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
MDNSResponder mdns;
ESP8266WebServer server(80);
int TRIGGER_PIN = 15;

boolean customerDetailsFlag;
String  customerDetails;

String customerDetailPage = "";
String resultPage = "";
  

void setup(void) {
   
    Serial.begin(115200);
    Serial.print("Welcome To MachForce! Version Number: ");
    Serial.println(version);
    
    ///// Initiate Hardware ///////
    initiateHardware();

    ///// Initiate WiFi ///////
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setMinimumSignalQuality(10);
    wifiManager.setConfigPortalTimeout(60);
    wifiManager.autoConnect("MachForce", "1234567");
    // ^BLOCKS UNTIL TRUE --- Will automatically go into WiFi Manager if false //
    checkResetButton();
    
    //if you get here you have connected to the WiFi
    Serial.println("Successfully Connected!");

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
      customerDetails = f.readStringUntil('\n');
      Serial.println("Customer Details Exist, Recorded As:");
      Serial.println(customerDetails);
      f.close();
    }

    runUpdater();
    
}

void loop() {
      Serial.println("Going into sleep mode now...");
      ESP.deepSleep(0, WAKE_RF_DEFAULT); // if using relay as a switching device, change this to yield(); 
      delay(100);
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

    if(mdns.begin("esp8266", WiFi.localIP())){
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
    Serial.println(WiFi.localIP());

    while(customerDetailsFlag == false){
      server.handleClient();
    }
}

void checkResetButton(){
          // Read the state of the button
        buttonVal = digitalRead(TRIGGER_PIN); 
        
        // Test for button held down for longer than the hold time
        if (buttonVal == HIGH){
          factoryReset();
        }
}

void initiateHardware(){
    Serial.print("Hardware Inititiating..............");
    digitalWrite(TRIGGER_PIN, LOW);
    pinMode(TRIGGER_PIN, INPUT);
    Serial.println("Hardware Inititiated!");
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

void shortPress(){
    Serial.println("Service Requested! Sending...");
    Serial.println(customerDetails);
//  int len = customerDetails.length();
//  String lenString = String(len);
//  HTTPClient http;
//
//  http.begin(host);
//  http.addHeader("POST", "/post HTTP/1.1");
//  http.addHeader("Host", "machforce-api.herokuapp.com");
//  http.addHeader("Content-Type", "application/json");
//  http.addHeader("Content-Length",lenString);
//  http.addHeader("Cache-Control","no-cache");
//  http.POST(customerDetails);
//  http.writeToStream(&Serial);
//  http.end();
//
//  Serial.println("CHECK: OK-3");

}

void factoryReset(){
  // RESETS MACHFORCE DEVICE -> Clears WiFi settings and empties SPIFF file, starts from begginning into STAGE 1
   // Empty SPIFF Files here
  Serial.print("Restoring to factory settings, please wait...");
  SPIFFS.format();
  wifiManager.resetSettings();
  Serial.println("Done! Restarting now.");
  ESP.restart();
}

void handle_form() {                                       
  String customerName       = server.arg("customerName");
  String cAccount           = server.arg("cAccount");
  String callbackNumber     = server.arg("callbackNumber");
  String vAccount           = server.arg("vAccount");
  String groupId            = server.arg("groupId");
  String productForService  = server.arg("productForService");
  String modelNumber        = server.arg("modelNumber");
  String deviceId           = String(ESP.getChipId());

  customerDetails += "{\"smsMessage\":\"/ ALERT: Service Requested / ";
  customerDetails += "Customer Name: "        + customerName        + " / ";
  customerDetails += "Account #: "            + cAccount            + " / ";
  customerDetails += "Callback #: "           + callbackNumber      + " / ";
  customerDetails += "Product: "              + productForService   + "\",";
  customerDetails += "\"vAccount\": \""       + vAccount            + "\",";
  customerDetails += "\"groupId\": \""        + groupId             + "\",";
  customerDetails += "\"cAccount\": \""       + cAccount            + "\",";
  customerDetails += "\"cName\": \""          + customerName        + "\",";
  customerDetails += "\"deviceId\": \""       + deviceId            + "\",";
  customerDetails += "\"callback\": \""       + callbackNumber      + "\",";
  customerDetails += "\"ForService\": \""     + productForService   + "\",";
  customerDetails += "\"modelNumber\": \""    + modelNumber         + "\"}";
  
  
  Serial.println(customerName);
  Serial.println(cAccount);
  Serial.println(callbackNumber);
  Serial.println(vAccount);
  Serial.println(groupId);
  Serial.println(productForService);
  Serial.println(modelNumber);

  // Send HTTP Request to MachForce API to validate deviceId and valid inputs
                          
  // If deviceId checks out and valid input
  File F = SPIFFS.open("/f.txt", "w");
  if(!F){
    Serial.println("File not found! Cannot record customer details.");
  }
  Serial.println("====== Writing to SPIFFS file =========");
  F.print(customerDetails);
  Serial.println(customerDetails);
  F.close();
  
  server.send(200, "text/html", resultPage);

  customerDetailsFlag = true;

  }

  void runUpdater(){
      Serial.println("Running updater...");
      t_httpUpdate_return ret = ESPhttpUpdate.update("http://machforce-api-multi.herokuapp.com/update/" + version);
        //t_httpUpdate_return  ret = ESPhttpUpdate.update("https://server/file.bin");

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

