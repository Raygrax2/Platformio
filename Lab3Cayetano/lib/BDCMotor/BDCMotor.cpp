#include "BDCMotor.h"
#include <cmath>

BDCMotor::BDCMotor()
{
    threshold = 0.0f;     // Default (no compensation)
    startDuty = 0.0f;     // Optional: minimum duty to actually start motor (0..100)
    hysteresis = 0.0f;    // Optional: small hysteresis to avoid rapid on/off around threshold
}

void BDCMotor::setup(const uint8_t in_pin[2], const uint8_t ch[2], TimerConfig motor_timer, float threshold, float startDuty_, float hysteresis_)
{
    in[0].setup(in_pin[0], ch[0], &motor_timer);
    in[1].setup(in_pin[1], ch[1], &motor_timer);

    // Clamp threshold to [0, 100)
    if (threshold < 0.0f) threshold = 0.0f;
    if (threshold >= 100.0f) threshold = 99.999f; // avoid division by zero later

    // Clamp startDuty to [0,100]
    if (startDuty_ < 0.0f) startDuty_ = 0.0f;
    if (startDuty_ > 100.0f) startDuty_ = 100.0f;

    // Clamp hysteresis to [0, 100]
    if (hysteresis_ < 0.0f) hysteresis_ = 0.0f;
    if (hysteresis_ > 100.0f) hysteresis_ = 100.0f;

    this->threshold = threshold;
    this->startDuty = startDuty_;
    this->hysteresis = hysteresis_;
}

void BDCMotor::setSpeed(float Speed)
{
    // Clamp Speed to [-100, 100]
    if (Speed > 100.0f) Speed = 100.0f;
    if (Speed < -100.0f) Speed = -100.0f;

    float absS = fabsf(Speed);

    // Apply hysteresis: compute effective threshold for stopping vs starting
    // When motor is currently off, require absS > threshold + hysteresis to start.
    // When motor is currently on, allow stopping when absS <= threshold.
    // (This requires tracking a state; if you don't have motor-on state, keep simpler logic.)
    // For simplicity in this example we just use threshold for both directions.
    float effThreshold = threshold;

    // Below threshold → no motion
    if (absS <= effThreshold)
    {
        in[0].setDuty(0.0f);
        in[1].setDuty(0.0f);
        return;
    }

    // Remap from [threshold..100] -> [startDuty..100]
    float denom = (100.0f - threshold);
    float scaledMag;
    if (denom <= 0.0001f) {
        // practically impossible because we clamp threshold < 100, but safeguard anyway
        scaledMag = 100.0f;
    } else {
        float norm = (absS - threshold) / denom; // 0..1
        // Map to [startDuty..100]
        scaledMag = startDuty + norm * (100.0f - startDuty);
    }

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
