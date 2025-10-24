#ifndef __LCDI2C_H__
#define __LCDI2C_H__

#include <cstdint>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "simpleI2C.h"

class LCDI2C
{
public:
    LCDI2C();
    // use your SimpleI2C instance inside this class
    SimpleI2C i2c;

    // Setup will initialize the underlying I2C peripheral (via SimpleI2C)
    // and run the LCD init sequence.
    void setup(uint8_t device_address, uint32_t freq = 400000, uint8_t sda_pin = 4, uint8_t scl_pin = 5, i2c_port_t i2c_num = I2C_NUM_0);

    // common operations
    void home();
    void clear();
    void print(const char* str);
    void setCursor(uint8_t col, uint8_t row);
    void backlightOn();
    void backlightOff();
    void command(uint8_t value);
    void writeChar(char c);

private:
    uint8_t _addr;
    uint8_t _cols;
    uint8_t _rows;
    uint8_t _backlight_flag;

    // low-level helpers
    void pcfWrite(uint8_t data);
    void pulseEnable(uint8_t data);
    void write4bits(uint8_t nibble, uint8_t flags);
    void send(uint8_t value, bool isData);

    static constexpr const char* TAG = "LCDI2C";
};

#endif // __LCDI2C_H__
