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
    Initialize with HardwareSerial (ESP32)
    display.begin(Serial2, 25, 26, Color::BLACK);
}

void loop() {
    display.print("Hello World!");
}

## API Initialization
cpp

###Initialization

    begin(Stream &serial, Color bg) - Start with any Stream (Serial, SoftwareSerial)

    begin(HardwareSerial &serial, uint8_t RX, uint8_t TX, Color bg) - Start with hardware serial pins

    reset() - Reset display to default state

###Screen Management

    clear() - Clear entire screen with current background

    clear(Color wallbg) - Clear screen with specific color

    clearRow(int row) - Clear a single row

    clearHead() - Clear top notification area

    clearFoot() - Clear bottom notification area

    clearArea(startRow, rowCount) - Clear a section of rows

    setBackgroundColor(bg) - Change screen background color

###Notification Areas

    enableNotifications(top, bottom) - Show/hide top/bottom bars

    disableNotifications() - Hide both notification areas

###Text Settings

    setCursor(row, col) - Move cursor to position

    setFontSize(size) - Set font (SMALL, MEDIUM, LARGE)

    setTextColor(foreground, background) - Set text colors

    setAlignment(pos) - Set text alignment (LEFT, CENTER, RIGHT, MENU)

###Basic Printing

    print(text) - Print at current cursor position

    println(text) - Print and move to next line

###Advanced Printing

    printAt(row, col, text) - Print at specific position with current settings

    printAt(row, col, text, font, fg, bg) - Print with custom font and colors

    printHeadNote(text, alignment, fg, bg) - Print in top notification area

    printFootNote(text, alignment, fg, bg) - Print in bottom notification area

###Utility

    getMaxRows() - Returns 16 (total rows available)

    getMaxCols(font) - Returns characters per row for given font

    getRowsConsumed(font) - Returns how many rows a font uses

    getCurrentRow()/getCurrentCol() - Get current cursor position

####Color Options:

RED, GREEN, BLUE, LIME, WHITE, BLACK, YELLOW, PURPLE, NO_COLOR
####Font Sizes:

SMALL (14px), MEDIUM (25px), LARGE (50px)
####Text Alignment:

LEFT, CENTER, RIGHT, MENU

The class handles a 16-row display with top/bottom notification areas and supports mixed fonts and colors with automatic positioning.

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

Works with any Stream interface
display.begin(bluetoothSerial, Color::BLACK);

### License

MIT License - see LICENSE file for details.
