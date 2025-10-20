/*
	Author:Topaz 2025
	shows how to perform random align.
    similar method can be used to get random color.
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
    Serial.println("Printed large font!");   
    display.printAt(4, 0, "MOTTO: STAY CALM, STAY COOL!!", FontSize::MEDIUM, Color::GREEN, Color::NO_COLOR);
    Serial.println("Printed medium font!");   
    display.printAt(6, 0, "Line 1: Small font content", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(7, 0, "Line 2: More details here", FontSize::SMALL, Color::WHITE, Color::NO_COLOR);
    display.printAt(8, 0, "Line 3: Testing 123", FontSize::SMALL, Color::LIME, Color::NO_COLOR);
    display.setAlignment(Align::RIGHT);
    Serial.println("Printed small font!");   

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
    Serial.println("End of setup!");   
}

void loop() {
    static unsigned long lastUpdate = 0;
    static int newrow=0;
    if (millis() - lastUpdate > 5000) {
        lastUpdate = millis();   
        display.clearRow(newrow);
        Align position = static_cast<Align>(random(4));
        display.setAlignment(position);
        display.printAt(newrow, 0, "Updated Row: " + String(newrow), 
                       FontSize::SMALL, Color::BLUE);
        newrow++;
        if (newrow == 16) {
            display.clearFoot();
            display.printFootNote("PRINTED FOOTER NOTIFICATION REGION RIGHT", Align::RIGHT, Color::RED);
        }
        else if (newrow == 17) display.screenOff();
        else if (newrow == 25) display.screenOn();   
        //FROM 25 TO 50 > DISPLAY IS ON BUT NO UART TRIGGERS //SCREEN SAVER MODE
        else if (newrow == 50) ESP.restart();
        Serial.printf("newrow:%d\n", newrow);
    }    
    delay(100);
}
