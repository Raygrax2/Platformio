#ifndef _NIBBLELCD_H
#define _NIBBLELCD_H

#include "SimpleGPIO.h" // Clase que controla pines
#include <FreeRTOS/FreeRTOS.h>

#define NLCD_NUM_PINS   7
#define NLCD_DATA_PINS  4

#define NLCD_RS 4
#define NLCD_RW 5
#define NLCD_EN 6

#define CMD_CLEAR       0b00000001
#define CMD_SECOND_LINE 0b11000000
#define CMD_HOME        0b00000010
#define CMD_ENTRY_MODE  0b00000110
#define CMD_DISPLAY_ON  0b00001100
#define CMD_DISPLAY_OFF 0b00001000
#define CMD_DISP_SHIFT  0b00010100
#define CMD_FUNC_SET    0b00111110
#define CMD_4BIT_MODE   0b00101000

/*
Medido con ESP32D

NibbleLCD:
Transmission for 33 chars took 27ms (0.818182ms per char avg with clear/home/second line)
*/

class NibbleLCD
{
public:
    NibbleLCD();
    ~NibbleLCD();

    // Initialize the pins we will use.
    // - pins[0] to pins[3] are D4 to D7
    // - pins[4] to pins[6] are RS, RW, EN.
    void setup(uint8_t pins[]);
    void writeCommand(uint8_t command);
    void print(char character);
    void printStr(const char* str);

private:
    void pulseEnable();
    void sendNibble(uint8_t nibble);
    void sendByte(uint8_t byte, bool isData);
    void writeCommand_nodelay(uint8_t command);
    SimpleGPIO _gpio[NLCD_NUM_PINS];
};

#endif // _NIBBLELCD_H