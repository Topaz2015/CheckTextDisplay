/*
	Author:Topaz 2025
	shows how to use the available font sizes.
*/

#include "TextDisplay.h"

TextDisplay display;
HardwareSerial mySerial(2);  // Create UART2 instance
#define disp_RX 25
#define disp_TX 26
int nextrow = 10;
void setup() {
    Serial.begin(115200);  
    while (!Serial) {
        delay(10);
    }
    display.begin(mySerial, disp_RX, disp_TX, Color::BLACK);
    Serial.println("ESP32-WROOM-32E with extra serial port ready!");
   
    display.setAlignment(Align::CENTER);
    display.printHeadNote("WELCOME TO OUR CLUB 20/=", Align::MENU,Color::BLUE);
    display.printAt(0, 0, "COMPANY NAME", FontSize::LARGE, Color::YELLOW, Color::NO_COLOR);
    display.setAlignment(Align::LEFT);
    display.printAt(4, 0, "MOTTO: STAY CALM, STAY COOL!!", FontSize::MEDIUM, Color::GREEN, Color::NO_COLOR);
    display.printAt(6, 0, "Line 1: Small font content", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(7, 0, "Line 2: More details here", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(8, 0, "Line 3: Testing 123", FontSize::SMALL, Color::LIME, Color::NO_COLOR);
    display.setAlignment(Align::RIGHT);
    display.printFootNote("TILL NO: 7833388", Align::CENTER,Color::PURPLE);
    display.setAlignment(Align::LEFT);
    nextrow = display.getCurrentRow();
}

void loop() {
    static unsigned long lastUpdate = 0;
    if (millis() - lastUpdate > 10000 && nextrow < display.getMaxRows()) {
        lastUpdate = millis();   
        display.printAt(nextrow, 0, "Updated Row: " + String(display.getCurrentRow()), 
                       FontSize::SMALL, Color::RED, Color::BLACK);
        nextrow++;
    }
    
    delay(100);
}
