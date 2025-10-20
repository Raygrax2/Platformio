#include "BDCMotor.h"
#include <cmath>

BDCMotor::BDCMotor()
{
    threshold = 0.0f; // Default (no compensation)
}

void BDCMotor::setup(const uint8_t in_pin[2], const uint8_t ch[2], TimerConfig motor_timer, float threshold)
{
    in[0].setup(in_pin[0], ch[0], &motor_timer);
    in[1].setup(in_pin[1], ch[1], &motor_timer);

    // Clamp threshold to [0, 100]
    if (threshold < 0.0f) threshold = 0.0f;
    if (threshold > 100.0f) threshold = 100.0f;

    this->threshold = threshold;
}

void BDCMotor::setSpeed(float Speed)
{
    // Clamp Speed to [-100, 100]
    if (Speed > 100.0f) Speed = 100.0f;
    if (Speed < -100.0f) Speed = -100.0f;

    float absS = fabsf(Speed);

    // Below threshold → no motion
    if (absS <= threshold)
    {
        in[0].setDuty(0.0f);
        in[1].setDuty(0.0f);
        return;
    }

    // Remap from [threshold..100] → [0..100]
    float scaledMag = (absS - threshold) * (100.0f / (100.0f - threshold));

    // Safety clamp
    if (scaledMag < 0.0f) scaledMag = 0.0f;
    if (scaledMag > 100.0f) scaledMag = 100.0f;

    // Apply direction
    if (Speed >= 0.0f)
    {
        in[0].setDuty(scaledMag);
        in[1].setDuty(0.0f);
    }
    else
    {
        in[1].setDuty(scaledMag);
        in[0].setDuty(0.0f);
    }
}
