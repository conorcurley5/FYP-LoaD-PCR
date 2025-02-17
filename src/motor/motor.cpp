// Include ModbusMaster library
#include <ModbusMaster.h>
#include <Arduino_JSON.h>
#include <websocket.h>

// // Define Modbus Pins
#define MAX485_DE      10
#define MAX485_RE_NEG  11

ModbusMaster node;


ModbusMaster& get_modbus_master_instance()
{
    return node;
}

const int num_vitals = 5;

// Define vitals register addresses
const uint16_t vitals_registers[num_vitals] = {
    0x005F,
    0x0066,
    0x0076,
    0x0096,
    0x00C6
};

// // Define vitals register names
const char* register_titles[num_vitals] = {
    "ACT_MTR_SPD",
    "MTR_RUN_QRY",
    "MTR_FLT_CDE",
    "DRV_INT_TMP",
    "ACT_MTR_VLT"
};

// uint16_t vitals[] = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000};

int parseInt (char buffer[]) {
  int num = 0;

  for (int i = 0; buffer[i] != '\0'; i++) {
    num = num * 10 + (buffer[i] - '0');  // Convert each character to an integer and accumulate
  }

  return num;
}

// // Initialise RE/DE pins before transmission
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
  Serial1.begin(9600);

  pinMode(MAX485_RE_NEG, OUTPUT);
  pinMode(MAX485_DE, OUTPUT);
  digitalWrite(MAX485_RE_NEG, 1);
  digitalWrite(MAX485_DE, 1);

  node.begin(0x01, Serial1);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}


uint16_t fetch_value (uint16_t reg) {
  uint16_t result;
  result = node.readHoldingRegisters(reg, 1);
  uint16_t response = 0x00;

  if (result == node.ku8MBSuccess) {
    response = node.getResponseBuffer(0);
  }

  node.clearResponseBuffer();
  return response;
} 

void fetch_vitals () {
  JSONVar vitals_obj;
  uint16_t vitals[num_vitals];

  vitals_obj["type"] = "MOTOR_VITALS";

  for (int i = 0; i < num_vitals; i++) {
    vitals_obj[register_titles[i]] = (int)fetch_value(vitals_registers[i]);
    delay(200);
  }

  
  String v = JSON.stringify(vitals_obj);

  notify_clients(v);
}

void start_motor () {
  uint8_t result;
  result = node.writeSingleRegister(0x0066, 1);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Start Success.");
  } else {
    Serial.println("Error starting motor rotation.");
  }

  node.clearResponseBuffer();
}

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

void set_motor_speed (int speed) {
  uint8_t result;
  result = node.writeSingleRegister(0x0056, speed);

  if (result == node.ku8MBSuccess) {
    Serial.println("Motor Speed Set.");
  } else {
    Serial.println("Error setting motor speed.");
  }
  
  node.clearResponseBuffer();
}

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