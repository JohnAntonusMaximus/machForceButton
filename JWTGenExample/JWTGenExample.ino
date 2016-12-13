#include "sha256.h"
#include "Base64.h"

char header[]= "{\"alg\": \"HS256\",\"typ\": \"JWT\"}";
char payload[] = "{\"smsMessage\":\"/ ALERT: Service Requested / Customer Name: Broward Health Center / Account #: 456788879456755-54 / Callback #: 617-657-9981 / Product: IQon Cat Scan\",\"vAccount\":\"43234543234543\",\"cAccount\":\"456788879456755-54\",\"cName\":\"Broward Health Center\",\"deviceId\":\"FORCE-2355\",\"callback\":\"+16176579981\",\"ForService\":\"IQon Cat Scan\",\"modelNumber\":\"RX93QT\"}";
uint8_t* hash;


void setup() {
Serial.begin(115200);

int headerSize =  sizeof(header);
int payloadSize = sizeof(payload);

Serial.println("JWT Generator");

Serial.println("Loading...");
Serial.println(".....");
Serial.println(".......");


int encodedSizeHeader = base64_enc_len(headerSize);
int encodedSizePayload = base64_enc_len(payloadSize);


char encodedHeader[encodedSizeHeader];
char encodedPayload[encodedSizePayload];

base64_encode(encodedHeader, header, headerSize);
base64_encode(encodedPayload, payload, payloadSize);

Serial.print("Header = ");
Serial.println(encodedHeader);
String encodedHeaderString = encodedHeader;
String strippedHeader = encodedHeaderString;
strippedHeader.replace("=","");

Serial.print("Payload = ");
Serial.println(encodedPayload);
String encodedPayloadString = encodedPayload;
String strippedPayload = encodedPayloadString;
strippedPayload.replace("=","");
//Serial.println(strippedPayload);

String payload = strippedHeader + "." + strippedPayload;
Serial.println(payload);

Sha256.initHmac((const uint8_t*)"helloDolly", 10);
Sha256.print(payload);
hash = Sha256.resultHmac();
Serial.print("Hash = ");
print_hmac(hash);


int hashSize = 32;
int encodedSizeHash = base64_enc_len(hashSize);
char encodedHash[encodedSizeHash];
char* hashPointer = (char*) hash;
base64_encode(encodedHash, hashPointer, hashSize);

Serial.print("Encoded Hash = ");
Serial.println(encodedHash);
String strippedHash = encodedHash;
strippedHash.replace("=","");

String jwt = payload + "." + strippedHash;
Serial.print("JWT = ");
Serial.println(jwt);

}

void loop() {

}
void print_hmac(uint8_t *hmac){
  for(int i=0; i<32; i++)
  Serial.print(hmac[i],HEX);
}


