#include <SPI.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

char* ssid     = "AST_Network";
char* password = "cavadosta";
const char* host = "http://machforce-api.herokuapp.com/post";


// The number of times to trigger the action if the condition is met
// We limit this so you won't use all of your Temboo calls while testing
int maxCalls = 10;

// The number of times this Choreo has been run so far in this sketch
int calls = 0;

int inputPin = 0;

void setup() {
  Serial.begin(115200); 
  
  // For debugging, wait until the serial console is connected
   delay(10);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
 

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // Initialize pins
  pinMode(inputPin, INPUT);
  digitalWrite(inputPin, LOW);

  Serial.println("Setup complete.\n");

}

void runPublish() {

  String jsonObj = "{\"smsMessage\":\"/ ALERT: Service Requested / Customer Name: Broward Health Center / Account #: 456788879456755-54 / Callback #: 617-657-9981 / Product: IQon Cat Scan\",\"vAccount\":\"43234543234543\",\"cAccount\":\"456788879456755-54\",\"cName\":\"Broward Health Center\",\"deviceId\":\"FORCE-2355\",\"callback\":\"+16176579981\",\"ForService\":\"IQon Cat Scan\",\"modelNumber\":\"RX93QT\"}";

  int len = jsonObj.length();
  String lenString = String(len);
  HTTPClient http;

  http.begin(host);
  http.addHeader("POST", "/post HTTP/1.1");
  http.addHeader("Host", "machforce-api.herokuapp.com");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Content-Length",lenString);
  http.addHeader("Cache-Control","no-cache");
  http.POST(jsonObj);
  http.writeToStream(&Serial);
  http.end();


  
 
  Serial.println("CHECK: OK-3");

}

void loop() {

  int sensorValue = digitalRead(inputPin);
  Serial.println("Sensor: " + String(sensorValue));

  if (sensorValue == LOW) {
    if (calls < maxCalls) {
      Serial.println("\nTriggered! Calling Machforce API...");
      runPublish();
      calls++;
    } else {
      Serial.println("\nTriggered! Skipping. Adjust maxCalls as required.");
    }
  }
  delay(250);
}
