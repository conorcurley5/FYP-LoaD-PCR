// Include ModbusMaster library
#include <ModbusMaster.h>
// Include Arduino_JSON for sending vitals
#include <Arduino_JSON.h>
// Include functions from websocket for comms
#include <websocket.h>

// Define Modbus Pins
//  These are two of the pins on the RS485 transiever, the other two are RX and TX, which are automatically
//  allocated by using Serial.
//  Data enable (send)
#define MAX485_DE      10
//  Recieve enable (recieve)
#define MAX485_RE_NEG  11

// Declare the Modbus Master class (holds all of the Modbus Functions)
ModbusMaster node;

// You can ignore this function
//  If you want to use this modbus instance anyhere else in the application, this function returns a pointer to it
ModbusMaster& get_modbus_master_instance()
{
    return node;
}

// The number of datapoints (vitals) about the condition of the motor
const int num_vitals = 5;

// The modbus register indices that store these data for retrieval
const uint16_t vitals_registers[num_vitals] = {
    0x005F,
    0x0066,
    0x0076,
    0x0096,
    0x00C6
};

// The text names for this vitals that should be sent via websockets to the host
const char* register_titles[num_vitals] = {
    "ACT_MTR_SPD",
    "MTR_RUN_QRY",
    "MTR_FLT_CDE",
    "DRV_INT_TMP",
    "ACT_MTR_VLT"
};

// Function to parse an integer from a character buffer (string)
int parseInt (char buffer[]) {
  int num = 0;

  for (int i = 0; buffer[i] != '\0'; i++) {
    num = num * 10 + (buffer[i] - '0');  // Convert each character to an integer and accumulate
  }

  return num;
}

// Initialise RE/DE pins before transmission
void preTransmission()
{
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);
}

// Initialise RE/DE pins after transmission
void postTransmission()
{
  digitalWrite(MAX485_RE_NEG, 0);
  digitalWrite(MAX485_DE, 0);
}

// Declare setup function
void setup_modbus()
{
  // Start the serial connection at 9600 baud rate
  //  If you're using Arduino, use Serial instead of Serial1
  Serial1.begin(9600);

  // Set the pinmode of each of the pins declared above
  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  // Write initial values to the pins
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);

  // Begin the Modbus Master instance with a slave ID of 0x01 on the Serial1 serial connection.
  node.begin(0x01, Serial1);
  // Register the pre and post transmission callbacks for the ModbusMaster instance.
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

// Function to fetch a value from the motor controller
uint16_t fetch_value (uint16_t reg) {
  // 16 bit (2 byte) to hold controller response
  uint16_t result;
  // Get the value from the registers
  result = node.readHoldingRegisters(reg, 1);
  // 16 bit (2 byte) to hold target data from the response
  uint16_t response = 0x00;

  // If the request is successful
  if (result == node.ku8MBSuccess) {
    // Set response equal to the value of the response buffer at index 0.
    response = node.getResponseBuffer(0);
  }

  // clear the response buffer so next request will also be at index 0.
  node.clearResponseBuffer();
  // Return the response
  return response;
} 

// Function to fetch all listed vitals from the controller
void fetch_vitals () {
  // Set up a JSON object to store the vitals in a format conducive to web-based websocket client
  JSONVar vitals_obj;
  // Create an array to hold each of the vitals
  uint16_t vitals[num_vitals];

  // Set the type of the vitals object to MOTOR_VITALS so it can be processed correctly by the websocket client
  vitals_obj["type"] = "MOTOR_VITALS";

  // Loop over each of the vitals registers and fetch values
  for (int i = 0; i < num_vitals; i++) {
    vitals_obj[register_titles[i]] = (int)fetch_value(vitals_registers[i]);
    // Blocking delay after each request to prevent crosstalk
    // TODO: Check if this is even necessary
    delay(200);
  }

  // Convert the JSON object to a string
  String v = JSON.stringify(vitals_obj);
  // Send the vitals object to the websocket client via websocket.h
  notify_clients(v);
}

// Function to send the command to start the motor
void start_motor () {
  uint8_t result;
  // Write 0x01 (1) to the corresponding register (0x0066)
  // Locations of registers for these commands are available in the datasheet
  // https://www.omc-stepperonline.com/digital-brushless-dc-motor-driver-12-30vdc-max-8-0a-120w-bld-305s?srsltid=AfmBOorNjh4E7RWXbFQKLfUvEyLzFFCwWfr8LMeTnb1IW6SbcKYGp0_a
  // Downloads > Manuals > Page 6+
  result = node.writeSingleRegister(0x0066, 1);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Start Success.");
  } else {
    Serial.println("Error starting motor rotation.");
  }

  node.clearResponseBuffer();
}

// Stop the motor
void stop_motor () {
  uint8_t result;
  result = node.writeSingleRegister(0x0066, 0);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Stop Success.");
  } else {
    Serial.println("Error stopping motor rotation.");
  }

  node.clearResponseBuffer();
}

// Set the speed of the motor
void set_motor_speed (int speed) {
  uint8_t result;
  // You can pass an integer value here, ModbusMaster will handle that internally.
  result = node.writeSingleRegister(0x0056, speed);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Speed Set.");
  } else {
    Serial.println("Error setting motor speed.");
  }
  
  node.clearResponseBuffer();
}

// Set the motor mode
// The motor needs to be in one of two modes to communicate over modbus.
// If the request is successful but the controller doesnt respond to commands, try changing this.
void set_motor_mode (int mode) {
  uint8_t result;
  
  result = node.writeSingleRegister(0x0136, mode);

  delay(500);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Mode Set.");
  } else {
    Serial.println("Error setting motor mode.");
    Serial.println(result);
  }
  
  node.clearResponseBuffer();
}

// Set the accelleration time for the motor.
// This typically isnt really necessary.
void set_motor_accel_time (int time) {
  uint8_t result;
  result = node.writeSingleRegister(0x00E6, time);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Accel Set.");
  } else {
    Serial.println("Error setting motor accel.");
  }
  
  node.clearResponseBuffer();
}