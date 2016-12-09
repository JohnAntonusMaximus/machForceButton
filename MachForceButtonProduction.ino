#include <ESP8266WiFi.h>
#include "FS.h"
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/////////////////////////
///Hardware Definitions//
/////////////////////////

const int button = 12;
const int ledRed = 13;
const int ledGreen = 14;
const int ledYellow = 15;
const int ledWhite = 11;

/////////////////////////
///Stage 1 Definitions///
/////////////////////////

char* ssid;
char* password;

const char WiFiAPPSK[] = "MachForce";
WiFiServer server(80);
bool result = false;
bool start;

String accessPointSetup = "";

/////////////////////////
///Stage 2 Definitions///
/////////////////////////

String customerDetailPage = "";
String resultPage = "";
  
/////////////////////////
///Stage 3 Definitions///
/////////////////////////

/////////////////////////
///Stage 4 Definitions///
/////////////////////////

void setup() {

    ///// Populate HTML For Client ///////
  
  
    ///// Initiate Hardware ///////

    initiateHardware(); 

    ///// STAGE 1 - SPIFF Format & Access Point ///////
    
    StageOne();
    
    ///// Stage 2 Setup ///////
    
    ///// Stage 3 Setup ///////
    
    ///// Stage 4 Setup ///////

}

void loop() {
    
    ///// Stage 1 - Access Point Setup ///////
    if(!ssid && !password){
      WiFiClient client = server.available();
      if(!client){
        return;
      }
      // Send Access Point Setup Page
      client.print(accessPointSetup);
    }

  

}

void StageOne(){
    SPIFFS.begin();
    
    //Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    //digitalWrite(ledWhite,HIGH);
    //result = SPIFFS.format();
    //if(result == true){
      //digitalWrite(ledWhite, LOW);
     // digitalWrite(ledGreen, HIGH);
     // delay(200);
     // digitalWrite(ledGreen, HIGH);
     // delay(200);
     // digitalWrite(ledGreen, HIGH);
    //}
  
    accessPointSetup += "<html>"; 
    accessPointSetup += "<head>";
    accessPointSetup += "<!-- Latest compiled and minified CSS -->";
    accessPointSetup += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\">";
    accessPointSetup += "<!-- Optional theme -->";
    accessPointSetup += "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\">";
    accessPointSetup += "<!-- Latest compiled and minified JavaScript -->";
    accessPointSetup += "<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script>";
    accessPointSetup += "</head>";
    accessPointSetup += "<body>";
    accessPointSetup += "<div class=\"container\" id=\"container\" style=\"margin-left: 30px; line-height: 10px; position: relative; \">";
    accessPointSetup += "<div class=\"row\">";
    accessPointSetup += "<div class=\"cols-xs-12 col-md-9 pull-md-3 bd-content\" style=\"padding-bottom: 15px;\">";
    accessPointSetup += "<h1 style=\"font-family:\'Oswald\', sans-serif; padding-bottom 20px;\">Welcome To MachForce <span style=\"font-weight: bold; color: dodgerblue;\">Access Point</span> Setup</h1>";
    accessPointSetup += "</div>";
    accessPointSetup += "<br/><br/>";
    accessPointSetup += "<div class=\"cols-xs-12 col-md-9 pull-md-3 bd-content\">";
    accessPointSetup += "<p>Please enter access point SSID and password.</p>";
    accessPointSetup += "</div>";
    accessPointSetup += "<br/><br/>";
    accessPointSetup += "<div class=\"panel panel-default\" style=\"width: 80%; margin-top: 5%;\">";
    accessPointSetup += "<div class=\"panel-body\">";
    accessPointSetup += "<form method=\"post\" action=\"accessPointCredentials\">"; 
    accessPointSetup += "<div class=\"form-group row\">";
    accessPointSetup += "<label for=\"SSID\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">SSID </label>";
    accessPointSetup += "<div class=\"col-xs-4\">";
    accessPointSetup += "<input class=\"form-control\" type=\"text\" id=\"ssid\" name=\"ssid\">";
    accessPointSetup += "</div>";
    accessPointSetup += "</div>";
    accessPointSetup += "<div class=\"form-group row\">";
    accessPointSetup += "<label for=\"password\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Password </label>";
    accessPointSetup += "<div class=\"col-xs-4\">";
    accessPointSetup += "<input class=\"form-control\" type=\"password\" id=\"password\" name=\"password\">";
    accessPointSetup += "</div>";
    accessPointSetup += "</div>";
    accessPointSetup += "<button type=\"submit\" class=\"btn btn-primary\">Submit</button>";
    accessPointSetup += "</form>";
    accessPointSetup += "</div>"; // <--- panel-body
    accessPointSetup += "</div>"; // <--- panel panel-default
    accessPointSetup += "</div>"; // <--- row
    accessPointSetup += "</div>"; // <--- container
    accessPointSetup += "</body>"; // <--- body
    accessPointSetup += "</html>"; // <--- html
      
    server.begin();
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
    pinMode(button, INPUT);
    pinMode(ledRed, OUTPUT);
    pinMode(ledGreen, OUTPUT);
    pinMode(ledYellow, OUTPUT);
    pinMode(ledWhite, OUTPUT);
    setupWiFiAccessPoint();
}

void setupWiFiAccessPoint(){
  WiFi.mode(WIFI_AP);

  // Do a little work to get a unique-ish name. Append the
  // last two bytes of the MAC (HEX'd) to "Thing-":
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "MachForce Device " + macID;

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}

