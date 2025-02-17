#include <WiFi.h>
#include <Arduino_JSON.h>
// Include async libraries for Websockets and servers
#include <ESPAsyncWebServer.h>
// Include JSON
#include <Arduino_Json.h>
// include motor functions
#include <motor.h>
#include <fluorescence.h>

// Create server object on port 80, then websocket
static AsyncWebServer server(80);
static AsyncWebSocket ws("/ws");

AsyncWebServer& get_server_instance()
{
    return server;
}

AsyncWebSocket& get_websocket_instance()
{
    return ws;
}

// Put in network credentials
// Probably shouldn't be out on the internet but sure, whoops.
// TODO: Store these in an environment variable instead for security.
const char* ssid = "Whitehouse Wi-Fi";
const char* password = "Whitehouse08!";

// Connect to WiFi
void setup_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi ..");
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print('.');
        delay(1000);
    }
    Serial.println(WiFi.localIP());
}

void notify_clients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

// Handle an incoming request from the master.
void handle_command(char* buffer) {
  // Get the character length of the command
  int howMany = strlen(buffer);

  // Find the index of the equals sign
  int index = -1;

  for (int i = 0; i < howMany; i++) {
    if (buffer[i] == '=') {
      index = i;
    }
  }

  // Return if you cant find an =, request is malformed.
  if (index == -1) return;

  // Split the request into an action (i.e what are you requesting, e.g. SET_MOTOR_SPEED) and a payload (i.e. the value, e.g. 3000)
  char action[index + 1];
  char payload[howMany-index];

  for (int i = 0; i < index; i++) {
    action[i] = buffer[i];
  }
  // Terminate the strings
  action[index] = '\0';
    
  for (int i = index + 1; i < howMany; i++) {
    payload[i - index - 1] = buffer[i];  // Use i - index - 1 to correctly place characters in the payload
  }
  payload[howMany - index - 1] = '\0';
  
  // Process the actions using IF/ELSE
  // C++ cannot do SWITCH statements on strings FML.
  if (strcmp(action, "SET_MTR_SPD") == 0) {
    int speed = parseInt(payload);
    set_motor_speed(speed);
  } else if (strcmp(action, "TGL_MTR_ON") == 0) {
    if (strcmp(payload, "1") == 0) {
      start_motor();
    } else {
      stop_motor();
    }
  } else if (strcmp(action, "TGL_MTR_MODE") == 0) {
    if (strcmp(payload, "0") == 0) {
      set_motor_mode(0);
    } else {
      set_motor_mode(1);
    }
  } else if (strcmp(action, "TGL_MTR_MODE") == 0) {
    if (strcmp(payload, "0") == 0) {
      set_motor_mode(0);
    } else {
      set_motor_mode(1);
    }
  } else if (strcmp(action, "FLR_SET_GAIN") == 0) {
    int gain = parseInt(payload);
    set_gain(gain);
  } else if (strcmp(action, "FLR_SET_ATIME") == 0) {
    int atime = parseInt(payload);
    set_atime(atime);
  } else if (strcmp(action, "FLR_SET_ASTEP") == 0) {
    int astep = parseInt(payload);
    set_astep(astep);
  } else {
    Serial.println("unknown command");
  }
}

// When a message is recieved, handle it
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  // boilerplate code from the developer
  AwsFrameInfo *info = (AwsFrameInfo*)arg;

  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;

    // Send data to our function above
    handle_command((char*)data);
  }
}

// Handle websocket events
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// Initialise the websocket
void setup_websocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}