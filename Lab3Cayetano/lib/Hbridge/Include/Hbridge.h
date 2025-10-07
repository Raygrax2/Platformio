// HBridge.h
#ifndef __HBRIDGE_H__
#define __HBRIDGE_H__


#include "SimpleGPIO.h"
#include "SimplePWM.h"
#include <stdint.h>


class HBridge
{
public:
HBridge();


// Configure the H-bridge controller.
// - Ain0Pin / Ain1Pin: two direction pins for A-side (digital outputs)
// - Bin0Pin / Bin1Pin: two direction pins for B-side (digital outputs)
// - pwmA / pwmB: two SimplePWM objects (passed by reference) that will drive PWM on each half-bridge
// - threshold: signed-percentage deadband around 0 (0..100). Values with absolute value <= threshold are treated as 0 (stop)
void setup(uint8_t Ain0Pin, uint8_t Ain1Pin, uint8_t Bin0Pin, uint8_t Bin1Pin,
SimplePWM &pwmA, SimplePWM &pwmB, float threshold = 1.0f);


// Set motor speed as signed percentage (-100 .. +100). Sign controls direction.
// If abs(value) <= threshold, motor is stopped (PWMs set to 0 and direction pins set low).
void setSpeed(float pwm_signed_percentage);


// Convenience
void stop(); // set both PWMs to 0 and direction pins low
void setThreshold(float threshold) { _threshold = threshold; }


private:
// store copies / references to the PWM drivers
SimplePWM *_pwmA;
SimplePWM *_pwmB;


// direction GPIO lines (owned by this class)
SimpleGPIO _ain0;
SimpleGPIO _ain1;
SimpleGPIO _bin0;
SimpleGPIO _bin1;


// store pin numbers (optional, useful for diagnostics)
uint8_t _ain0Pin;
uint8_t _ain1Pin;
uint8_t _bin0Pin;
uint8_t _bin1Pin;


float _threshold;
};


#endif // __HBRIDGE_H__