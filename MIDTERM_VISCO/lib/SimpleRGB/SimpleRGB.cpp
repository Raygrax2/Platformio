#include "SimpleRGB.h"

SimpleRGB::SimpleRGB()
{
}

void SimpleRGB::setup(uint8_t pin[3], const uint8_t channel[3], TimerConfig *config, bool invert)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        pwm[i].setup(pin[i], channel[i], config, invert);
    }
}

void SimpleRGB::setColor(uint32_t red, uint32_t green, uint32_t blue)
{
    pwm[0].setDigitalLevel(red);
    pwm[1].setDigitalLevel(green);
    pwm[2].setDigitalLevel(blue);
}

void SimpleRGB::setColor(uint32_t color_code)
{
    setColor(color_code >> 16 & 0xFF, color_code >> 8 & 0xFF, color_code & 0xFF);
}