#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3); // RX, TX
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // initialize software serial
  mySerial.begin(115200);
  Serial.println("Hello, testing.");
}

void loop() {
  // use software serial to read data from pins 2 and 3 as tx and rx respectively and write it to serial monitor
  if (mySerial.available()) {
    Serial.write(mySerial.read());
  }
}

//empty sketch
// void setup(){

// }

// void loop(){

// }