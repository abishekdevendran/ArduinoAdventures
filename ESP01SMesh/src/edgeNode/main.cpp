#include <painlessMesh.h>
#include <PubSubClient.h>
#include <CertStoreBearSSL.h>
#include <time.h>
#include <FS.h>
#include <LittleFS.h>

#define MESH_PREFIX "whateverYouLike"
#define MESH_PASSWORD "somethingSneaky"
#define MESH_PORT 5555

#define WIFI_SSID "abishekMSI"
#define WIFI_PASSWORD "EperOmte"
#define STATION_PORT     5555

// #define MQTT_SERVER "f3a1076c3f5f4aed88138a010566de24.s2.eu.hivemq.cloud"
#define MQTT_SERVER "f3a1076c3f5f4aed88138a010566de24.s2.eu.hivemq.cloud"
#define MQTT_PORT 8883
#define MQTT_USER "username"
#define MQTT_PASSWORD "password"
#define MQTT_TOPIC "yourMqttTopic"

#define HOSTNAME "IIITDMKChingChong"

painlessMesh mesh;
PubSubClient mqttClient; 
BearSSL::WiFiClientSecure bear;

IPAddress getlocalIP();
IPAddress myIP(0,0,0,0);

// A single, global CertStore which can be used by all
// connections.  Needs to stay live the entire time any of
// the WiFiClientBearSSLs are present.
BearSSL::CertStore certStore;


static uint32_t lastPrint = 0;

void onNewConnection(uint32_t nodeId) {
  Serial.printf("New connection from node %u\n", nodeId);
}

void onConnectionLost(uint32_t nodeId) {
  Serial.printf("Connection lost from node %u\n", nodeId);
}

void onReceive(uint32_t from, String &message) {
  Serial.printf("Received message from node %u: %s\n", from, message.c_str());
  mqttClient.publish(MQTT_TOPIC, message.c_str());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if the first character is present
  if ((char)payload[0] != '\0') {
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  } else {
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
  }
}

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
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
  else if(targetStr == "broadcast") 
  {
    mesh.sendBroadcast(msg);
  }
  else
  {
    uint32_t target = strtoul(targetStr.c_str(), NULL, 10);
    if(mesh.isConnected(target))
    {
      mesh.sendSingle(target, msg);
    }
    else
    {
      mqttClient.publish("painlessMesh/from/gateway", "Client not connected!");
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting up");

  // WiFi.mode(WIFI_AP_STA);
  // WiFi.setHostname(HOSTNAME);
  // WiFi.setAutoReconnect(true);
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD, 6, NULL, true);

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE | MESH_STATUS); // all types on
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT, WIFI_AP_STA, 6);
  // mesh.onNewConnection(&onNewConnection);
  // mesh.onDroppedConnection(&onConnectionLost);
  mesh.onReceive(&onReceive);
  mesh.stationManual(WIFI_SSID, WIFI_PASSWORD);
  // mesh.setHostname(HOSTNAME);
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
    Serial.println("\nConnected to WiFi");
    randomSeed(micros());
    setClock();// Required for X.509 validation
    if (mqttClient.connect(HOSTNAME, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("Connected to MQTT");
      mqttClient.publish("painlessMesh/from/gateway","Ready!");
      mqttClient.subscribe("painlessMesh/to/#");
    } 
    else {
      Serial.println("MQTT connection failed");
    }
  }
}
IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}