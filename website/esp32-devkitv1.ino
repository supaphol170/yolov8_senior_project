#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "supaphol";
const char* password = "63070170";
const char* server = "10.50.5.15";
const int port = 8887;
const int relay_IN1 = 26;
const int relay_IN2 = 32;
String previous_message_left;
String current_message_left;
String previous_message_right;
String current_message_right;
int index_of_previous_message_left;
int index_of_current_message_left;
int index_of_previous_message_right;
int index_of_current_message_right;
const char* message_list[] = {"sleep_left", "read_left", "another_left", "close_left"};//for base check between current and previous variable for know how many count close and open lamp.
const int message_list_size = sizeof(message_list) / sizeof(message_list[0]);

WebSocketsClient webSocket;

// Function to find the index of a word in the message list
int findIndex(const String& word) {
    for (int i = 0; i < message_list_size; i++) {
        if (word == message_list[i]) {
            return i; // Return the index if the word is found
        }
    }
    return -1; // Return -1 if the word is not found
}

void setup() {
    Serial.begin(115200);
    pinMode(relay_IN1, OUTPUT);
    pinMode(relay_IN2, OUTPUT);
    // Set the initial state of the lamp (closed)
    digitalWrite(relay_IN1, HIGH);
    digitalWrite(relay_IN2, HIGH);
    // Set the initial state of lamp for default.
    previous_message_left = "read_left";
    previous_message_right = "read_right";
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
            webSocket.sendTXT(payload);
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
            String dataValue = doc["data"].as<String>();// for compare value
            const char* value = doc["data"]; //for show output
            //lamp1
            //check this value are same previous or not if it's same don't do anyting/
            if (dataValue == previous_message_left){
              // If the received message is the same as the previous one
              Serial.println("Received message is the same as the previous one, no action required");
            }
            //If it's not same have to change
            else{
              current_message_left = dataValue;
              index_of_previous_message_left = findIndex(previous_message_left);
              index_of_current_message_left = findIndex(current_message_left);
              Serial.print("Index of previous_message_left before compare : ");
              Serial.println(index_of_previous_message_left);
              Serial.print("Index of current_message_left before compare : ");
              Serial.println(index_of_current_message_left);
              if (index_of_previous_message_left == index_of_current_message_left){
                Serial.println("previous message are same curreent message so noting to change lamp");
              }
              else if (index_of_previous_message_left > index_of_current_message_left){
                for (int i = 0; i < (index_of_current_message_left-index_of_previous_message_left); i++) {
                  delay(50);
                  digitalWrite(relay_IN1, HIGH);//close lamp i
                  delay(50);
                  digitalWrite(relay_IN1, LOW);//open lamp i
                }
                previous_message_left = current_message_left;
                current_message_left = "";
                Serial.print("Index of previous_message_left : ");
                Serial.println(previous_message_left);
                Serial.print("Index of current_message_left : ");
                Serial.println(current_message_left);
              }
              else if (index_of_previous_message_left < index_of_current_message_left){
                for (int i = 0; i < (index_of_previous_message_left-index_of_current_message_left); i++) {
                  delay(50);
                  digitalWrite(relay_IN1, HIGH);//close lamp i
                  delay(50);
                  digitalWrite(relay_IN1, LOW);//open lamp i
                }
                previous_message_left = current_message_left;
                current_message_left = "";
                Serial.print("Index of previous_message_left : ");
                Serial.println(previous_message_left);
                Serial.print("Index of current_message_left : ");
                Serial.println(current_message_left);
              }
            }
          break;
    }
}
