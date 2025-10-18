#ifndef LCDI2C_H
#define LCDI2C_H

#include <cstdint>
#include <string>
#include "SimpleI2C.h"

// HD44780 commands
#define LCD_CLEARDISPLAY 0x01
#define LCD_RETURNHOME   0x02
#define LCD_ENTRYMODESET 0x04
#define LCD_DISPLAYCONTROL 0x08
#define LCD_CURSORSHIFT  0x10
#define LCD_FUNCTIONSET  0x20
#define LCD_SETCGRAMADDR 0x40
#define LCD_SETDDRAMADDR 0x80

// flags
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00

#define LCD_DISPLAYON  0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON   0x02
#define LCD_CURSOROFF  0x00
#define LCD_BLINKON    0x01
#define LCD_BLINKOFF   0x00

#define LCD_DISPLAYMOVE 0x08
#define LCD_MOVERIGHT   0x04
#define LCD_MOVELEFT    0x00

#define LCD_8BITMODE 0x10
#define LCD_4BITMODE 0x00
#define LCD_2LINE    0x08
#define LCD_1LINE    0x00
#define LCD_5x10DOTS 0x04
#define LCD_5x8DOTS  0x00

#define LCD_BACKLIGHT   0x08
#define LCD_NOBACKLIGHT 0x00

class LCDI2C {
public:
    // i2c: reference to your SimpleI2C instance (already setup)
    // lcd_addr: 7-bit PCF8574 address (0x27 or 0x3F commonly)
    LCDI2C(SimpleI2C &i2c, uint8_t lcd_addr, uint8_t lcd_cols = 16, uint8_t lcd_rows = 2);

    // initialize (call after SimpleI2C::setup)
    void init();                    // compatibility
    void begin(uint8_t cols = 16, uint8_t rows = 2, uint8_t dotsize = LCD_5x8DOTS);

    // high level
    void clear();
    void home();
    void setCursor(uint8_t col, uint8_t row);
    void noDisplay();
    void display();
    void noCursor();
    void cursor();
    void noBlink();
    void blink();
    void scrollDisplayLeft();
    void scrollDisplayRight();
    void leftToRight();
    void rightToLeft();
    void autoscroll();
    void noAutoscroll();

    // custom char
    void createChar(uint8_t location, const uint8_t charmap[8]);

    // backlight
    void noBacklight();
    void backlight();
    void setBacklight(uint8_t value);

    // output
    size_t write(uint8_t value);               // single char
    size_t print(const char *str);
    size_t print(const std::string &s);

    // low level command
    void command(uint8_t value);

private:
    SimpleI2C &_i2c;
    uint8_t _Addr;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _backlightval;
    uint8_t _displayfunction;
    uint8_t _displaycontrol;
    uint8_t _displaymode;
    uint8_t _numlines;
    bool _oled = false;

    // PCF8574 mapping (common backpack)
    static constexpr uint8_t En = 0x04;  // P2
    static constexpr uint8_t Rw = 0x02;  // P1 (we keep low)
    static constexpr uint8_t Rs = 0x01;  // P0

    // low-level helpers
    void send(uint8_t value, uint8_t mode);
    void write4bits(uint8_t value);
    void expanderWrite(uint8_t data);
    void pulseEnable(uint8_t data);
    void delayMicroseconds(uint32_t us);
};

#endif // LCDI2C_H
