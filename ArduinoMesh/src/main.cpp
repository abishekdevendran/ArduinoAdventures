#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <SoftwareSerial.h>
#define USE_ARDUINO_INTERRUPTS false //set to true if you are using an Arduino Uno, Leonardo, or Micro
#include <PulseSensorPlayground.h>
#include <Ultrasonic.h>
#include <DHT.h>
#include <ArduinoJson.h>

//setup serial communication between arduino and esp8266 in pins 2 and 3
SoftwareSerial esp8266(2, 3);
#define DHTTYPE DHT11
#define DHTPIN 5
#define pulseLED 7
#define pulseSensorThreshold 520
#define distanceThreshold 10

PulseSensorPlayground pulseSensor; //create a new instance of the PulseSensorPlayground object
Ultrasonic ultrasonic1(9, 10); //trig, echo
DHT dht(DHTPIN, DHTTYPE);
int distance = 0, pastDistance = 0;
int heartrate = 0;
byte samplesUntilReport;
const byte SAMPLES_PER_SERIAL_SAMPLE = 5;
void setup(){
  Serial.begin(9600);
  esp8266.begin(9600);
  pinMode(A1, INPUT);
  pulseSensor.analogInput(0); //set the analog input pin for the PulseSensor
  pulseSensor.blinkOnPulse(pulseLED); //set the LED pin for the PulseSensor
  pulseSensor.setThreshold(pulseSensorThreshold); //set the threshold for the PulseSensor
  if (pulseSensor.begin()) {
    Serial.println("PulseSensor found!");
  }
  else {
    Serial.println("No PulseSensor found!");
  }
  // Skip the first SAMPLES_PER_SERIAL_SAMPLE in the loop().
  samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
}

void sensorTasks(){
  //read ultrasound sensor
  distance = ultrasonic1.read();

  //read DHT sensor
  float h = dht.readHumidity();
  float f = dht.readTemperature(true);
  //send data to esp8266 depending on threshold and negative values
  ArduinoJson::StaticJsonDocument<200> doc;
  if(heartrate > 0){
    if(heartrate < 200 && heartrate >50){
      doc["heartrate"] = heartrate;
    }
  }
  if (abs(distance - pastDistance) > distanceThreshold) {
    doc["distance"] = distance;
    pastDistance = distance;
  }
  if(!isnan(h) && !isnan(f)){
    doc["temperature"] = f;
    doc["humidity"] = h;
  }
  String json;
  serializeJson(doc, json);
  if(json.length() > 0 && json != "null"){
    esp8266.println(json);
    Serial.println(json);
  }


}
void loop(){
// For debugging, print esp8266 response to serial monitor
  if (esp8266.available()) {
    //read until available
    while (esp8266.available()) {
      Serial.write(esp8266.read());
    }
  }
  //read heartrate sensor
  if (pulseSensor.sawNewSample()) {
    if (--samplesUntilReport == (byte) 0) {
      samplesUntilReport = SAMPLES_PER_SERIAL_SAMPLE;
      if (pulseSensor.sawStartOfBeat()) {
        heartrate = pulseSensor.getBeatsPerMinute();
      }
    }
  }
  //every 2 seconds print data to esp8266
  if (millis() % 2000 == 0) {
    sensorTasks();
  }
}

//empty sketch
// void setup(){

// }
// void loop(){

// }