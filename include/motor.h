#include <Arduino.h>
// Include for ModbusMaster type 
#include <ModbusMaster.h>

#ifndef MOTOR_H
#define MOTOR_H

ModbusMaster& get_modbus_master_instance();

int parseInt (char buffer[]);

// // Initialise RE/DE pins before transmission
void preTransmission();

// Initialise RE/DE pins after transmission
void postTransmission();

// Declare setup function
void setup_modbus();

void fetch_vitals ();


uint16_t get_register_value (uint16_t reg);

void start_motor ();

void stop_motor ();

void set_motor_speed (int speed);

void set_motor_mode (int mode);

void set_motor_accel_time (int time);

#endif