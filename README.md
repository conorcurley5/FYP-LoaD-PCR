This project contains an implementation of ModbusMaster for the BLD305S motor driver.
Commands are sent to and from the host and client using Websockets.

Information on the BLD305S is available here: https://www.omc-stepperonline.com/index.php?route=product/product/get_file&file=4639/BLD-305S%20-%20Manual.pdf
Documentation for ModbusMaster is available here: https://github.com/4-20ma/ModbusMaster
Pinout guidance for Arduino (incompatible with ESP32) is available here: https://circuitdigest.com/microcontroller-projects/rs485-serial-communication-between-arduino-uno-and-arduino-nano

This project is developed using PlatformIO, which provides better tools for segmenting the application into different files, in my opinion.
You can either:
- Use PlatformIO
- Take the code from this project and paste into ArduinoIDE (both use C++)

Project files are in the /src directory, motor files are in /src/motor/motor.cpp.
