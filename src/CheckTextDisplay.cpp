#include "TextDisplay.h"

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

void TextDisplay::wifiOff() {
    String cmd = setCommand("WIFI_OFF", 1);
    sendCommand("[" + cmd + "]");
    delay(100); // Allow display to reset
}

void TextDisplay::clear() {
    String cmd = setCommand("WALL_BG", static_cast<uint8_t>(currentScreenBg));
    sendCommand("[" + cmd + "]");
    currentRow = 0;
    currentCol = 0;
}

void TextDisplay::clear(Color wallbg) {
    currentScreenBg = wallbg;
    String cmd = setCommand("WALL_BG", static_cast<uint8_t>(currentScreenBg));
    sendCommand("[" + cmd + "]");
    currentRow = 0;
    currentCol = 0;
}

// void TextDisplay::enableNotifications(bool top, bool bottom) {
//     notificationsTop = top;
//     notificationsBottom = bottom;
//     String cmd = setCommand("W_TOP", (top)? 1: 0);
//     cmd += setCommand("W_BTM", (bottom)? 1: 0);
//     sendCommand("[" + cmd + "]");
// }

// void TextDisplay::disableNotifications() {
//     enableNotifications(false, false);
// }

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

bool  TextDisplay::showImage(const String &name) {
    String file = "";
    if (!name.indexOf(".") > -1) file = name.substring(0, name.indexOf("."));
    file += ".bin";
    sendCommand("[CMD_IMG=1]" + file);
    return true;
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
    String cmds = "";
    cmds += setCommand("XPOS", calculateXPosition(currentFont, col));
    cmds += setCommand("YPOS", calculateYPosition(row));
    cmds += setCommand("FONT", static_cast<uint8_t>(currentFont)); 
    cmds += setCommand("CHAR_FG", static_cast<uint8_t>(currentFgColor));
    cmds += setCommand("CHAR_BG", static_cast<uint8_t>(currentBgColor));
    cmds += setCommand("ALIGN", static_cast<uint8_t>(currentAlign));
    sendCommand("[" + cmds + "]" + text);   
    currentRow = row;
    currentCol = col + text.length();
    if (currentCol >= fontToCols(currentFont)) {
        currentRow += rowsNeeded;
        currentCol = 0;
    }    
    return true;
}


void TextDisplay::clearSection(int startY, int height){
    String cmds = "";
    cmds += setCommand("YPOS", startY);
    cmds += setCommand("HGT", height);
    cmds += setCommand("CLR_SECT", static_cast<uint8_t>(currentScreenBg));
    cmds = "[" + cmds + "]";
    sendCommand(cmds);
}

bool TextDisplay::clearRow(int row) {
    if (row >= 16) {
        return false;
    }
    String cmds = "";
    int startY = calculateYPosition(row);
    clearSection(startY, 16);
    currentRow = 0;
    currentCol = 0;
    return true;
}

bool TextDisplay::clearHead() {
    String cmds = "";
    clearSection(0, 22);
    return true;
}
bool TextDisplay::clearFoot() {
    String cmds = "";
    clearSection(278, 22);
    return true;
}

void TextDisplay::setBackgroundColor(Color bg){
    currentScreenBg = bg;
}

bool TextDisplay::printAt(uint8_t row, uint8_t col, const String &text, 
                         FontSize font, Color fg, Color bg) {
    if (row >= 16 || col >= fontToCols(font)) {
        return false;
    }
    uint8_t rowsNeeded = fontToRows(font);
    uint8_t colsAvailable = fontToCols(font) - col;
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
    cmds += setCommand("ALIGN", static_cast<uint8_t>(pos));
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
    

// void TextDisplay::setScroll(NotificationArea section, const String &text, bool enable) {
//     uint16_t yPos = 0;
//     if (section == NotificationArea::BOTTOM) yPos = 300-22; 
//     String cmd = setCommand("SCROLL", (enable)? 1:0);
//     cmd += setCommand("ROW", yPos);
//     cmd = "[" + cmd + "]" + text;
//     sendCommand(cmd);
// }

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
