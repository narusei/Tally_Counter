/*
  Lora Send And Receive
  This sketch demonstrates how to send and receive data with the MKR WAN 1300/1310 LoRa module.
  This example code is in the public domain.
*/

#include <MKRWAN_v2.h>

LoRaModem modem;
const int buttonPin = 2;

// Uncomment if using the Murata chip as a module
// LoRaModem modem(Serial1);

#include "arduino_secrets.h"
// Please enter your sensitive data in the Secret tab or arduino_secrets.h
String appEui = SECRET_APP_EUI;
String appKey = SECRET_APP_KEY;
int buttonState = 0;

String tallyCounterName = "iwailab";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);
  // change this to your regional band (eg. US915, AS923, ...)
  if (!modem.begin(AS923)) {
    Serial.println("Failed to start module");
    while (1) {}
  };
  Serial.print("Your module version is: ");
  Serial.println(modem.version());
  Serial.print("Your device EUI is: ");
  Serial.println(modem.deviceEUI());

  int connected = modem.joinOTAA(appEui, appKey);
  if (!connected) {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    while (1) {}
  } else {
    Serial.println("Connect Success!");
  }

  // Set poll interval to 60 secs.
  modem.minPollInterval(60);
  // NOTE: independent of this setting, the modem will
  // not allow sending more than one message every 2 minutes,
  // this is enforced by firmware and can not be changed.
  pinMode(buttonPin, INPUT);
  
  Serial.println();
  Serial.print("Tally Counter Name is : ");
  Serial.print(tallyCounterName);
  Serial.print(" - ");
  for (unsigned int i = 0; i < tallyCounterName.length(); i++) {
    Serial.print(tallyCounterName[i] >> 4, HEX);
    Serial.print(tallyCounterName[i] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void loop() {

  if (isPushedTallyCounter()) {
    int err;
    modem.setPort(1);
    modem.beginPacket();
    modem.print(tallyCounterName);
    err = modem.endPacket(false);
    if (err > 0) {
      Serial.println("Message sent correctly!");
    } else {
      Serial.println("Error sending message :(");
    }
  } else {
    Serial.println("No Pushed Tally Counter");
  }

  if (!modem.available()) {
    Serial.println("No downlink message received at this time.");
    modem.poll();
    delay(200);
    return;
  }
  
  char rcv[64];
  int i = 0;
  String msg = "";
  while (modem.available()) {
    rcv[i++] = (char)modem.read();
  }
  Serial.println();
  Serial.print("Received: ");
  for (unsigned int j = 0; j < i; j++) {
    msg.concat(rcv[j]);
    Serial.print(rcv[j] >> 4, HEX);
    Serial.print(rcv[j] & 0xF, HEX);
    Serial.print(" ");
  }
  Serial.print(" - ");
  Serial.print(msg);
  Serial.println();

  tallyCounterName = msg;
  Serial.print("Change Tally Counter Name : ");
  Serial.print(tallyCounterName);
  Serial.println();
  modem.poll();
  delay(1000);
}

bool isPushedTallyCounter() {
  buttonState = digitalRead(buttonPin);

  if (buttonState == HIGH) {
    return true;
  } else {
    return false;
  }
}
