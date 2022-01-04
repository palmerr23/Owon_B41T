# Owon_B41T
An ESP32 dongle for Owon B series multimeters and a TestController definition file

The Owon B series multimeters use BLE GATT to transmit messages. The reading is transmitted as a signed magnitude binary number. There is an excellent discussion of the protocol at https://github.com/DeanCording/owonb35.

These multimeters also require a CC2450 based BLE dongle to communicate with a PC.

Together these factors make it very difficult to use these multimeters on a PC.

This project includes code for an ESP32 based dongle that should work with any ESP32 model that has a USB port. It has been tested with an ESP32-WROVER DevkitC.

The .txt file is an instrument definition file for TestController. https://lygte-info.dk/project/TestControllerScriptingExample%20UK.html
