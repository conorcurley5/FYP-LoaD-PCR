#include <Arduino.h>
#include <Wire.h>
// Include ModbusMaster library
// Include motor functions
#include <motor.h>
// Include websocket functions
#include <websocket.h>
#include <fluorescence.h>

AsyncWebServer& server = get_server_instance();

// Set up intervals for timed vitals
unsigned long previousMillis = 0;  // Stores the last time the code was executed
const long interval = 3000;

// Boolean to ensure setup functions are only run once. 
// This cannot be performed in setup on ESP32. DO NOT RUN MODBUS FUNCTIONS IN SETUP. The program will stall.
// This took me days to figure out, but your mileage might vary on Arduino.
volatile bool isSetUp = false;

void setup() {
    // setup_modbus();
    Serial.begin(9600);

    // Set up the relevant functions.
    setup_wifi();
    setup_websocket();
    setup_modbus();
    setup_fluorescence();

    // Start the server.
    server.begin();
}

void loop() {
    // Run setup functions
    if (!isSetUp) {
        set_motor_mode(1);
        stop_motor();
        isSetUp = true;
    };

    // At a timed intervals, send vitals and fluoresence readings to websocket client 
    unsigned long currentMillis = millis(); 
    // Check if the interval has passed
    if (currentMillis - previousMillis >= interval) {
        // Save the last time you ran the code
        previousMillis = currentMillis;

        // Your code to run periodically here
        fetch_vitals();

        fetch_fluorescence();
    }

    // Console log once a loop completes for debuging.
    Serial.println("Loop Complete.");
}
