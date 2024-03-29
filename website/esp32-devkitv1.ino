#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

const char* ssid = "Supaphol";
const char* password = "63070170";
const char* server = "192.168.78.157";
const int port = 8887;
const int relay_IN1 = 26;
const int relay_IN2 = 27;
// Variables for Lamp 1
String previous_message1 = "";
String current_message1 = "";
int index_of_previous_message1;
int index_of_current_message1;

// Variables for Lamp 2
String previous_message2 = "";
String current_message2 = "";
int index_of_previous_message2;
int index_of_current_message2;
const char* message_list[] = {"read", "another", "sleep", "close"};
const int message_list_size = sizeof(message_list) / sizeof(message_list[0]);

WebSocketsClient webSocket;

int findIndex(const String& word) {
    for (int i = 0; i < message_list_size; i++) {
        if (word == message_list[i]) {
            return i;
        }
    }
    return -1;
}

void setup() {
  Serial.begin(115200);
  pinMode(relay_IN1, OUTPUT);
  pinMode(relay_IN2, OUTPUT);
  digitalWrite(relay_IN1, HIGH);
  digitalWrite(relay_IN2, HIGH);
  previous_message1 = "close"; // Set initial value
  previous_message2 = "close"; // Set initial value
  current_message1 = "close"; // Set initial value
  current_message2 = "close"; // Set initial value
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
  // for show first state before do action  
  index_of_previous_message1 = findIndex(previous_message1);
  index_of_current_message1 = findIndex(current_message1);
  Serial.println("---------------------START---------------------");
  Serial.print("Index of previous_message1 before compare : ");
  Serial.println(index_of_previous_message1);
  Serial.print("Index of current_message1 before compare : ");
  Serial.println(index_of_current_message1);
  Serial.println(".............................");
  // for show first state before do action  
  index_of_previous_message2 = findIndex(previous_message2);
  index_of_current_message2 = findIndex(current_message2);
  Serial.println("---------------------START---------------------");
  Serial.print("Index of previous_message2 before compare : ");
  Serial.println(index_of_previous_message2);
  Serial.print("Index of current_message2 before compare : ");
  Serial.println(index_of_current_message2);
  Serial.println(".............................");
}

void loop() {
  webSocket.loop();
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch (type) {
        case WStype_DISCONNECTED:
            Serial.println("WebSocket Disconnected!");
            webSocket.begin(server, port, "/");
            break;
        case WStype_CONNECTED:
            webSocket.sendTXT("ESP32 Devkitv1 connected");
            Serial.println("WebSocket Connected!");
            break;
        case WStype_TEXT:
            Serial.printf("Received message: %s\n", payload);
            DynamicJsonDocument doc(256);
            DeserializationError error = deserializeJson(doc, payload, length);// for check error json.
            if (error) {
                Serial.print("Failed to parse JSON: ");
                Serial.println(error.c_str());
                webSocket.sendTXT(payload);
                return;
            }
            String dataValue = doc["data"].as<String>();
            const char* value = doc["data"];
            char *token = strtok((char *)dataValue.c_str(), "_");
            String action = token; //before "_"
            token = strtok(NULL, "_");
            String lamp = token; //after "_"
            Serial.println(lamp);
            // Check if tokenization was successful
            if (!lamp.isEmpty()) {
                // Tokenization successful
                if (lamp == "left") {
                  // Handle message for lamp1
                  webSocket.sendTXT(payload);
                  handleLampMessage(relay_IN1, action, previous_message1, current_message1, index_of_previous_message1, index_of_current_message1);
              } else if (lamp == "right") {
                  // Handle message for lamp2
                  webSocket.sendTXT(payload);
                  handleLampMessage(relay_IN2, action, previous_message2, current_message2, index_of_previous_message2, index_of_current_message2);
              } else {
                  webSocket.sendTXT(payload);
                  Serial.println("Invalid lamp identifier");
              }
            } else {
                // Tokenization failed
                if (dataValue == "Hello ESP32!"){
                  webSocket.sendTXT(payload);
                }
                else if (dataValue == "reconnect"){
                  webSocket.sendTXT("ESP32-DevkitV1 are connected now!!");
                }
                else {
                  //for when from auto it's open both of lamp.
                  webSocket.sendTXT(payload);
                  handleLampMessage(relay_IN1, dataValue, previous_message1, current_message1, index_of_previous_message1, index_of_current_message1);
                  handleLampMessage(relay_IN2, dataValue, previous_message2, current_message2, index_of_previous_message2, index_of_current_message2);
                }
            }
            break;
    }
}

void handleLampMessage(int relayPin, String action, String &previous_message, String &current_message, int &index_of_previous_message, int &index_of_current_message){
  if (action == previous_message) {
      Serial.println("Received message is the same as the previous one, no action required");
      return;
  }
  else if (action == "close"){
      digitalWrite(relayPin, HIGH);
      return;
  }
  current_message = action;
  index_of_previous_message = findIndex(previous_message);
  index_of_current_message = findIndex(current_message);
  Serial.print("Index of previous_message before compare : ");
  Serial.println(index_of_previous_message);
  Serial.print("Index of current_message before compare : ");
  Serial.println(index_of_current_message);
  if (index_of_previous_message == index_of_current_message) {
      Serial.println("Previous message is the same as the current message, no action required");
  } else if (index_of_previous_message < index_of_current_message) {
      for (int i = index_of_previous_message; i <= index_of_current_message-index_of_previous_message; i++) {
          digitalWrite(relayPin, HIGH); // Close lamp i
          delay(30);
          digitalWrite(relayPin, LOW); // Open lamp i
          delay(30);
      }
      previous_message = current_message;
  } else if (index_of_previous_message > index_of_current_message) {
      // Calculate the number of states to cycle through
      int states_to_cycle = index_of_previous_message - index_of_current_message;
      // Turn off and on the lamp for each state transition
      for (int i = 0; i < ((message_list_size-1)-states_to_cycle); i++) {
          // Turn off the lamp
          digitalWrite(relayPin, HIGH);
          delay(30);
          // Turn on the lamp
          digitalWrite(relayPin, LOW);
          delay(30);
      }
      previous_message = current_message;
  } else {
      Serial.println("Invalid message order");
  }
}
