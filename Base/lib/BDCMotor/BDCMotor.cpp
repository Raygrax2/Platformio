#include "BDCMotor.h"

BDCMotor::BDCMotor()
{
}

void BDCMotor::setup(const uint8_t in_pin[2], const uint8_t ch[2], TimerConfig motor_timer)
{
    in[0].setup(in_pin[0], ch[0], &motor_timer);
    in[1].setup(in_pin[1], ch[1], &motor_timer);
}

void BDCMotor::setSpeed(float Speed)
{
    if (Speed >= 0)
    {
        in[0].setDuty(Speed);
        in[1].setDuty(0);
    }
    else
    {
        in[1].setDuty(-Speed);
        in[0].setDuty(0);
    }
}
