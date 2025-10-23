#include "Stepper.h"

Stepper::Stepper()
{
}

void Stepper::setup(const uint8_t drive_pins[3], const uint8_t stepper_ch, TimerConfig stepper_timer)
{
    dir.setup(drive_pins[0], GPO);
    step.setup(drive_pins[1], stepper_ch, &stepper_timer);
}

void Stepper::setSpeed(float speed)
{
    if (speed > 0)
        dir.set(1);
    else
        dir.set(0);

    speed = fabs(speed);
        step.setFrequency((uint32_t)speed);
}
