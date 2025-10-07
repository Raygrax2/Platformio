// HBridge.cpp
#include "HBridge.h"
#include <algorithm> // for std::clamp
#include <cmath>

HBridge::HBridge()
    : _pwmA(nullptr), _pwmB(nullptr), _ain0Pin(0), _ain1Pin(0), _bin0Pin(0), _bin1Pin(0), _threshold(1.0f)
{
}

void HBridge::setup(uint8_t Ain0Pin, uint8_t Ain1Pin, uint8_t Bin0Pin, uint8_t Bin1Pin,
                    SimplePWM &pwmA, SimplePWM &pwmB, float threshold)
{
    _ain0Pin = Ain0Pin;
    _ain1Pin = Ain1Pin;
    _bin0Pin = Bin0Pin;
    _bin1Pin = Bin1Pin;

    // Save pointers to the provided PWM objects (do not take ownership)
    _pwmA = &pwmA;
    _pwmB = &pwmB;

    _threshold = fabsf(threshold);

    // Configure direction GPIOs
    _ain0.setup(_ain0Pin, GPIO_MODE_OUTPUT);
    _ain1.setup(_ain1Pin, GPIO_MODE_OUTPUT);
    _bin0.setup(_bin0Pin, GPIO_MODE_OUTPUT);
    _bin1.setup(_bin1Pin, GPIO_MODE_OUTPUT);

    // Ensure everything starts stopped (low direction pins + 0 duty)
    stop();
}

void HBridge::stop()
{
    // Set direction lines low to avoid accidental conduction
    _ain0.set(0);
    _ain1.set(0);
    _bin0.set(0);
    _bin1.set(0);

    if (_pwmA) _pwmA->setDuty(0.0f);
    if (_pwmB) _pwmB->setDuty(0.0f);
}

void HBridge::setSpeed(float pwm_signed_percentage)
{
    // must have PWMs configured
    if (!_pwmA || !_pwmB)
        return; // or handle error

    // Clamp to -100..100
    float v = std::clamp(pwm_signed_percentage, -100.0f, 100.0f);

    float abs_val = fabsf(v);

    // Deadband: treat small values as zero
    if (abs_val <= _threshold)
    {
        stop();
        return;
    }

    // Apply direction and PWM. The convention used here is:
    //  - Positive value -> A-side drives PWM, A direction pins set to forward; B side PWM = 0
    //  - Negative value -> B-side drives PWM, B direction pins set to forward; A side PWM = 0
    // Adapt this to your hardware if your H-bridge wiring differs.

    if (v > 0.0f)
    {
        // Forward on A-side
        _ain0.set(1);
        _ain1.set(0);
        _bin0.set(0);
        _bin1.set(0);

        _pwmA->setDuty(abs_val);
        _pwmB->setDuty(0.0f);
    }
    else
    {
        // Reverse on B-side
        _ain0.set(0);
        _ain1.set(0);
        _bin0.set(1);
        _bin1.set(0);

        _pwmA->setDuty(0.0f);
        _pwmB->setDuty(abs_val);
    }
}
