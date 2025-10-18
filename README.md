# CheckTextDisplay Library

    Arduino library for interfacing with CheckTextDisplay via UART

## Features

    UART Communication - Control displays using serial communication

    Stream Compatibility - Works with HardwareSerial, SoftwareSerial, and any Stream-based interface

    ESP32 Optimized - Built-in support for ESP32's multiple hardware UARTs

    Simple API - Easy-to-use text display methods

## Installation

#include <CheckTextDisplay.h>

CheckTextDisplay display;

void setup() {
    // Initialize with HardwareSerial (ESP32)
    display.begin(Serial2, 25, 26, Color::BLACK);
}

void loop() {
    display.print("Hello World!");
}

## API Initialization
cpp

// Core method - works with any Stream
bool begin(Stream &serial, Color bg);

// Convenience method for HardwareSerial
bool begin(HardwareSerial &serial, uint8_t RX, uint8_t TX, Color bg);

### Basic Operations
cpp

display.print("Text");      // Display text
display.clear();           // Clear display
display.setCursor(x, y);   // Set text position

### Wiring
ESP32 Connection
text

ESP32          Display
-----          -------
GPIO25 (RX) -> TX
GPIO26 (TX) -> RX
GND        -> GND
3.3V       -> VCC

### Examples

Basic Usage:
cpp

display.begin(Serial2, 25, 26, Color::BLACK);
display.print("Sensor: ");
display.println(analogRead(A0));

Stream Compatibility:
cpp

// Works with any Stream interface
display.begin(bluetoothSerial, Color::BLACK);

### License

MIT License - see LICENSE file for details.
