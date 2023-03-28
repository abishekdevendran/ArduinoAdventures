// #include "painlessMesh.h"

// #define   MESH_PREFIX     "disTheBestMesh"
// #define   MESH_PASSWORD   "muchSneak"
// #define   MESH_PORT       5555

// // prototypes
// void receivedCallback( uint32_t from, String &msg );
// painlessMesh  mesh;

// void setup() {
//   Serial.begin(115200);
//   Serial.println("bridge: Starting");
//   mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages
//   // Channel set to 6. Make sure to use the same channel for your mesh and for you other
//   // network (STATION_SSID)
//   mesh.init( MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6 );
//   mesh.setRoot(true);
//   // This node and all other nodes should ideally know the mesh contains a root, so call this on all nodes
//   mesh.setContainsRoot(true);
//   mesh.onReceive(&receivedCallback);
// }

// void loop() {
//   mesh.update();
// }

// void receivedCallback( uint32_t from, String &msg ) {
//   Serial.printf("bridge: Received from %u msg=%s\n", from, msg.c_str());
// }

