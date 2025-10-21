#ifndef BDCMOTOR_H
#define BDCMOTOR_H

#include <stdint.h>
#include "SimplePWM.h"

// Forward-declare TimerConfig if it's defined elsewhere
// (remove if TimerConfig is included by SimplePWM.h or another header)
struct TimerConfig;

class BDCMotor
{
public:
    BDCMotor();

    // Configure pins/channels and threshold parameters.
    //
    // threshold: dead-zone offset in percent (0..100). Values <= threshold produce no motion.
    // startDuty: minimum PWM percent to actually start the motor once past threshold (0..100).
    //            This maps the scaled magnitude to [startDuty .. 100].
    // hysteresis: small hysteresis (percent) to avoid chatter around threshold (0..100).
    //             Full hysteresis behavior requires the class to track on/off state (we keep a flag).
    //
    // NOTE: if you were previously calling setup(in_pin, ch, motor_timer, threshold)
    //       the defaults below preserve that behaviour (startDuty=0, hysteresis=0).
    void setup(const uint8_t in_pin[2],
               const uint8_t ch[2],
               TimerConfig motor_timer,
               float threshold,
               float startDuty = 0.0f,
               float hysteresis = 0.0f);

    // Set commanded speed in percent (-100..100).
    void setSpeed(float Speed);

    // Optional helpers (read-only)
    float getThreshold() const { return threshold; }
    float getStartDuty() const { return startDuty; }
    float getHysteresis() const { return hysteresis; }

private:
    SimplePWM in[2];

    // Parameters (0..100)
    float threshold;   // dead-zone threshold
    float startDuty;   // minimum PWM once moving
    float hysteresis;  // hysteresis amount

    // Internal state used for hysteresis (true if motor currently considered "on")
    bool motorOn;
};

#endif // BDCMOTOR_H
