#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

char* ssid  = "AST_Network";
char* password = "cavadosta";

ESP8266WebServer server(80);

String homePage = "";
String resultPage = "";

void setup(void) {
  
  homePage += "<html><head><!-- Latest compiled and minified CSS --><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\"><!-- Optional theme --><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\"><!-- Latest compiled and minified JavaScript --><script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script></head><body><div class=\"container\" id=\"container\" style=\"margin-left: 30px; line-height: 10px; position: relative; \"><div class=\"row\"><div class=\"cols-xs-12 col-md-9 pull-md-3 bd-content\" style=\"padding-bottom: 15px;\"><h1 style=\"font-family:\'Oswald\', sans-serif; padding-bottom 20px;\">Welcome To MachForce <span style=\"font-weight: bold; color: dodgerblue;\">Device</span> Setup</h1></div><br/><br/><div class=\"cols-xs-12 col-md-9 pull-md-3 bd-content\"><p>Please enter customer information for button below.</p></div><br/><br/><div class=\"panel panel-default\" style=\"width: 80%; margin-top: 5%;\"><div class=\"panel-body\"><form method=\"post\" action=\"customerDetails\"><div class=\"form-group row\"><label for=\"customerName\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Customer Name </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"customerName\" name=\"customerName\"></div></div><div class=\"form-group row\"><label for=\"cAccount\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Customer Account # </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"cAccount\" name=\"cAccount\"></div></div><div class=\"form-group row\"><label for=\"callbackNumber\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Callback Number (Cell) </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"callbackNumber\" name=\"callbackNumber\"></div></div><div class=\"form-group row\"><label for=\"vAccount\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Vendor Account # </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"vAccount\" name=\"vAccount\"></div></div><div class=\"form-group row\"><label for=\"groupId\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Group ID </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"groupId\" name=\"groupId\"></div></div><div class=\"form-group row\"><label for=\"productForService\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px;\">Product<br/>Name </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"productForService\" name=\"productForService\"></div></div><div class=\"form-group row\"><label for=\"modelNumber\" class=\"col-xs-4 col-form-label\" style=\"line-height: 15px; width: 22%; padding-top: 10px; padding-top: 10px;\">Model # </label><div class=\"col-xs-4\"><input class=\"form-control\" type=\"text\" id=\"modelNumber\" name=\"modelNumber\"></div></div><button type=\"submit\" class=\"btn btn-primary\">Submit</button></form></div></div></div></div></body></html>";
  resultPage +="<html><head><!-- Latest compiled and minified CSS --><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\" integrity=\"sha384-BVYiiSIFeK1dGmJRAkycuHAHRg32OmUcww7on3RYdg4Va+PmSTsz/K68vbdEjh4u\" crossorigin=\"anonymous\"><!-- Optional theme --><link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap-theme.min.css\" integrity=\"sha384-rHyoN1iRsVXV4nD0JutlnGaslCJuC7uwjduW9SVrLvRYooPp2bWYgmgJQIXwl/Sp\" crossorigin=\"anonymous\"><!-- Latest compiled and minified JavaScript --><script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js\" integrity=\"sha384-Tc5IQib027qvyjSMfHjOMaLkfuWVxZxUPnCJA7l2mCWNIpG9mGCD8wGNIcPD7Txa\" crossorigin=\"anonymous\"></script></head><body><div class=\"container\"><div class=\"\" style=\"padding-top: 50px;\"><div class=\"row\"><h1 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Thank You</h1><br/><h2 style=\"font-family:\'Oswald\', sans-serif; text-align: center;\">Customer <span style=\"font-weight: bold; color: dodgerblue;\">Device</span> Has Now Been Configured</h2></div><div style=\"text-align: center;\"><br/><br/><img src=\"http://res.cloudinary.com/ha3eifngn/image/upload/v1480980127/MachForce_Logo_fvngw9.png\" height=\"75px\"></div></div></div></body></html>";
  
  delay(1000);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  if(mdns.begin("esp8266", WiFi.localIP())){
    Serial.println("MDNS responder started");
  }

  server.on("/", [](){
    server.send(200,"text/html", homePage);
  });

  server.on("/customerDetails", handle_form);

  server.on("/setupResult", [](){
    server.send(200,"text/html", resultPage);
  });

  server.begin();
  Serial.println("HTTP server started");

}

void loop(void) {
  server.handleClient();
}

void handle_form() {                                       
  String customerName = server.arg("customerName");
  String cAccount = server.arg("cAccount");
  String callbackNumber = server.arg("callbackNumber");
  String vAccount = server.arg("vAccount");
  String groupId = server.arg("groupId");
  String productForService = server.arg("productForService");
  String modelNumber = server.arg("modelNumber");
  
  Serial.println(customerName);
  Serial.println(cAccount);
  Serial.println(callbackNumber);
  Serial.println(vAccount);
  Serial.println(groupId);
  Serial.println(productForService);
  Serial.println(modelNumber);
                          

  server.send(200, "text/html", resultPage);

  }
