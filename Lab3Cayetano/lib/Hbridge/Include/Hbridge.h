#ifndef __HBRIDGE_H__
#define __HBRIDGE_H__
#include "SimpleGPIO.h"
#include "SimplePWM.h"
#include <stdint.h>

class HBridge
{
public:
    HBridge();
    enum Mode { FORWARD, BACKWARD, STOP, BRAKE };
    /**
     * Setup the H-bridge PWM outputs.
     * @param pinA GPIO pin for PWM channel A
     * @param chA  LEDC channel for PWM A
     * @param pinB GPIO pin for PWM channel B
     * @param chB  LEDC channel for PWM B
     * @param timer_config Pointer to TimerConfig used by both channels
     * @param threshold PWM percentage below which motor is considered stopped (0-100)
     */
    void setup(uint8_t pinA, uint8_t chA, uint8_t pinB, uint8_t chB, TimerConfig *timer_config, float threshold = 1.0f);

    /**
     * Set motor speed. Signed percentage (-100 .. 100). Sign controls direction.
     * Values with absolute value less than threshold will stop the motor.
     */
    void setSpeed(float pwm_signed_percentage);
    /**
     * Set the H-bridge mode explicitly. For FORWARD/BACKWARD a duty (0-100) may be provided.
     */
    void setMode(Mode mode, float duty = 0.0f);

private:
    SimplePWM _pwmA;
    SimplePWM _pwmB;
    float _threshold;
};

#endif // __HBRIDGE_H__