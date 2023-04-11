#include "painlessMesh.h"
#include <Arduino.h>
#include <ArduinoJson.h>

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

// prototypes
void receivedCallback( uint32_t from, String &msg );
painlessMesh  mesh;
String msg = String(mesh.getNodeId());
String sensors="";
String filter = "THDP";
String values[] = {"temperature", "humidity", "distance", "heartrate"};
String *filteredValues;
void setup() {
  Serial.begin(9600);
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
  mesh.onReceive(&receivedCallback);
  // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
  mesh.setContainsRoot(true);
}

void loop() {
  // if nodes>1, and serial input is available, send it to the root
  if (Serial.available()) {
    //handle serial input with error handling
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input.length() > 0) {
      //deserialize input to json
      ArduinoJson::StaticJsonDocument<200> doc;
      deserializeJson(doc, input);
      // for every key in doc, if it exists in sensors, continue, else drop field
      JsonObject root = doc.as<JsonObject>();
      for (JsonPair kv : root) {
        String temp=kv.key().c_str();
        //check if temp exist in values
        bool found = false;
        int i=0;
        while(!found && i<sensors.length()){
          Serial.println(filteredValues[i]);
          if(temp==filteredValues[i]){
            found=true;
          }
          i++;
        }
        if(!found){
          doc.remove(temp);
        }
      }
      //check if doc is empty
      if(doc.size()==0){
        Serial.println("Empty doc");
      }
      else{
        // back to string
        input="";
        serializeJson(doc, input);
        //send to root
        mesh.sendBroadcast( input );
        Serial.printf("Sending %s to root\n", input.c_str());
      }
    }else{
      Serial.println("Serial reading gone wrong :(");
    }
  }
  mesh.update();
}

void receivedCallback(uint32_t from, String &msg){
  //set new sensors value
  sensors = msg.c_str();
  Serial.printf("Set sensors value as: %s\n",sensors.c_str());
  // filter main values
  String *temp = new String[sensors.length()];
  Serial.print("Setting filteredValues as: ");
  for(int i=0;i<sensors.length();i++){
    int idx=filter.indexOf(sensors[i]);
    temp[i]=values[idx];
    Serial.printf("%s\n",temp[i].c_str());
  }
  filteredValues=temp;
  Serial.printf("\n");
}

