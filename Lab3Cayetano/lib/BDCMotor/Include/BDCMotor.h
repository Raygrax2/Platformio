#ifndef BDCMOTOR_H
#define BDCMOTOR_H

#include <stdint.h>
#include "SimplePWM.h"

class BDCMotor
{
public:
    BDCMotor();

    // threshold: dead-zone offset, in percentage (0–100)
    void setup(const uint8_t in_pin[2], const uint8_t ch[2], TimerConfig motor_timer, float threshold);

    // Speed range: -100 to 100
    void setSpeed(float Speed);

private:
    SimplePWM in[2];
    float threshold; // Dead-zone threshold (0–100)
};

#endif // BDCMOTOR_H
