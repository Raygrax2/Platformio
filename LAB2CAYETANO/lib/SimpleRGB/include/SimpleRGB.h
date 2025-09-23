#ifndef __SIMPLERGB_H__
#define __SIMPLERGB_H__

#include "SimplePWM.h"

class SimpleRGB
{
public:
    SimpleRGB();
    void setup(uint8_t pin[3], const uint8_t channel[3], TimerConfig *config, bool invert);
    void setColor(uint32_t red, uint32_t green, uint32_t blue);
    void setColor(uint32_t color_code);

private:
    SimplePWM pwm[3];
};

#endif // __SIMPLERGB_H__