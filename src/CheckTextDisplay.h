/*
	Author:Topaz 
    Date: Oct 2025
    TextDisplay
	Serial Command Library for use with CheckBox Digital Lightbox.
*/

#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H

#include <Arduino.h>
#include <Stream.h>

enum class FontSize : uint8_t { 
    SMALL  = 14, 
    MEDIUM = 25, 
    LARGE  = 50
};
enum class Align  : uint8_t { 
    LEFT   = 0, 
    CENTER = 1, 
    RIGHT  = 2,
    MENU   = 3      //text to the left, number to the right
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
    /*Simple begin. works with any hardware Serial port.
    Library does the initialization*/
    bool begin(HardwareSerial &serial, uint8_t RX, uint8_t TX, Color bg); 
    /*Start with a prepared Serial port initialized in your code.
    untested: can work even with software serial.
    bg becomes the background: currentScreenBg*/
    bool begin(Stream &serial, Color bg);
    void reset();
    
    // Screen Management
    /*Turn off display*/
    void screenOn();
    /*Turn on display*/
    void screenOff();
    /*clear() - Clear entire screen with current backgroundcurrentScreenBg*/
    void clear();
    /*clear(Color wallbg) - Clear screen with specific color*/
    void clear(Color wallbg);
    /*wifiOff() - Shut down wifi completely.
    To enable reset().*/
    void wifiOff();
    /*clearRow(int row) - Clear a single row*/
    bool clearRow(int row);
    /*clearHead() - Clear top notification area with currentScreenBg*/
    bool clearHead();
    /*clearFoot() - Clear bottom notification area with currentScreenBg*/
    bool clearFoot();
    /*setBackgroundColor(bg) - Change screen background color: currentScreenBg.
    please note this doesnt update screen immediately.
    Changes take effect on next screen update*/
    void setBackgroundColor(Color bg);
    
    // Notification Areas
    // void enableNotifications(bool top = true, bool bottom = true);
    // void disableNotifications();
    
    // Text Printing
    /*setCursor(row, col) - Move cursor to position*/
    void setCursor(uint8_t row, uint8_t col = 0);
    /*setFontSize(size) - Set font (SMALL, MEDIUM, LARGE)*/
    void setFontSize(FontSize size);
    /*setTextColor(foreground, background) - Set text colors
    NOTE: background only affects text area. rest of row background depends on currentScreenBg*/
    void setTextColor(Color textFg, Color textBg);
    /*setAlignment(pos) - Set text alignment (LEFT, CENTER, RIGHT, MENU)*/
    void setAlignment(Align pos);
    /*print(text) - Print at current cursor position*/
    bool print(const String &text);
    /*println(text) - Print and move to next line*/
    bool println(const String &text);
    
    // Higher level Printing - recommended

    /*printAt(row, col, text) - Print at specific position with current settings*/
    bool printAt(uint8_t row, uint8_t col, const String &text);    
    /*printAt(row, col, text, font, fg, bg) - Print with custom font and colors*/
    bool printAt(uint8_t row, uint8_t col, const String &text, 
                 FontSize font,
                 Color fg, Color bg=Color::NO_COLOR);
    /*printHeadNote(text, alignment, fg, bg) - Print in top notification area*/           
    bool printFootNote(const String &text, Align pos, Color fg, Color bg=Color::NO_COLOR);
    /*printFootNote(text, alignment, fg, bg) - Print in bottom notification area*/
    bool printHeadNote(const String &text, Align pos, Color fg, Color bg=Color::NO_COLOR);
    
    // Utility
    /*getMaxRows() - Returns 16 (total rows available)*/
    uint8_t getMaxRows() const;           // Returns 16
    /*getMaxCols(font) - Returns characters per row for given font*/
    uint8_t getMaxCols(FontSize font) const; // 50, 28, or 14
    /*getRowsConsumed(font) - Returns how many rows a font uses*/
    uint8_t getRowsConsumed(FontSize font) const; // 1, 2, or 4
    
    // Cursor position after last print
    /*getCurrentRow() - Get current row position after printing*/
    uint8_t getCurrentRow() const;
    /*getCurrentCol() - Get current cursor position*/
    uint8_t getCurrentCol() const;
    /*showImage() - Display image on the screen.
    NOTE: Image must be saved in display prior.
    
    IMAGE UPLOAD
    switch on display and scan for it in available ap servers.
    open 192.168.25.1/img on your browser.
    upload image.
    edit its position.
    click Preocess
    Click upload
    Enter name without extension (eg. image) and save the same somwhere
    Done!!
    Call it using library like: display.showImage("image");
    */
    bool showImage(const String &name);

private:
    struct TextCell {
        char character = '\0';
        FontSize font = FontSize::SMALL;
        Color textFg = Color::WHITE;
        Color textBg = Color::BLACK;
        bool occupied = false;
    };
    
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
    void sendCommand(const String &cmd);
    uint8_t fontToRows(FontSize font) const;
    uint8_t fontToCols(FontSize font) const;
    uint16_t calculateYPosition(uint8_t logicalRow) ;    
    uint16_t calculateXPosition(FontSize font, uint8_t logicalCol) ;
    void clearSection(int startY, int height);
};

#endif
