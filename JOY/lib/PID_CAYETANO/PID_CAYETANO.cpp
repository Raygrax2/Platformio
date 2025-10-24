#include "PID_CAYETANO.h"

PID_CAYETANO::PID_CAYETANO(/* args */)
{

}

void PID_CAYETANO::setup(float Gains[3], float dt)
{
    _Kp = Gains[0];
    _Ki = Gains[1];
    _Kd = Gains[2];
    _dt = dt;
}

float PID_CAYETANO::computedU(float error)
{
    float u = _Kp * error; 
    u += _Kd*(error - prev_error)/_dt;
    integral += (error + prev_error)*(_dt/2);
    u+=_Ki*integral;
    prev_error = error;
    return u;
}
