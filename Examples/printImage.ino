/*
	Author:Topaz 2025
	shows how to print saved image on display.
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
    Serial.println("Started serial2 as display socket!");  
    // display.printHeadNote("PAYBILL: ", Align::LEFT,Color::LIME);
    // display.printHeadNote("7833388", Align::RIGHT,Color::LIME); 
    display.printHeadNote("STARTS AT: 10:30 PM", Align::MENU,Color::LIME);
    display.setAlignment(Align::CENTER);
    display.printAt(0, 0, "COMPANY NAME", FontSize::LARGE, Color::YELLOW, Color::NO_COLOR);
    display.setAlignment(Align::LEFT);
    display.printAt(4, 0, "MOTTO: STAY CALM, STAY COOL!!", FontSize::MEDIUM, Color::GREEN, Color::NO_COLOR);
    display.printAt(6, 0, "Line 1: Small font content", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(7, 0, "Line 2: More details here", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(8, 0, "Line 3: Testing 123", FontSize::SMALL, Color::LIME, Color::NO_COLOR);
    display.setAlignment(Align::RIGHT);

    //you can use MENU to auto-format text if number is on the right
    display.printFootNote("ONE LITRE AT: 50/=", Align::MENU,Color::LIME);
    //the next 2 lines let you manually print menu-like. best if a number is in the name eg below
    // display.printFootNote("1 LITRE AT: ", Align::LEFT,Color::LIME);
    // display.printFootNote("5/=", Align::RIGHT,Color::LIME);
    
    display.setAlignment(Align::LEFT);
    nextrow = display.getCurrentRow();
    // display.enableNotifications(true, false);
    delay(5000);
    display.setBackgroundColor(Color::YELLOW);
    // display.clear(Color::WHITE);
    display.clearHead();
    display.printHeadNote("PRINTED HEADER NOTIFICATION REGION LEFT", Align::LEFT,Color::RED);
    // display.printFootNote("PRINTED FOOTER NOTIFICATION REGION RIGHT", Align::RIGHT,Color::RED);
    display.setAlignment(Align::CENTER);
    delay(5000);
    Serial.println("End of setup!");   
    display.showImage("qr4.bin");
}

void loop() {}
