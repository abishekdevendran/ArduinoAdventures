# Real-time Heart Rate and Distance Monitor using ESP8266 and WebSocket

This project is a real-time monitor that displays heart rate and distance readings on a webpage using an ESP8266 WiFi module and a WebSocket connection. It uses an Arduino sketch to read sensor data from an external device and sends the data over WiFi to the client's web browser.

[![wakatime](https://wakatime.com/badge/github/abishekdevendran/ArduinoAdventures.svg)](https://wakatime.com/badge/github/abishekdevendran/ArduinoAdventures)

## Components Required

- ESP8266 WiFi module
- Breadboard
- Jumper wires
- Heart rate sensor (e.g. MAX30100)
- Distance sensor (e.g. HC-SR04)

## Wiring

The wiring for this project is as follows:

VCC ──────── 3.3V
GND ──────── GND
MAX30100 ────── SCL (D1 on ESP8266)
MAX30100 ────── SDA (D2 on ESP8266)
HC-SR04 ────── TRIG (D5 on ESP8266)
HC-SR04 ────── ECHO (D6 on ESP8266)


Note: The above wiring is for the default pins used in the Arduino sketch. If you want to use different pins, you will need to modify the sketch accordingly.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
