#include <Arduino.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>

//setup serial communication between arduino and esp8266 in pins 2 and 3
SoftwareSerial esp8266(2, 3);
#define USE_ARDUINO_INTERRUPTS true //set to true if you are using an Arduino Uno, Leonardo, or Micro
#define DHTTYPE DHT12
#define DHTPIN 5
#include <PulseSensorPlayground.h>
#include <Ultrasonic.h>

PulseSensorPlayground pulseSensor; //create a new instance of the PulseSensorPlayground object
const int pulseLED = 7;
int pulseSensorThreshold = 550;
int pulseBPM= 0;

Ultrasonic ultrasonic1(9, 10);
int distance = 0;
int heartrate = 0;

void setup() {
  pinMode(A1, INPUT);
  Serial.begin(115200);
  esp8266.begin(115200);
  pulseSensor.analogInput(0); //set the analog input pin for the PulseSensor
  pulseSensor.blinkOnPulse(pulseLED); //set the LED pin for the PulseSensor
  pulseSensor.setThreshold(pulseSensorThreshold); //set the threshold for the PulseSensor
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor found!");
  }
  else {
    Serial.println("No PulseSensor found!");
  }
}

void loop() {
  //read ultrasound sensor
  distance = ultrasonic1.read();
  
  //read heartrate sensor
  if (pulseSensor.sawStartOfBeat()) {
    pulseBPM = pulseSensor.getBeatsPerMinute();
    if (pulseBPM < 40 || pulseBPM > 180) {
      pulseBPM = -2;
    }
    heartrate = pulseBPM;
  }
  
  //send sensor values to wifi module
  StaticJsonDocument<64> doc;
  doc["distance"] = distance;
  doc["heartrate"] = heartrate;
  char buffer[64];
  serializeJson(doc, buffer);
  esp8266.println(buffer);
  
  //error handling for serial communication
  if (esp8266.available() > 0) {
    String response = esp8266.readStringUntil('\n');
    Serial.println(response);
  }
}

// Null Sketch
// void setup(){

// }

// void loop(){

// }