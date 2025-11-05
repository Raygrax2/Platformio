#include "PID_CAYETANO.h"

PID_CAYETANO::PID_CAYETANO()
{
    integral = 0.0f;
    prev_error = 0.0f;
}

void PID_CAYETANO::setup(float Gains[3], float dt_us)
{
    _Kp = Gains[0];
    _Ki = Gains[1];
    _Kd = Gains[2];
    _dt = dt_us / 1000000.0f; // Convert microseconds to seconds
    integral = 0.0f;           // Reset integral
    prev_error = 0.0f;         // Reset previous error
}

float PID_CAYETANO::computedU(float error)
{
    // Proportional term
    float P = _Kp * error;
    
    // Derivative term
    float D = _Kd * (error - prev_error) / _dt;
    
    // Trapezoidal integration
    float error_avg = (error + prev_error) / 2.0f;
    float integral_update = error_avg * _dt;
    
    // Calculate output with potential integral update
    float u_test = P + D + _Ki * (integral + integral_update);
    
    // Anti-windup: only integrate if not saturated
    if (u_test > 100.0f || u_test < -100.0f) {
        // Saturated - conditional integration
        if ((u_test > 100.0f && error < 0.0f) || 
            (u_test < -100.0f && error > 0.0f)) {
            // Integration would help reduce saturation
            integral += integral_update;
        }
    } else {
        // Not saturated - integrate normally
        integral += integral_update;
    }
    
    // Clamp integral to reasonable bounds
    float integral_limit = 100.0f;
    if (integral > integral_limit) integral = integral_limit;
    if (integral < -integral_limit) integral = -integral_limit;
    
    // Calculate final output
    float u = P + _Ki * integral + D;
    
    // Clamp output
    if (u > 100.0f) u = 100.0f;
    if (u < -100.0f) u = -100.0f;
    
    prev_error = error;
    return u;
}

void PID_CAYETANO::reset()
{
    integral = 0.0f;
    prev_error = 0.0f;
}