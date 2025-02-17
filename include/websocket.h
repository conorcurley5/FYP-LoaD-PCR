#include <Arduino.h>
// Include async libraries for Websockets and servers
#include <ESPAsyncWebServer.h>

#ifndef WEBSOCKET_H
#define WEBSOCKET_H

// Connect to WiFi
void setup_wifi();

void notify_clients(String sensorReadings);
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
void setup_websocket();

// Handle websocket events
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

AsyncWebServer& get_server_instance();
AsyncWebSocket& get_websocket_instance();

#endif