// #include "painlessMesh.h"
// #include <Arduino.h>

// #define MESH_PREFIX "whateverYouLike"
// #define MESH_PASSWORD "somethingSneaky"
// #define MESH_PORT 5555

// // prototypes
// void receivedCallback( uint32_t from, String &msg );
// painlessMesh  mesh;
// String msg = String(mesh.getNodeId());

// void setup() {
//   Serial.begin(115200);
//   mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
//   mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
//   // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
//   mesh.setContainsRoot(true);
// }

// void loop() {
//   // every 2 seconds send id of self to root
//   if (millis() % 2000 == 0) {
//     //send msg only to root
//     String msg = String(mesh.getNodeId());
//     Serial.printf("%s:Sending msg: %s\n", msg.c_str(), msg.c_str());
//     mesh.sendBroadcast(msg);
//   }
//   mesh.update();
// }

