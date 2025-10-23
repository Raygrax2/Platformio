#include "NibbleLCD.h"
#include "esp_rom_sys.h"

NibbleLCD::NibbleLCD()
{
}

NibbleLCD::~NibbleLCD()
{
}

void NibbleLCD::setup(uint8_t pins[])
{
    for (int i = 0; i < NLCD_NUM_PINS; i++)
        _gpio[i].setup(pins[i], GPIO);
    for (int i = 0; i < NLCD_NUM_PINS; i++)
        _gpio[i].set(0);

    // Wait >40ms after power on
    vTaskDelay(pdMS_TO_TICKS(50));

    // Initialization sequence for 4-bit mode
    sendNibble(0x03);
    vTaskDelay(pdMS_TO_TICKS(5)); 
    sendNibble(0x03);
    vTaskDelay(pdMS_TO_TICKS(5));
    sendNibble(0x03);
    esp_rom_delay_us(150);
    sendNibble(0x02); // Set to 4-bit mode

    writeCommand_nodelay(CMD_4BIT_MODE); // 4-bit, 2 line, 5x8 font
    writeCommand_nodelay(CMD_DISPLAY_ON); // Display ON, Cursor OFF, Blink OFF
    writeCommand_nodelay(CMD_ENTRY_MODE); // Entry mode: move cursor right
    writeCommand_nodelay(CMD_CLEAR); // Clear display
    vTaskDelay(pdMS_TO_TICKS(2)); // Wait for clear
}

void NibbleLCD::writeCommand(uint8_t command) 
{
    sendByte(command, false);
    vTaskDelay(pdMS_TO_TICKS(10)); // Wait for command when sent by user
}
void NibbleLCD::print(char character)
{
    sendByte(character, true);
}

void NibbleLCD::pulseEnable() {
    _gpio[NLCD_EN].set(0);
    esp_rom_delay_us(1);
    _gpio[NLCD_EN].set(1);
    esp_rom_delay_us(1); // Enable pulse must be >450ns
    _gpio[NLCD_EN].set(0);
    esp_rom_delay_us(100); // Commands need >37us to settle
}

void NibbleLCD::sendNibble(uint8_t nibble) {
    for (int i = 0; i < NLCD_DATA_PINS; i++)
        _gpio[i].set((nibble >> i) & 1);

    pulseEnable();
}

void NibbleLCD::sendByte(uint8_t byte, bool isData) {
    _gpio[NLCD_RS].set(isData ? 1 : 0);
    sendNibble(byte >> 4);     // High nibble
    sendNibble(byte & 0x0F);   // Low nibble
}

void NibbleLCD::printStr(const char* str)
{
    writeCommand(CMD_CLEAR);
    writeCommand(CMD_HOME);
    while (*str) {
        if(*str == '\n') {
            writeCommand(CMD_SECOND_LINE);
            str++;
        }
        else
            print(*str++);
        counter++;
    }
}

void NibbleLCD::writeCommand_nodelay(uint8_t command) 
{
    sendByte(command, false);
}
