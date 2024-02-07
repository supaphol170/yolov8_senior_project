#include <WiFi.h>
#include <WebSocketsClient.h>

const char* ssid = "realme 6 pro";
const char* password = "babe1234";
const char* server = "192.168.80.157";
const int port = 8887;

WebSocketsClient webSocket;

void setup() {
    Serial.begin(115200);

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
            Serial.println("WebSocket Connected!");
            break;
        case WStype_TEXT:
            Serial.printf("Received message: %s\n", payload);
            // Handle the received message as needed
            break;
    }
}
