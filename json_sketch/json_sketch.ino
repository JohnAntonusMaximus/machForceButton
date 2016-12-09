#include <Bridge.h>
#include <Temboo.h>
#include "TembooAccount.h" // contains Temboo account information, as described below

int numRuns = 1;   // Execution count, so this doesn't run forever
int maxRuns = 10;   // Maximum number of times the Choreo should be executed

void setup() {
  Serial.begin(9600);
  
  // For debugging, wait until the serial console is connected
  delay(4000);
  while(!Serial);
  Bridge.begin();
}

void loop() {
  if (numRuns <= maxRuns) {
    Serial.println("Running Publish - Run #" + String(numRuns++));
    
    TembooChoreo PublishChoreo;

    // Invoke the Temboo client
    PublishChoreo.begin();

    // Set Temboo account credentials
    PublishChoreo.setAccountName(TEMBOO_ACCOUNT);
    PublishChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
    PublishChoreo.setAppKey(TEMBOO_APP_KEY);
    
    // Set Choreo inputs
    PublishChoreo.addInput("Message", "{ \"vendorAccount\": \"43234543234543\", \"customerAccount: \"323928349223-2\", \"callback: \"954-770-3598\", \"productForService: \"Comcast Cable Box\", \"modelNumber: \"DHG-YDUS732\" }");
    PublishChoreo.addInput("TopicArn", "arn:aws:sns:us-east-1:539520134050:PrototypeTemboo");
    PublishChoreo.addInput("UserRegion", "us-east-1");
    PublishChoreo.addInput("MessageStructure", "json");
    PublishChoreo.addInput("AWSAccessKeyId", "AKIAJQPXC274NVBY732A");
    PublishChoreo.addInput("AWSSecretKeyId", "IzHQ9ncUKi/NjZnQ61cil8wfky1Fv1A7we8y4bDS");
    
    // Identify the Choreo to run
    PublishChoreo.setChoreo("/Library/Amazon/SNS/Publish");
    
    // Run the Choreo; when results are available, print them to serial
    PublishChoreo.run();
    
    while(PublishChoreo.available()) {
      char c = PublishChoreo.read();
      Serial.print(c);
    }
    PublishChoreo.close();
  }

  Serial.println("Waiting...");
  delay(30000); // wait 30 seconds between Publish calls
}
