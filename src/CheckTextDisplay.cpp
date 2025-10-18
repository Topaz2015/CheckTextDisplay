#include "CheckTextDisplay.h"

bool TextDisplay::begin(Stream &serial) {
    displaySerial = &serial;
    reset();
    delay(10000);
    return true;
} 

bool TextDisplay::begin(Stream &serial, Color bg) {
    displaySerial = &serial;
    clear(bg);
    delay(5000);
    return true;
} 

bool TextDisplay::begin(HardwareSerial &serial, uint8_t RX, uint8_t TX, Color bg) {
    serial.begin(9600, SERIAL_8N1, RX, TX);
    while (!serial) {
        delay(10);
    }
    return begin(serial, bg); // Just pass the reference directly
}


// bool TextDisplay::begin(SoftwareSerial &serial, uint8_t RX, uint8_t TX, Color bg) {
//     serial.begin(9600);  // SoftwareSerial only needs baud rate
//     // No "while (!serial)" needed for SoftwareSerial
//     return begin(serial, bg);
// }

String setCommand(String cmd, int val) {
    String res = cmd + "=";
    res += String(val) + ";";
    return res;
}

void TextDisplay::reset() {
    String cmd = setCommand("SCREEN", 2);
    sendCommand("[" + cmd + "]");
    delay(100); // Allow display to reset
}

void TextDisplay::screenOn() {
    String cmd = setCommand("SCREEN", 1);
    sendCommand("[" + cmd + "]");
    delay(100); // Allow display to reset
}

void TextDisplay::screenOff() {
    String cmd = setCommand("SCREEN", 0);
    sendCommand("[" + cmd + "]");
    delay(100); // Allow display to reset
}

void TextDisplay::clear() {
    String cmd = setCommand("WALL_BG", static_cast<uint8_t>(currentScreenBg));
    sendCommand("[" + cmd + "]");
    // Clear internal grid state
    for (uint8_t row = 0; row < 16; row++) {
        for (uint8_t col = 0; col < 50; col++) {
            grid[col][row] = TextCell();
        }
    }
    currentRow = 0;
    currentCol = 0;
}

void TextDisplay::clear(Color wallbg) {
    currentScreenBg = wallbg;
    String cmd = setCommand("WALL_BG", static_cast<uint8_t>(currentScreenBg));
    sendCommand("[" + cmd + "]");
    // Clear internal grid state
    for (uint8_t row = 0; row < 16; row++) {
        for (uint8_t col = 0; col < 50; col++) {
            grid[col][row] = TextCell();
        }
    }
    currentRow = 0;
    currentCol = 0;
}

void TextDisplay::clearArea(uint8_t startRow, uint8_t rowCount) {
    if (startRow >= 16 || rowCount == 0) return;    
    uint8_t endRow = min(startRow + rowCount, 16);
    for (uint8_t row = startRow; row < endRow; row++) {
        for (uint8_t col = 0; col < 50; col++) {
            grid[col][row] = TextCell();
        }
    }    
    // Send clear command for physical area
    uint16_t y = calculateYPosition(startRow);
    uint16_t height = rowCount * 16; // Each logical row = 16px
    //send background color here
    String cmd = setCommand("CLEAR", static_cast<uint8_t>(currentScreenBg));
    cmd += setCommand("YPOS", y);
    cmd += setCommand("PIX", height);
    sendCommand("[" + cmd + "]");
}

void TextDisplay::clearNotificationArea(NotificationArea area) {
    // String cmd = area == NotificationArea::TOP ? "[CLRTOP]" : "[CLRBOT]";
    String cmd = (area == NotificationArea::TOP)? setCommand("W_TOP", 0) : setCommand("W_BTM", 0);
    sendCommand("[" + cmd + "]");
}

void TextDisplay::enableNotifications(bool top, bool bottom) {
    notificationsTop = top;
    notificationsBottom = bottom;
    String cmd = setCommand("W_TOP", (top)? 1: 0);
    cmd += setCommand("W_BTM", (bottom)? 1: 0);
    sendCommand("[" + cmd + "]");
}

void TextDisplay::disableNotifications() {
    enableNotifications(false, false);
}

void TextDisplay::setCursor(uint8_t row, uint8_t col) {
    if (row >= 16) return;
    currentRow = row;
    currentCol = col;
}

void TextDisplay::setFontSize(FontSize size) {
    currentFont = size;
}

void TextDisplay::setTextColor(Color fg, Color bg) {
    currentFgColor = fg;
    currentBgColor = bg;
}

void TextDisplay::setScreenBackground(Color bg){
    currentScreenBg = bg;
    clear();
}

bool TextDisplay::print(const String &text) {
    return printAt(currentRow, currentCol, text, currentFont, currentFgColor, currentBgColor);
}

void TextDisplay::setAlignment(Align pos) {
    currentAlign = pos;
}

bool TextDisplay::println(const String &text) {
    bool success = print(text);
    if (success) {
        currentRow += fontToRows(currentFont);
        currentCol = 0;
    }
    return success;
}

bool TextDisplay::printAt(uint8_t row, uint8_t col, const String &text) {
    if (row >= 16 || col >= fontToCols(currentFont)) {
        return false;
    }        
    uint8_t rowsNeeded = fontToRows(currentFont);
    uint8_t colsAvailable = fontToCols(currentFont) - col;
    // Check if target area is clear
    if (!isAreaClear(row, rowsNeeded, col, min((uint8_t)text.length(), colsAvailable))) {
        return false; // Area occupied, prevent overlap
    }
    String cmds = "";
    cmds += setCommand("XPOS", calculateXPosition(currentFont, col));
    cmds += setCommand("YPOS", calculateYPosition(row));
    cmds += setCommand("FONT", static_cast<uint8_t>(currentFont)); 
    cmds += setCommand("CHAR_FG", static_cast<uint8_t>(currentFgColor));
    cmds += setCommand("CHAR_BG", static_cast<uint8_t>(currentBgColor));
    cmds += setCommand("ALIGN", static_cast<uint8_t>(currentAlign));
    // String displayCmd = "[" + cmds + "]" + text;
    sendCommand("[" + cmds + "]" + text);
    markAreaOccupied(row, rowsNeeded, col, text.length(), currentFont);    
    currentRow = row;
    currentCol = col + text.length();
    if (currentCol >= fontToCols(currentFont)) {
        currentRow += rowsNeeded;
        currentCol = 0;
    }    
    return true;
}

bool TextDisplay::printAt(uint8_t row, uint8_t col, const String &text, 
                         FontSize font, Color fg, Color bg) {
    if (row >= 16 || col >= fontToCols(font)) {
        return false;
    }
    uint8_t rowsNeeded = fontToRows(font);
    uint8_t colsAvailable = fontToCols(font) - col;
    // Check if target area is clear
    if (!isAreaClear(row, rowsNeeded, col, min((uint8_t)text.length(), colsAvailable))) {
        return false; // Area occupied, prevent overlap
    }
    if (bg == Color::NO_COLOR) bg = currentScreenBg;
    if (fg == Color::NO_COLOR) fg = currentFgColor; //dont allow bg = fg
    setFontSize(font);
    setTextColor(fg, bg);
    String cmds = "";
    cmds += setCommand("XPOS", calculateXPosition(currentFont, col));
    cmds += setCommand("YPOS", calculateYPosition(row));
    cmds += setCommand("FONT", static_cast<uint8_t>(currentFont)); 
    cmds += setCommand("CHAR_FG", static_cast<uint8_t>(currentFgColor));
    cmds += setCommand("CHAR_BG", static_cast<uint8_t>(currentBgColor));
    cmds += setCommand("ALIGN", static_cast<uint8_t>(currentAlign));
    cmds = "[" + cmds + "]" + text;
    sendCommand(cmds);
    markAreaOccupied(row, rowsNeeded, col, text.length(), currentFont);    
    currentRow = row;
    currentCol = col + text.length();
    if (currentCol >= fontToCols(currentFont)) {
        currentRow += rowsNeeded;
        currentCol = 0;
    }    
    return true;
}

bool TextDisplay::printFootNote(const String &text, Align pos, Color fg, Color bg){
    if ((uint8_t)text.length() > 49) {
        return false; // Too many chars
    }
    if (bg == Color::NO_COLOR) bg = currentScreenBg;
    if (fg == Color::NO_COLOR) fg = currentFgColor; //dont allow bg = fg
    String cmds = "";
    cmds += setCommand("FOOT_FG", static_cast<uint8_t>(fg));
    cmds += setCommand("FOOT_BG", static_cast<uint8_t>(bg));
    cmds += setCommand("ALIGN", static_cast<uint8_t>(currentAlign));
    cmds = "[" + cmds + "]" + text;
    sendCommand(cmds);    
    return true;
}
    
bool TextDisplay::printHeadNote(const String &text, Align pos, Color fg, Color bg){
    if ((uint8_t)text.length() > 49) {
        return false; // Too many chars
    }
    if (bg == Color::NO_COLOR) bg = currentScreenBg;
    if (fg == Color::NO_COLOR) fg = currentFgColor; //dont allow bg = fg
    String cmds = "";
    cmds += setCommand("HEAD_FG", static_cast<uint8_t>(fg));
    cmds += setCommand("HEAD_BG", static_cast<uint8_t>(bg));
    cmds += setCommand("ALIGN", static_cast<uint8_t>(pos));
    cmds = "[" + cmds + "]" + text;
    sendCommand(cmds);    
    return true;
}
    

void TextDisplay::setScroll(NotificationArea section, const String &text, bool enable) {
    // if (row >= 16) return;    
    uint16_t yPos = 0;
    if (section == NotificationArea::BOTTOM) yPos = 300-22; 
    String cmd = setCommand("SCROLL", (enable)? 1:0);
    cmd += setCommand("ROW", yPos);
    cmd = "[" + cmd + "]" + text;
    sendCommand(cmd);
}

uint8_t TextDisplay::getMaxRows() const {
    return 16;
}

uint8_t TextDisplay::getMaxCols(FontSize font) const {
    return fontToCols(font);
}

uint8_t TextDisplay::getRowsConsumed(FontSize font) const {
    return fontToRows(font);
}

uint8_t TextDisplay::getCurrentRow() const {
    return currentRow;
}

uint8_t TextDisplay::getCurrentCol() const {
    return currentCol;
}

// Private implementation
void TextDisplay::sendCommand(const String &cmd) {
    bool res = false;
    String rcvd = "";
    int del = 10000, len = cmd.length();
    if (displaySerial) {
        // displaySerial->println(cmd);
        for (int i=0; i<len; i++){
            displaySerial->print(cmd[i]);
            delayMicroseconds(10);
        }
        displaySerial->println();
    }    
}
// // Private implementation
// void TextDisplay::sendCommand(const String &cmd) {
//     bool res = false;
//     String rcvd = "";
//     int del = 10000, len = cmd.length();
//     // if (displaySerial) {
//     //     displaySerial->println(cmd);
//     // }    
//     if (displaySerial) {
//         displaySerial->readString();
//         while (rcvd.length() < len) {
//             // displaySerial->println(cmd);
//             for (int i=0; i<len; i++){
//                 displaySerial->print(cmd[i]);
//                 delayMicroseconds(10);
//             }
//             delay(10);
//             if (displaySerial->available()){
//                 char ch = displaySerial->read();
//                 rcvd += ch;
//                 if (ch == '\n') {
//                     res = true;
//                     break;
//                 }
//                 del--;
//                 if (del <= 0) break;
//                 delayMicroseconds(10);
//             }
//         }
//     } 
//     // while (displaySerial->available()) {
//     //     char ch = displaySerial->read();
//     //     rcvd += ch;
//     //     if (ch == '\n') {
//     //         res = true;
//     //         break;
//     //     }
//     //     del--;
//     //     if (del <= 0) break;
//     //     delayMicroseconds(10);
//     // }
//     Serial.println();
//     Serial.println("CMD SENT:"+cmd);
//     Serial.println("CMD LEN:"+String(len));
//     if (res == false) Serial.println("SORRY TIMED OUT");
//     else {
//         Serial.println("RES LEN:"+String(rcvd.length()));
//         Serial.println("RES RCVD:"+rcvd);
//     }    
//     delay(100);
// }

void TextDisplay::clearGridArea(uint8_t startRow, uint8_t rowCount, uint8_t startCol, uint8_t colCount) {
    uint8_t endRow = min(startRow + rowCount, 16);
    uint8_t endCol = min(startCol + colCount, 50);
    
    for (uint8_t row = startRow; row < endRow; row++) {
        for (uint8_t col = startCol; col < endCol; col++) {
            grid[col][row] = TextCell();
        }
    }
}

bool TextDisplay::isAreaClear(uint8_t startRow, uint8_t rowCount, uint8_t startCol, uint8_t colCount) const {
    uint8_t endRow = min(startRow + rowCount, 16);
    uint8_t endCol = min(startCol + colCount, 50);
    
    for (uint8_t row = startRow; row < endRow; row++) {
        for (uint8_t col = startCol; col < endCol; col++) {
            if (grid[col][row].occupied) {
                return false;
            }
        }
    }
    return true;
}

void TextDisplay::markAreaOccupied(uint8_t startRow, uint8_t rowCount, uint8_t startCol, uint8_t colCount, FontSize font) {
    uint8_t endRow = min(startRow + rowCount, 16);
    uint8_t endCol = min(startCol + colCount, 50);
    
    for (uint8_t row = startRow; row < endRow; row++) {
        for (uint8_t col = startCol; col < endCol; col++) {
            grid[col][row].occupied = true;
            grid[col][row].font = font;
        }
    }
}

uint8_t TextDisplay::fontToRows(FontSize font) const {
    switch(font) {
        case FontSize::SMALL: return 1;
        case FontSize::MEDIUM: return 2;
        case FontSize::LARGE: return 4;
        default: return 1;
    }
}

uint8_t TextDisplay::fontToCols(FontSize font) const {
    switch(font) {
        case FontSize::SMALL: return 50;
        case FontSize::MEDIUM: return 28;
        case FontSize::LARGE: return 14;
        default: return 50;
    }
}

// String TextDisplay::colorToCode(Color color) const {
//     // int code = 
//     switch(color) {
//         case Color::BLACK: return "BLK";
//         case Color::WHITE: return "WHT";
//         case Color::RED: return "RED";
//         case Color::GREEN: return "GRN";
//         case Color::BLUE: return "BLU";
//         case Color::YELLOW: return "YLW";
//         case Color::LIME: return "AQU";
//         case Color::PURPLE: return "PUR";
//         default: return "WHT";
//     }
// }

// String TextDisplay::fontToCode(FontSize font) const {
//     switch(font) {
//         case FontSize::SMALL: return "S";
//         case FontSize::MEDIUM: return "M";
//         case FontSize::LARGE: return "L";
//         default: return "S";
//     }
// }

uint16_t TextDisplay::calculateYPosition(uint8_t logicalRow) {
    // First usable row starts at y=22, each logical row = 16px
    return 22 + (logicalRow * 16);
}
uint16_t TextDisplay::calculateXPosition(FontSize font, uint8_t logicalCol) {
    switch (font) {
        case FontSize::LARGE: return 4 + (28 * logicalCol);
        case FontSize::MEDIUM: return 4 + (14 * logicalCol);
        default : return 4 + (8 * logicalCol);
    }
    return 4 + (8 * logicalCol);
}