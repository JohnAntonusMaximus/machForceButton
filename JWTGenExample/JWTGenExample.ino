#include "sha256.h"
#include "Base64.h"
#include <ArduinoJson.h>


//char header[]= "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
//char payload[] = "{\"smsMessage\":\"/ ALERT: Service Requested / Customer Name: Broward Health Center / Account #: 456788879456755-54 / Callback #: 617-657-9981 / Product: IQon Cat Scan\",\"vAccount\":\"43234543234543\",\"cAccount\":\"456788879456755-54\",\"cName\":\"Broward Health Center\",\"deviceId\":\"FORCE-2355\",\"callback\":\"+16176579981\",\"ForService\":\"IQon Cat Scan\",\"modelNumber\":\"RX93QT\"}";
uint8_t* hash;




void setup() {
Serial.begin(115200);
while(!Serial){
  // Wait for serial port initialization
}

DynamicJsonBuffer  jsonBuffer;
DynamicJsonBuffer  jsonBufferTwo;

// Header
JsonObject &rootHeader = jsonBufferTwo.createObject();
rootHeader["alg"] = "HS256";
rootHeader["typ"] = "JWT";

// Payload
JsonObject &rootPayload = jsonBuffer.createObject();
rootPayload["smsMessage"] = "ALERT: Service Requested | Customer Name: Broward Health Center | Account #: 456788879456755-54 | Callback #: 617-657-9981 | Product: IQon Cat Scan";
rootPayload["vAccount"] = "MF-564gh75647fgHj";
rootPayload["cAccount"] = "456788879456755-54";
rootPayload["groupId"] = "1bcdefYuh";
rootPayload["cName"] = "Broward Health Center";
rootPayload["deviceId"] = "453664563582";
rootPayload["callback"] = "+16176579981";
rootPayload["ForService"] = "IQon Cat Scan";
rootPayload["modelNumber"] = "RX93QT";

int sizeOfHeader = sizeof(rootHeader);
int sizeOfPayload = sizeof(rootPayload);
char header[]= "{\"alg\":\"HS256\",\"typ\":\"JWT\"}";
char payload[] = "{\"smsMessage\":\"/ ALERT: Service Requested / Customer Name: Broward Health Center / Account #: 456788879456755-54 / Callback #: 617-657-9981 / Product: IQon Cat Scan\",\"vAccount\":\"43234543234543\",\"cAccount\":\"456788879456755-54\",\"cName\":\"Broward Health Center\",\"deviceId\":\"FORCE-2355\",\"callback\":\"+16176579981\",\"ForService\":\"IQon Cat Scan\",\"modelNumber\":\"RX93QT\"}";
//rootHeader.printTo(header, sizeof(header));
//rootPayload.printTo(payload, 500);

int headerSize =  sizeof(header);
int payloadSize = sizeof(payload);
Serial.println(header);
Serial.println(header);
Serial.println(header);
Serial.println(header);
Serial.println(payload);
Serial.println(payload);
Serial.println(payload);
Serial.println(payload);

Serial.println("JWT Generator");

int encodedSizeHeader = base64_enc_len(headerSize);
int encodedSizePayload = base64_enc_len(strlen(payload));

Serial.print("Header Size: ");
Serial.println(headerSize);
Serial.print("PayloadSize: ");
Serial.println(payloadSize);
Serial.print("EncodedHeaderSize: ");
Serial.println(encodedSizeHeader);
Serial.print("EncodedPayloadSize: ");
Serial.println(encodedSizePayload);
//
char encodedHeader[encodedSizeHeader];
char encodedPayload[encodedSizePayload];
//
base64_encode(encodedHeader, header, headerSize);
base64_encode(encodedPayload, payload, payloadSize);
//
Serial.print("Header = ");
Serial.println(encodedHeader);
Serial.println();
String encodedHeaderString = encodedHeader;
String strippedHeader = encodedHeaderString;
strippedHeader.replace("=","");
Serial.print("Stripped Header = ");
Serial.println(strippedHeader);
Serial.println();

//Serial.print("Payload = ");
//Serial.println(encodedPayload);
//String encodedPayloadString = encodedPayload;
//String strippedPayload = encodedPayloadString;
//strippedPayload.replace("=","");
//Serial.print("Stripped Payload = ");
//Serial.println(strippedPayload);
//Serial.println();
//
//String finalPayload = strippedHeader + "." + strippedPayload;
//Serial.print("Final Payload = ");
//Serial.println(finalPayload);
//
//Sha256.initHmac((const uint8_t*)"helloDolly", 10);
//Sha256.print(finalPayload);
//hash = Sha256.resultHmac();
//Serial.print("Hash = ");
//print_hmac(hash);
//Serial.println();
//
//
//int hashSize = 32;
//int encodedSizeHash = base64_enc_len(hashSize);
//char encodedHash[encodedSizeHash];
//char* hashPointer = (char*) hash;
//base64_encode(encodedHash, hashPointer, hashSize);
//
//Serial.print("Encoded Hash = ");
//Serial.println(encodedHash);
//String strippedHash = encodedHash;
//strippedHash.replace("=","");
//
//String jwt = finalPayload + "." + strippedHash;
//Serial.print("JWT = ");
//Serial.println(jwt);

}

void loop() {

}
void print_hmac(uint8_t *hmac){
  for(int i=0; i<32; i++)
  Serial.print(hmac[i],HEX);
}


