#include <aws_iot_mqtt.h>
#include <aws_iot_version.h>
#include "aws_iot_config.h"
#include <avr/sleep.h>

//===============================================================
#define AWS_IOT_MQTT_HOST "afo457vzyno4v.iot.us-east-1.amazonaws.com"
// your endpoint
#define AWS_IOT_MQTT_PORT 8883
// your port
#define AWS_IOT_CLIENT_ID      "AWSIoT"
// your client ID
#define AWS_IOT_MY_THING_NAME "AWSIoT"
// your thing name
#define AWS_IOT_ROOT_CA_FILENAME "root-CA.crt"
// your root-CA filename
#define AWS_IOT_CERTIFICATE_FILENAME "b9bb4d0e5f-certificate.pem.crt"
// your certificate filename
#define AWS_IOT_PRIVATE_KEY_FILENAME "b9bb4d0e5f-private.pem.key"
// your private key filename
//===============================================================

int led = 4;
int wakePin = 3;
int sleepStatus = 0;
int count = 0;

void wakeUpNow(){
   // execute AWS communincations code here after wake-up before returning to the loop() function
   // timers and code using timers (serial.print and more...) will not work here.
   // we don't really need to execute any special functions here, since we
   // just want the thing to wake up
}

aws_iot_mqtt_client myClient; // init iot_mqtt_client
char msg[32]; // read-write buffer
int cnt = 0; // loop counts
int rc = -100; // return value placeholder
bool success_connect = false; // whether it is connected

// Basic callback function that prints out the message
void msg_callback(char* src, unsigned int len, Message_status_t flag) {
  if(flag == STATUS_NORMAL) {
    Serial.println("CALLBACK:");
    int i;
    for(i = 0; i < (int)(len); i++) {
      Serial.print(src[i]);
    }
    Serial.println("");
  }
}


void setup() {
  pinMode(wakePin,INPUT);
  pinMode(led,OUTPUT);
  Serial.begin(9600);

  attachInterrupt(0,wakeUpNow, LOW); // Using interrupt 0 (pin 2) and run function wakeUpNow() when pin 2 goes LOW

}


// Bed-time for Arduino
void sleepNow(){     
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

  sleep_enable();
  attachInterrupt(0,wakeUpNow,LOW);
  sleep_mode();
  sleep_disable();
  detachInterrupt(0); //disables interrupt 0 on pin 2 so the wakeUpNow code will not be executed during normal running time.
} 

void loop() {
  //display information about the counter
  Serial.print("Awake for ");
  Serial.print(count);
  Serial.print("sec");
  count++;
  delay(1000); // wait one second

  if(Serial.available()){
    int val = Serial.read();
    if(val == 'S'){
      Serial.println("Serial: Entering sleep mode...");
      delay(100) // Needed delay or the sleep function will provoke a Serial Error
      count = 0;
      sleepNow();
      }
    if(val == 'A'){
      Serial.println("Good morning! Time to rise and shine!");
      }
  }

  //check if it should go to sleep because of time
  if(count >=10){
    Serial.println("Timer: Entering sleep mode...");
    delay(100); // Needed delay or the sleep function will provoke a Serial Error
    count = 0;
    sleepNow();
  }

}
