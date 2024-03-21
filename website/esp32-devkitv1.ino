#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "supaphol";
const char* password = "63070170";
const char* server = "192.168.31.157";
const int port = 8887;
const int relay_IN1 = 32;
const int relay_IN2 = 33;
String previous_message;
String current_message;
int index_of_previous_message;
int index_of_current_message;
const char* message_list[] = {"read", "another", "sleep", "close"};//for base check between current and previous variable for know how many count close and open lamp.
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
    previous_message = "read";
    previous_message = "read";
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
            char *token = strtok((char *)dataValue.c_str(), "_");
            String first_token = token; //value before "_"
            token = strtok(NULL, "_"); //value after "_"
            //lamp1
            //check this value are same previous or not if it's same don't do anyting/
            if (dataValue == previous_message){
              // If the received message is the same as the previous one
              Serial.println("Received message is the same as the previous one, no action required");
            }
            //If it's not same have to change
            else{
              current_message = dataValue;
              index_of_previous_message = findIndex(previous_message);
              index_of_current_message = findIndex(current_message);
              Serial.print("Index of previous_message before compare : ");
              Serial.println(index_of_previous_message);
              Serial.print("Index of current_message before compare : ");
              Serial.println(index_of_current_message);
              if (index_of_previous_message == index_of_current_message){
                Serial.println("previous message are same curreent message so noting to change lamp");
              }
              else if (index_of_previous_message > index_of_current_message){
                for (int i = 0; i < (index_of_current_message-index_of_previous_message); i++) {
                  delay(50);
                  digitalWrite(relay_IN1, HIGH);//close lamp i
                  delay(50);
                  digitalWrite(relay_IN1, LOW);//open lamp i
                }
                previous_message = current_message;
                current_message = "";
                Serial.print("Index of previous_message : ");
                Serial.println(previous_message);
                Serial.print("Index of current_message : ");
                Serial.println(current_message);
              }
              else if (index_of_previous_message < index_of_current_message){
                for (int i = 0; i < (index_of_previous_message-index_of_current_message); i++) {
                  delay(50);
                  digitalWrite(relay_IN1, HIGH);//close lamp i
                  delay(50);
                  digitalWrite(relay_IN1, LOW);//open lamp i
                }
                previous_message = current_message;
                current_message = "";
                Serial.print("Index of previous_message : ");
                Serial.println(previous_message);
                Serial.print("Index of current_message : ");
                Serial.println(current_message);
              }
            }
          break;
    }
}
