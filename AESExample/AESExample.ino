#include "AES.h"
#include "base64.h"

AES aes;

byte key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

byte my_iv[N_BLOCK] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

String msg = "{\"smsMessage\": \"ALERT / Customer: Broward Health / Callback #: 954-729-3345 / Account #: 4546756-01 / Product: IQon Cat Scan \", \"vAccount\": \"MF-ghjIjdkadj\" }";

void setup(){
  Serial.begin(115200);
  testAES128();
}

void loop(){
  
}

void testAES128(){
  char b64data[400];
  byte cipher[1000];
  byte iv [N_BLOCK];

  Serial.println('Encryption activating...');

  aes.set_key( key, sizeof(key));
  gen_iv(my_iv);

  // Print the IV
  base64_encode(b64data, (char *)my_iv, N_BLOCK);
  Serial.println("IV b64: " + String(b64data));

  Serial.println("Message To Encode: " + msg);

  int b64len = base64_encode(b64data, (char *)msg.c_str(),msg.length());

  //Encryption done here
  aes.do_aes_encrypt((byte *)b64data, b64len, cipher, key, 128, my_iv);

  Serial.println("Encryption Done!");
  Serial.println("Cipher Size: " + String(aes.get_size()));

  base64_encode(b64data, (char *)cipher, aes.get_size());
  Serial.println("Encrypted Data: " + String(b64data) );

  aes.clean();

  Serial.println("------End of Program------");
}

void gen_iv(byte *iv){
  for(int i=0; i< N_BLOCK; i++){
    iv[i] = (byte) getrnd();
  }
}

uint8_t getrnd(){
  uint8_t really_random = *(volatile uint8_t *) 0x3FF20E44;
  return really_random;
}
