#include "LCDI2C.h"

// Typical PCF8574 bit mapping - adjust if your board differs
#define LCD_MASK_RS        0x01 // P0
#define LCD_MASK_RW        0x02 // P1
#define LCD_MASK_EN        0x04 // P2
#define LCD_MASK_BACKLIGHT 0x08 // P3
// P4..P7 -> DB4..DB7

LCDI2C::LCDI2C()
    : _addr(0x27), _cols(16), _rows(2), _backlight_flag(LCD_MASK_BACKLIGHT)
{
}

void LCDI2C::setup(uint8_t device_address, uint32_t freq, uint8_t sda_pin, uint8_t scl_pin, i2c_port_t i2c_num)
{
    _addr = device_address;
    // initialize underlying I2C device using your SimpleI2C API
    // SimpleI2C::setup expects device_address as first param per your header
    i2c.setup(device_address, freq, sda_pin, scl_pin, i2c_num);

    ESP_LOGI(TAG, "LCDI2C: init addr=0x%02X, sda=%u scl=%u freq=%u", device_address, sda_pin, scl_pin, (unsigned)freq);

    // Wait for LCD to power up (>40ms)
    vTaskDelay(pdMS_TO_TICKS(50));

    // Initialize to 4-bit mode:
    // Per HD44780 init procedure: send 0x03 three times, then 0x02
    write4bits(0x03, 0); vTaskDelay(pdMS_TO_TICKS(5));  // >4.1ms
    write4bits(0x03, 0); vTaskDelay(pdMS_TO_TICKS(5));  // >4.1ms
    write4bits(0x03, 0); vTaskDelay(pdMS_TO_TICKS(1));
    write4bits(0x02, 0); // set 4-bit mode

    // Function set: 4-bit, 2 lines, 5x8 dots
    command(0x28);

    // Display control: display on, cursor off, blink off
    command(0x0C);

    // Clear display
    clear();

    // Entry mode: cursor increments, no shift
    command(0x06);

    // keep backlight as previously set
    backlightOn();
}

void LCDI2C::clear()
{
    command(0x01);
    vTaskDelay(pdMS_TO_TICKS(2)); // clear command > 1.52ms
}

void LCDI2C::home()
{
    command(0x02);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void LCDI2C::setCursor(uint8_t col, uint8_t row)
{
    static const uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row >= _rows) row = _rows - 1;
    uint8_t addr = col + row_offsets[row];
    command(0x80 | addr);
}

void LCDI2C::print(const char* str)
{
    while (str && *str) {
        writeChar(*str++);
    }
}

void LCDI2C::backlightOn()
{
    _backlight_flag = LCD_MASK_BACKLIGHT;
    // write a no-op to update the state
    pcfWrite(_backlight_flag);
}

void LCDI2C::backlightOff()
{
    _backlight_flag = 0;
    pcfWrite(0);
}

void LCDI2C::command(uint8_t value)
{
    send(value, false);
}

void LCDI2C::writeChar(char c)
{
    send((uint8_t)c, true);
}

/*** low-level helpers ***/
void LCDI2C::send(uint8_t value, bool isData)
{
    uint8_t flags = _backlight_flag | (isData ? LCD_MASK_RS : 0);
    // high nibble
    write4bits((value >> 4) & 0x0F, flags);
    // low nibble
    write4bits(value & 0x0F, flags);
}

void LCDI2C::write4bits(uint8_t nibble, uint8_t flags)
{
    // map nibble to P4..P7 (shift left 4)
    uint8_t data = (nibble << 4) | flags;
    pcfWrite(data);
    pulseEnable(data);
}

void LCDI2C::pulseEnable(uint8_t data)
{
    // EN high
    pcfWrite(data | LCD_MASK_EN);
    // small delay to latch (1ms is safe; you may reduce to microseconds)
    vTaskDelay(pdMS_TO_TICKS(1));
    // EN low
    pcfWrite(data & ~LCD_MASK_EN);
    vTaskDelay(pdMS_TO_TICKS(1));
}

void LCDI2C::pcfWrite(uint8_t data)
{
    // SimpleI2C::write signature: void write(uint8_t data_in[], size_t size_in);
    // prepare a one-byte buffer and send. SimpleI2C internally knows the device address (per setup()).
    uint8_t buf[1] = { data };
    i2c.write(buf, 1);

    // tiny yield; optional
    taskYIELD();
}
