// #include "painlessMesh.h"
// #include <Arduino.h>
// #include <ArduinoJson.h>

// #define MESH_PREFIX "whateverYouLike"
// #define MESH_PASSWORD "somethingSneaky"
// #define MESH_PORT 5555

// // prototypes
// void receivedCallback( uint32_t from, String &msg );
// painlessMesh  mesh;
// String msg = String(mesh.getNodeId());
// // array to filter required sensorData
// String filter[];
// void setup() {
//   Serial.begin(9600);
//   mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
//   mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
//   // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
//   mesh.setContainsRoot(true);
// }

// void loop() {
//   // if nodes>1, and serial input is available, send it to the root
//   if (Serial.available()) {
//     //handle serial input with error handling
//     String input = Serial.readStringUntil('\n');
//     input.trim();
//     if (input.length() > 0) {
//       //filter input based on filter
//       //send to root
//       mesh.sendBroadcast( input );
//     }
//     Serial.printf("Sending %s to root\n", input.c_str());
//   }
//   mesh.update();
// }

