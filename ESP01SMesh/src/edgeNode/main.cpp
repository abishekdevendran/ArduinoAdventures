#include <painlessMesh.h>
#include <PubSubClient.h>
#include <CertStoreBearSSL.h>
#include <time.h>
#include <FS.h>
#include <LittleFS.h>

#define MQTT_DEBUG

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

// #define WIFI_SSID "Abishek's Nord"
// #define WIFI_PASSWORD "berySneak"
#define WIFI_SSID "abishekMSI"
#define WIFI_PASSWORD "EperOmte"
#define STATION_PORT     5555

// #define MQTT_SERVER "f3a1076c3f5f4aed88138a010566de24.s2.eu.hivemq.cloud"
#define MQTT_SERVER "f3a1076c3f5f4aed88138a010566de24.s2.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "username"
#define MQTT_PASSWORD "password"
#define MQTT_TOPIC "painlessMesh/from/"

#define HOSTNAME "IIITDMKBridgeNode"

painlessMesh mesh;
PubSubClient mqttClient; 
WiFiClient wifiClient;
BearSSL::WiFiClientSecure bear;

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);

// A single, global CertStore which can be used by all
// connections.  Needs to stay live the entire time any of
// the WiFiClientBearSSLs are present.
BearSSL::CertStore certStore;

void onNewConnection(uint32_t nodeId) {
  Serial.printf("\nNew connection from node %u\n", nodeId);
}

void onConnectionLost(uint32_t nodeId) {
  Serial.printf("\nConnection lost from node %u\n", nodeId);
}

void onReceive(uint32_t from, String &message) {
  Serial.printf("\nReceived message from node %u: %s\n", from, message.c_str());
  //make a char * topic
  char topic[50];
  sprintf(topic, "%s%u", MQTT_TOPIC, from);
  Serial.printf("The new topic is: %s\n",topic);
  mqttClient.publish(topic, message.c_str());
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("\nWaiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("\nCurrent time: ");
  Serial.print(asctime(&timeinfo));
}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length+1);
  String msg = String(cleanPayload);
  free(cleanPayload);

  String targetStr = String(topic).substring(16);

  if(targetStr == "gateway")
  {
    if(msg == "getNodes")
    {
      auto nodes = mesh.getNodeList(true);
      String str;
      for (auto &&id : nodes)
        str += String(id) + String(" ");
      mqttClient.publish("painlessMesh/from/gateway", str.c_str());
    }
  }
  else{
    //treat it as an application update
  }
}

void mqttSetup(){
  // case 1: use secure wifi client
  mqttClient.setClient(bear);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  if (mqttClient.connect(HOSTNAME, MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("Connected to MQTT");
    mqttClient.publish("painlessMesh/from/gateway","Ready!");
    mqttClient.subscribe("painlessMesh/to/#");
    //done, return
    return;
  } 
  else {
    Serial.println("MQTT connection failed");
    Serial.printf("MQTT state: %d\n", mqttClient.state());
  }
  // case 2: use insecure wifi client
  mqttClient.setClient(wifiClient);
  mqttClient.setServer(WiFi.gatewayIP(), 5555);
  if(mqttClient.connect(HOSTNAME)){
    Serial.println("Connected to MQTT");
    mqttClient.publish("painlessMesh/from/gateway","Ready!");
    mqttClient.subscribe("painlessMesh/to/#");
    return;
  }
  else{
    Serial.println("MQTT connection failed on local port 5555");
    Serial.printf("MQTT state: %d\n", mqttClient.state());
  }
  return;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up");
  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | MESH_STATUS); // all types on
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
  mesh.onNewConnection(&onNewConnection);
  mesh.onDroppedConnection(&onConnectionLost);
  mesh.onReceive(&onReceive);
  mesh.stationManual(WIFI_SSID, WIFI_PASSWORD);
  mesh.setHostname(HOSTNAME);
  mesh.setRoot(true);
  mesh.setContainsRoot(true);

  LittleFS.begin();
  if(!LittleFS.begin()){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
    return;  // Can't connect to anything w/o certs!
  }
  // Integrate the cert store with this connection
  bear.setCertStore(&certStore);
  bear.setInsecure();
  mqttClient.setClient(bear);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

void loop() {
  mesh.update();
  mqttClient.loop();

  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());
    Serial.println("Connected to WiFi");
    randomSeed(micros());
    // setClock();// Required for X.509 validation
    mqttClient.setKeepAlive(300);
    mqttClient.setCallback(mqttCallback);
    mqttSetup();
  }
}
IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}