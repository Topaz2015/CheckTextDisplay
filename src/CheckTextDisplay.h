#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H

#include <Arduino.h>
#include <Stream.h>
// #include <SoftwareSerial.h>
// enum class FontSize { 
//     SMALL, 
//     MEDIUM, 
//     LARGE  
// };

enum class FontSize : uint8_t { 
    SMALL  = 14, 
    MEDIUM = 25, 
    LARGE  = 50
};
enum class Align  : uint8_t { 
    LEFT   = 0, 
    CENTER = 1, 
    RIGHT  = 2,
    MENU   = 3
};

enum class Color : uint8_t {
    RED     = 0b00000001,
    GREEN   = 0b00000010,
    BLUE    = 0b00000100,
    LIME    = 0b00000110, 
    WHITE   = 0b00000111,
    BLACK   = 0b00000000,
    YELLOW  = 0b00000011,
    PURPLE  = 0b00000101,
    NO_COLOR = 0b01111111
};

enum class NotificationArea { 
    TOP, 
    BOTTOM 
};

class TextDisplay {
public:
    // Initialization
    bool begin(Stream &serial, Color bg);
    bool begin(HardwareSerial &serial, uint8_t RX, uint8_t TX, Color bg); 
    // bool begin(SoftwareSerial &serial, uint8_t RX, uint8_t TX, Color bg);
    void reset();
    
    // Screen Management
    void clear();
    void clear(Color wallbg);
    void clearArea(uint8_t startRow, uint8_t rowCount);
    void clearNotificationArea(NotificationArea area);
    
    // Notification Areas
    void enableNotifications(bool top = true, bool bottom = true);
    void disableNotifications();
    
    // Text Printing
    void setCursor(uint8_t row, uint8_t col = 0);
    void setFontSize(FontSize size);
    void setTextColor(Color textFg, Color textBg);
    void setAlignment(Align pos);
    
    bool print(const String &text);
    bool println(const String &text);
    
    // Advanced Printing
    bool printAt(uint8_t row, uint8_t col, const String &text);
    
    // Advanced Printing
    bool printAt(uint8_t row, uint8_t col, const String &text, 
                 FontSize font,
                 Color fg, Color bg=Color::NO_COLOR);
    bool printFootNote(const String &text, Align pos, Color fg, Color bg=Color::NO_COLOR);
    
    bool printHeadNote(const String &text, Align pos, Color fg, Color bg=Color::NO_COLOR);
    // Scrolling
    void setScroll(NotificationArea section, const String &text, bool enable);
    
    // Utility
    uint8_t getMaxRows() const;           // Returns 16
    uint8_t getMaxCols(FontSize font) const; // 50, 28, or 14
    uint8_t getRowsConsumed(FontSize font) const; // 1, 2, or 4
    
    // Cursor position after last print
    uint8_t getCurrentRow() const;
    uint8_t getCurrentCol() const;

private:
    struct TextCell {
        char character = '\0';
        FontSize font = FontSize::SMALL;
        Color textFg = Color::WHITE;
        Color textBg = Color::BLACK;
        bool occupied = false;
    };
    
    // Internal state
    TextCell grid[50][16]; // 50 columns, 16 unified rows
    Stream* displaySerial = nullptr;
    
    uint8_t currentRow = 0;
    uint8_t currentCol = 0;
    FontSize currentFont = FontSize::SMALL;
    Color currentScreenBg = Color::LIME;
    Color currentFgColor = Color::WHITE;
    Color currentBgColor = Color::BLACK;
    Align currentAlign = Align::LEFT;
    
    bool notificationsTop = false;
    bool notificationsBottom = false;
    
    // Internal methods
    bool begin(Stream &serial);
    void screenOn();
    void screenOff();
    void sendCommand(const String &cmd);
    void clearGridArea(uint8_t startRow, uint8_t rowCount, uint8_t startCol = 0, uint8_t colCount = 50);
    bool isAreaClear(uint8_t startRow, uint8_t rowCount, uint8_t startCol, uint8_t colCount) const;
    void markAreaOccupied(uint8_t startRow, uint8_t rowCount, uint8_t startCol, uint8_t colCount, FontSize font);
    uint8_t fontToRows(FontSize font) const;
    uint8_t fontToCols(FontSize font) const;
    String colorToCode(Color color) const;
    String fontToCode(FontSize font) const;
    void setScreenBackground(Color screenBg);
    uint16_t calculateYPosition(uint8_t logicalRow) ;    
    uint16_t calculateXPosition(FontSize font, uint8_t logicalCol) ;
};

#endif