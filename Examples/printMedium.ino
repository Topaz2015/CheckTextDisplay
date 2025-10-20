/*
	Author:Topaz 2025
	shows how to use the medium font size.
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
    display.printHeadNote("PAYBILL: ", Align::LEFT,Color::LIME);
    display.printHeadNote("7833388", Align::RIGHT,Color::LIME);
    display.setAlignment(Align::CENTER);
    display.printAt(0, 0, "COMPANY NAME", FontSize::LARGE, Color::YELLOW, Color::NO_COLOR);
    display.setAlignment(Align::LEFT);
    display.printAt(4, 0, "MOTTO: STAY CALM, STAY COOL!!", FontSize::MEDIUM, Color::GREEN, Color::NO_COLOR);
    display.printAt(6, 0, "Line 1: Small font content", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(7, 0, "Line 2: More details here", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(8, 0, "Line 3: Testing 123", FontSize::SMALL, Color::LIME, Color::NO_COLOR);
    display.setAlignment(Align::RIGHT);
    display.printFootNote("1 LITRE AT: ", Align::LEFT,Color::LIME);
    display.printFootNote("5/=", Align::RIGHT,Color::LIME);
    // display.printFootNote("TILL NO: 7833388", Align::CENTER,Color::PURPLE);
    display.setAlignment(Align::LEFT);
    nextrow = display.getCurrentRow();
    delay(5000);
    display.clear(Color::WHITE);
    display.printHeadNote("PRINTED HEADER NOTIFICATION REGION LEFT", Align::LEFT,Color::RED);
    display.printFootNote("PRINTED FOOTER NOTIFICATION REGION RIGHT", Align::RIGHT,Color::RED);
    display.setAlignment(Align::CENTER);
}

void loop() {
    static unsigned long lastUpdate = 0, newrow=0;
    if (millis() - lastUpdate > 5000 && nextrow < display.getMaxRows()) {
        lastUpdate = millis();   
        display.printAt(newrow, 0, "Updated Row: " + String(newrow), 
                       FontSize::MEDIUM, Color::RED, Color::BLACK);
        newrow+=2;
    }    
    delay(100);
}
