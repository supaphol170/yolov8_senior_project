#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "realme 6 pro";
const char* password = "babe1234";
const char* server = "127.0.0.1";
const int port = 8887;
const int relay_IN1 = 26;
const int relay_IN2 = 32;
const char* state_lamp;

WebSocketsClient webSocket;

void setup() {
    Serial.begin(115200);
    pinMode(relay_IN1, OUTPUT);
    pinMode(relay_IN2, OUTPUT);
    // Set the initial state of the lamp (closed)
    digitalWrite(relay_IN1, HIGH);
    digitalWrite(relay_IN2, HIGH);

    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");

    // Connect to WebSocket server
    webSocket.begin(server, port, "/");
    webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket Disconnected!");
            // Attempt to reconnect here
            webSocket.begin(server, port, "/");
            break;
        case WStype_CONNECTED:
          //send data to node.js
            webSocket.sendTXT("ESP32 Devkitv1 can connected");
            Serial.println("WebSocket Connected!");
            break;
        case WStype_TEXT:
            Serial.printf("Received message: %s\n", payload);
            // if right it's in 2, if left it's in 1
            // Parse the JSON payload
            DynamicJsonDocument doc(256); // Adjust the size based on your JSON structure
            DeserializationError error = deserializeJson(doc, payload, length);
            if (error) {
                Serial.print("Failed to parse JSON: ");
                Serial.println(error.c_str());
                return;
            }
            // Extract data from the JSON document
            const char* dataValue = doc["data"];
            state_lamp = dataValue;
            //lamp1
            if (strcmp(dataValue, "sleep_left") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN1, LOW);
            }
            else if (strcmp(dataValue, "read_left") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN1, LOW);
                delay(50);
                digitalWrite(relay_IN1, HIGH);
                delay(50);
                digitalWrite(relay_IN1, LOW);
            }
            else if (strcmp(dataValue, "another_left") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN1, LOW);
                delay(50);
                digitalWrite(relay_IN1, HIGH);
                delay(50);
                digitalWrite(relay_IN1, LOW);
                delay(50);
                digitalWrite(relay_IN1, HIGH);
                delay(50);
                digitalWrite(relay_IN1, LOW);
            }
            else if (strcmp(dataValue, "close_left") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN1, HIGH);
            }
            //lamp2
            else if (strcmp(dataValue, "sleep_right") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN2, LOW);
            }
            else if (strcmp(dataValue, "read_right") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN2, LOW);
                delay(50);
                digitalWrite(relay_IN2, HIGH);
                delay(50);
                digitalWrite(relay_IN2, LOW);
            }
            else if (strcmp(dataValue, "another_right") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN2, LOW);
                delay(50);
                digitalWrite(relay_IN2, HIGH);
                delay(50);
                digitalWrite(relay_IN2, LOW);
                delay(50);
                digitalWrite(relay_IN2, HIGH);
                delay(50);
                digitalWrite(relay_IN2, LOW);
            }
            else if (strcmp(dataValue, "close_right") == 0) {
                Serial.printf("Received data is : %s\n", dataValue);
                delay(50);
                digitalWrite(relay_IN2, HIGH);
            } 
            else{
              // for send re-connect status back to flask
              Serial.print("Received message from server: ");
              Serial.println(dataValue);
              // Check if the received message is a reconnection request
              if (strcmp(dataValue, "Reconnect") == 0) {
                // Send a response back to the server
                const char* responseMessage = "ESP32 Devkitv1 can connected";
                // Send the formatted date and time to the Node.js server
                // Return the result to the calling code
                webSocket.sendTXT(responseMessage);
              }
              
            }
          break;
    }
}
