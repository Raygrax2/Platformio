#include "NibbleLCD_I2C.h"

void NibbleLCD_I2C::setup(SimpleI2C &i2c, uint8_t addr) {
    _i2c = &i2c;
    _addr = addr;

    // Wait >40ms after power-on
    vTaskDelay(pdMS_TO_TICKS(50));

    // Reset sequence for 4-bit mode
    sendNibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));
    sendNibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(5));
    sendNibble(0x03, false);
    vTaskDelay(pdMS_TO_TICKS(2));
    sendNibble(0x02, false); // 4-bit mode

    // Function set, display on, entry mode, clear
    writeCommand(0x28); // 4-bit, 2 lines
    writeCommand(0x0C); // display on, cursor off
    writeCommand(0x06); // entry mode
    writeCommand(0x01); // clear
    vTaskDelay(pdMS_TO_TICKS(2));
}

void NibbleLCD_I2C::pulse(uint8_t val) {
    uint8_t buf[1];
    buf[0] = val | (1 << LCD_EN);  // EN high
    _i2c->write(buf, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    buf[0] = val & ~(1 << LCD_EN); // EN low
    _i2c->write(buf, 1);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void NibbleLCD_I2C::sendNibble(uint8_t nibble, bool rs) {
    uint8_t val = _backlight;
    if(rs) val |= (1 << LCD_RS);
    if(nibble & 0x01) val |= (1 << LCD_D4);
    if(nibble & 0x02) val |= (1 << LCD_D5);
    if(nibble & 0x04) val |= (1 << LCD_D6);
    if(nibble & 0x08) val |= (1 << LCD_D7);
    pulse(val);
}

void NibbleLCD_I2C::sendByte(uint8_t byte, bool rs) {
    sendNibble(byte >> 4, rs);   // high nibble
    sendNibble(byte & 0x0F, rs); // low nibble
}

void NibbleLCD_I2C::writeCommand(uint8_t cmd) {
    sendByte(cmd, false);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void NibbleLCD_I2C::print(char c) {
    sendByte(c, true);
    vTaskDelay(pdMS_TO_TICKS(1));
}

void NibbleLCD_I2C::printStr(const char* str) {
    writeCommand(0x01); // clear
    writeCommand(0x02); // home
    while (*str) {
        if (*str == '\n') {
            writeCommand(0xC0); // second line
            str++;
        } else {
            print(*str++);
        }
    }
}
