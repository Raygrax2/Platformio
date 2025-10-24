// Joystick.cpp (corrected / hardened)
#include "Joystick.h"
#include "esp_timer.h"
#include <algorithm>
#include <cstdio>

Joystick::Joystick()
{
    _centerX = _centerY = 2048;
    _deadZoneX = _deadZoneY = 400;
}

void Joystick::setup(int pinx, int piny, int pinBtn)
{
    _ejeX.setup(pinx);
    _ejeY.setup(piny);
    _button.setup(pinBtn, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
}

bool Joystick::calibrate(uint64_t timeMicros)
{
    // use 64-bit ints for timestamps and sums
    uint64_t begin = esp_timer_get_time();
    uint64_t prev = begin;
    uint64_t stop = begin + timeMicros;

    int minX = 4095, maxX = 0;
    int minY = 4095, maxY = 0;
    int64_t sumX = 0, sumY = 0;
    int samples = 0;

    // sample roughly every 1 ms (as original did)
    const uint64_t sample_interval_us = 1000ULL;

    while (true)
    {
        uint64_t current = esp_timer_get_time();
        if (current >= stop) break;

        if (current - prev >= sample_interval_us)
        {
            int valX = readX();
            int valY = readY();

            // validate ADC range (optional)
            if (valX < 0) valX = 0;
            if (valX > 4095) valX = 4095;
            if (valY < 0) valY = 0;
            if (valY > 4095) valY = 4095;

            sumX += valX;
            sumY += valY;

            if (valX < minX) minX = valX;
            if (valX > maxX) maxX = valX;

            if (valY < minY) minY = valY;
            if (valY > maxY) maxY = valY;

            prev = current;
            samples++;
        }
    }

    if (samples == 0)
    {
        // nothing sampled
        return false;
    }

    // compute center as integer average
    _centerX = static_cast<int>(sumX / samples);
    _centerY = static_cast<int>(sumY / samples);

    // compute ranges
    int rangeX = maxX - minX;
    int rangeY = maxY - minY;

    // If the joystick reading didn't change (range 0), set a safe default deadzone
    // Also apply scaling factor but ensure it is >= 1 to avoid division-by-zero later.
    const int min_deadzone = 8;             // choose a small but non-zero safe min
    int proposed_deadX = static_cast<int>(rangeX * 0.90);
    int proposed_deadY = static_cast<int>(rangeY * 1.05);

    _deadZoneX = std::max(proposed_deadX, min_deadzone);
    _deadZoneY = std::max(proposed_deadY, min_deadzone);

    // Debug print
    printf("Calibration: centerX=%d centerY=%d deadX=%d deadY=%d samples=%d rangeX=%d rangeY=%d\n",
           _centerX, _centerY, _deadZoneX, _deadZoneY, samples, rangeX, rangeY);

    return true;
}

int Joystick::readX()
{
    return _ejeX.read(ADC_READ_RAW);
}

int Joystick::readY()
{
    return _ejeY.read(ADC_READ_RAW);
}

bool Joystick::Up()
{
    // returns true if Y is sufficiently below center (negative direction)
    return readY() < (_centerY - _deadZoneY);
}

bool Joystick::Down()
{
    return readY() > (_centerY + _deadZoneY);
}

bool Joystick::Right()
{
    return readX() > (_centerX + _deadZoneX);
}

bool Joystick::Left()
{
    return readX() < (_centerX - _deadZoneX);
}

bool Joystick::result()
{
    int rx = readX();
    int ry = readY();

    // Using absolute difference is generally better: determine dominant axis
    int dx = abs(rx - _centerX);
    int dy = abs(ry - _centerY);

    if (dx >= dy)
    {
        if (Right()) {
            printf("Right\n");
        } else if (Left()) {
            printf("Left\n");
        }
    }
    else
    {
        if (Up()) {
            printf("Up\n");
        } else if (Down()) {
            printf("Down\n");
        }
    }
    return true;
}

bool Joystick::Pressed()
{
    return _button.get();
}
