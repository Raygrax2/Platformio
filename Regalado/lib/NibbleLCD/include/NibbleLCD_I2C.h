#ifndef _NIBBLELCD_I2C_H
#define _NIBBLELCD_I2C_H

#include "SimpleI2C.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdint.h>

// LCD backpack bit mapping (adjust if needed)
#define LCD_RS 0  // P0
#define LCD_RW 1  // P1
#define LCD_EN 2  // P2
#define LCD_BL 3  // P3
#define LCD_D4 4  // P4
#define LCD_D5 5  // P5
#define LCD_D6 6  // P6
#define LCD_D7 7  // P7

class NibbleLCD_I2C {
public:
    NibbleLCD_I2C() {}
    void setup(SimpleI2C &i2c, uint8_t addr);
    void writeCommand(uint8_t cmd);
    void print(char c);
    void printStr(const char* str);

private:
    void pulse(uint8_t val);
    void sendNibble(uint8_t nibble, bool rs);
    void sendByte(uint8_t byte, bool rs);

    SimpleI2C *_i2c;
    uint8_t _addr;
    uint8_t _backlight = (1 << LCD_BL);
};

#endif
