#ifndef __DEFINITONS_H__
#define __DEFINITONS_H__

#include <stdio.h>
#include <string.h>
#include "SimpleRGB.h"
#include "SimpleTimer.h"
#include "BDCMotor.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "QuadratureEncoder.h"
#include "SimpleUART.h"
#include "SimplePWM.h"
#include "SimplePID.h"
#include "Stepper.h"
#include "Filter.h"
#include "Joystick.h"
#include "SimpleI2C.h"
#include "esp_task_wdt.h"

//Enumeration for Viscometer Modes
enum ViscometerMode {
    automatic_Mode = 0,
    manual_Mode = 1
};
enum AutomaticState {
    AUTO_IDLE = 0,          // Waiting for START_VISC command / ready
    AUTO_COLOR_READ,        // Read color sensor & map to target viscosity
    AUTO_HOMING,            // Home turret / Z axis and perform safety checks
    AUTO_LOWER_SPINDLE,     // Lower spindle into the container
    AUTO_PRE_MIX,           // Short stabilization mix (e.g., 30 RPM for a few seconds)
    AUTO_MEASURE,           // Take measurement (ADC current + encoder -> viscosity)
    AUTO_EVALUATE,          // Compare measured viscosity vs target (decision)
    AUTO_COMPUTE_DOSE,      // Compute dose (ml) to add
    AUTO_DOSE,              // Activate pump to add dilution
    AUTO_STIR,              // Stir at higher RPM (e.g., 120 RPM)
    AUTO_WAIT_SETTLE,       // Wait for foam/air to dissipate
    AUTO_REMEASURE,         // Re-measure viscosity after dosing/stir/settle
    AUTO_RAISE_SPINDLE,     // Raise spindle to safe position
    AUTO_ROTATE_TO_CLEAN,   // Rotate turret/spindle to cleaning station
    AUTO_CLEANING,          // Run cleaning sequence (flush/rinse)
    AUTO_COMPLETE,          // Cycle complete, success
    AUTO_ERROR              // Error/abort inside automatic sequence
};

//Classes definitions
SimpleTimer timer;
BDCMotor motor1; //Pins for motor driver
SimplePID motorPID;
Stepper stepperMotor_UP_Down;
Stepper StepperMotor_rotate;    
SimpleADC Current_VOltage_Torque;
SimpleGPIO Button_automatic_mode;
Joystick Joystick_System;
SimpleGPIO Buzzer;
SimpleI2C ColorSensor;
QuadratureEncoder Encoder_motor;

//PWM Timer Configuration
TimerConfig PWM_Timer_BDCMotor = {
    .timer = LEDC_TIMER_0,
    .frequency = 20000,                 // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
TimerConfig Stepper_down_up_PWM = {
    .timer = LEDC_TIMER_0,
    .frequency = 20000,                 // frecuencia PWM stepper Up/Down (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
TimerConfig Stepper_Rotate_PWM = {
    .timer = LEDC_TIMER_0,
    .frequency = 20000,                 // Frecuencia PWM stepper Rotate (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

//Pins 
uint8_t SpinMotorPin[2] = {18,19};
uint8_t ChMotor[2] = {0,1};
uint8_t Current_VOltage_Torque_PIN = 34; //ADC1_6
uint8_t joystickx_pin = 36; 
uint8_t joysticky_pin = 39; 
uint8_t joystickbutton_pin = 4; 
uint8_t StepperMotorUP_Down_pins[3] = {25,26,27};
uint8_t StepperMotorRotate_pins[3] = {14,12,13};
uint8_t StepperMotorUP_Down_ch = 2;
uint8_t StepperMotorRotate_ch = 3;
uint8_t Buzzer_pin = 5;
uint8_t ColorSensor_pin[2]= {21,22}; //SDA, SCL
uint8_t Encoder_motor_pins[2] = {32,33};
float u ; //control variable


//Constants, will be changed later
const float Kp = 1.0f;
const float Ki = 0.5f;
const float Kd = 0.1f;


#endif // __DEFINITONS_H__
