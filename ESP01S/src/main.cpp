#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>

const char* ssid = "ESPWifi"; // SSID for the AP
const char* password = "87654321"; // Password for the AP
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
int lastHeartrate = -1;
int lastDistance = -1;

String html = R"(<doctype html><html><head><title>ESP8266 Websocket</title></head><body><h1>ESP8266 Websocket</h1><h2>Heart Rate</h2><div id='heartrate'></div><h2>Distance</h2><div id='distance'></div><script>var ws = new WebSocket('ws://' + window.location.hostname + ':80/ws');ws.onmessage = function(event) {var data = JSON.parse(event.data);document.getElementById('heartrate').innerHTML = data.heartrate;document.getElementById('distance').innerHTML = data.distance;};</script></body></html>)";

void setup() {
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  //when a client requests "/", send html response and connect to websocket
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", String(html));
  });
  //when a client connects to the websocket, send a message to the client
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      // Serial.printf("ws[%s][%u] connect from %s\n", server->url(), client->id(), client->remoteIP().toString().c_str());
      client->text("Connected");
    } else if (type == WS_EVT_DISCONNECT) {
      // Serial.printf("ws[%s][%u] disconnect from %s\n", server->url(), client->id(), client->remoteIP().toString().c_str());
      ;
    }
  });
  server.addHandler(&ws);
  server.begin();
}

void loop() {
  //read data from WiFi module
  if (Serial.available() > 0) {
    StaticJsonDocument<64> doc;
    DeserializationError error = deserializeJson(doc, Serial.readStringUntil('\n'));
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
    }
    else {
      lastDistance = doc["distance"];
      lastHeartrate = doc["heartrate"];
    }
  }

  //every 500ms, send the sensor data to the client
  static unsigned long lastSend = 0;
  if (millis() - lastSend > 500) {
    lastSend = millis();
    StaticJsonDocument<64> doc;
    doc["distance"] = lastDistance;
    doc["heartrate"] = lastHeartrate;
    char buffer[64];
    serializeJson(doc, buffer);
    ws.textAll(buffer);
  }
}
