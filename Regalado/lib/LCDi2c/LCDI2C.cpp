#include "LCDI2C.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "rom/ets_sys.h" // ets_delay_us
#include "esp_err.h"
#include <cstring>

static const char *TAG = "LCDI2C";

LCDI2C::LCDI2C(SimpleI2C &i2c, uint8_t lcd_addr, uint8_t lcd_cols, uint8_t lcd_rows)
    : _i2c(i2c), _Addr(lcd_addr), _cols(lcd_cols), _rows(lcd_rows),
      _backlightval(LCD_NOBACKLIGHT), _displayfunction(0),
      _displaycontrol(0), _displaymode(0), _numlines(0)
{
}

void LCDI2C::init() {
    begin(_cols, _rows);
}

void LCDI2C::begin(uint8_t cols, uint8_t lines, uint8_t dotsize) {
    _cols = cols;
    if (lines > 1) _displayfunction |= LCD_2LINE; else _displayfunction |= LCD_1LINE;
    _numlines = lines;

    if ((dotsize != 0) && (lines == 1)) _displayfunction |= LCD_5x10DOTS;
    else _displayfunction |= LCD_5x8DOTS;

    // wait after power up
    vTaskDelay(pdMS_TO_TICKS(50));

    // Reset expander and backlight (write current backlight)
    expanderWrite(_backlightval);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Initialization to 4-bit mode (per HD44780)
    write4bits(0x03 << 4);
    delayMicroseconds(4500);

    write4bits(0x03 << 4);
    delayMicroseconds(4500);

    write4bits(0x03 << 4);
    delayMicroseconds(150);

    write4bits(0x02 << 4); // 4-bit mode
    delayMicroseconds(150);

    // function set
    command(LCD_FUNCTIONSET | _displayfunction);

    // display on, no cursor, no blink
    _displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    display();

    // clear and set entry mode
    clear();
    _displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    command(LCD_ENTRYMODESET | _displaymode);

    home();
}

void LCDI2C::clear() {
    command(LCD_CLEARDISPLAY);
    // clear needs >1.52ms
    vTaskDelay(pdMS_TO_TICKS(2));
    if (_oled) setCursor(0,0);
}

void LCDI2C::home() {
    command(LCD_RETURNHOME);
    vTaskDelay(pdMS_TO_TICKS(2));
}

void LCDI2C::setCursor(uint8_t col, uint8_t row) {
    static const uint8_t row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
    if (row >= _numlines) row = _numlines - 1;
    command(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void LCDI2C::noDisplay() {
    _displaycontrol &= ~LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDI2C::display() {
    _displaycontrol |= LCD_DISPLAYON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDI2C::noCursor() {
    _displaycontrol &= ~LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDI2C::cursor() {
    _displaycontrol |= LCD_CURSORON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDI2C::noBlink() {
    _displaycontrol &= ~LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDI2C::blink() {
    _displaycontrol |= LCD_BLINKON;
    command(LCD_DISPLAYCONTROL | _displaycontrol);
}

void LCDI2C::scrollDisplayLeft() {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT);
}

void LCDI2C::scrollDisplayRight() {
    command(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT);
}

void LCDI2C::leftToRight() {
    _displaymode |= LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCDI2C::rightToLeft() {
    _displaymode &= ~LCD_ENTRYLEFT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCDI2C::autoscroll() {
    _displaymode |= LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCDI2C::noAutoscroll() {
    _displaymode &= ~LCD_ENTRYSHIFTINCREMENT;
    command(LCD_ENTRYMODESET | _displaymode);
}

void LCDI2C::createChar(uint8_t location, const uint8_t charmap[8]) {
    location &= 0x7;
    command(LCD_SETCGRAMADDR | (location << 3));
    for (int i = 0; i < 8; ++i) {
        write(charmap[i]);
    }
}

void LCDI2C::noBacklight() {
    _backlightval = LCD_NOBACKLIGHT;
    expanderWrite(0);
}

void LCDI2C::backlight() {
    _backlightval = LCD_BACKLIGHT;
    expanderWrite(0);
}

void LCDI2C::setBacklight(uint8_t value) {
    if (value) backlight(); else noBacklight();
}

size_t LCDI2C::write(uint8_t value) {
    send(value, Rs);
    return 1;
}

size_t LCDI2C::print(const char *str) {
    size_t count = 0;
    if (!str) return 0;
    while (*str) {
        write(static_cast<uint8_t>(*str++));
        ++count;
    }
    return count;
}

size_t LCDI2C::print(const std::string &s) {
    return print(s.c_str());
}

void LCDI2C::command(uint8_t value) {
    send(value, 0);
}

// low-level send
void LCDI2C::send(uint8_t value, uint8_t mode) {
    uint8_t highnib = value & 0xF0;
    uint8_t lownib = (value << 4) & 0xF0;
    write4bits(highnib | mode);
    write4bits(lownib | mode);
}

void LCDI2C::write4bits(uint8_t value) {
    expanderWrite(value);
    pulseEnable(value);
}

void LCDI2C::expanderWrite(uint8_t data) {
    uint8_t out = data | _backlightval;
    esp_err_t err = _i2c.write(&out, 1);
    if (err != ESP_OK) {
        ESP_LOGW(TAG, "I2C write failed: %s", esp_err_to_name(err));
    }
}

void LCDI2C::pulseEnable(uint8_t data) {
    expanderWrite(data | En);
    delayMicroseconds(1);
    expanderWrite(data & ~En);
    delayMicroseconds(50);
}

void LCDI2C::delayMicroseconds(uint32_t us) {
    ets_delay_us(us);
}
