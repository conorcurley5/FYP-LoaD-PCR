#include <Arduino.h>
#include <Wire.h>
// Include ModbusMaster library
// Include motor functions
#include <motor.h>
// Include websocket functions
#include <websocket.h>
#include <fluorescence.h>

AsyncWebServer& server = get_server_instance();

unsigned long previousMillis = 0;  // Stores the last time the code was executed
const long interval = 3000;

volatile bool isSetUp = false;

void setup() {
    // setup_modbus();
    Serial.begin(9600);

    setup_wifi();
    setup_websocket();
    setup_modbus();
    setup_fluorescence();

    server.begin();
}

void loop() {
    if (!isSetUp) {
        set_motor_mode(1);
        stop_motor();
        isSetUp = true;
    };

    unsigned long currentMillis = millis(); 
    // Check if the interval has passed
    if (currentMillis - previousMillis >= interval) {
        // Save the last time you ran the code
        previousMillis = currentMillis;

        // Your code to run periodically here
        fetch_vitals();

        fetch_fluorescence();
    }

    Serial.println("Loop Complete.");
}