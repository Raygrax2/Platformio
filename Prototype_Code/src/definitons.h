#ifndef __DEFINITONS_H__
#define __DEFINITONS_H__

#include <stdio.h>
#include <string.h>
#include "SimpleRGB.h"
#include "SimpleTimer.h"
#include "BDCMotor.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "SimpleUART.h"
#include "SimplePWM.h"
#include "SimplePID.h"
#include "Stepper.h"
#include "Filter.h"

//Classes definitions
BDCMotor motor1; //Pins for motor driver
SimpleTimer timer;
SimplePID motorPID;
Stepper stepperMotor_UP_Down;
Stepper StepperMotor_rotate;    
SimpleADC Current_VOltage_Torque;
SimpleGPIO Button_automatic_mode;

//PWM Timer Configuration
TimerConfig PWM_Timer_BDCMotor = {
    .timer = LEDC_TIMER_0,
    .frequency = 20000,                 // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

//Pins 
uint8_t SpinMotorPin[2] = {18,19};
uint8_t ChMotor[2] = {0,1};

//Constants, will be changed later
const float Kp = 1.0f;
const float Ki = 0.5f;
const float Kd = 0.1f;


#endif // __DEFINITONS_H__
