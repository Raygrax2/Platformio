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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// ---------------------------------------------------------------------------
// ENUMS
// ---------------------------------------------------------------------------

/// Main operating modes
enum ViscometerMode {
    automatic_Mode = 0,
    manual_Mode = 1
};

/// Automatic-mode substates (kept for future use)
enum AutomaticState {
    AUTO_IDLE = 0,
    AUTO_COLOR_READ,
    AUTO_HOMING,
    AUTO_LOWER_SPINDLE,
    AUTO_MEASURE,
    AUTO_EVALUATE,
    AUTO_COMPUTE_DOSE,
    AUTO_DOSE,
    AUTO_STIR,
    AUTO_WAIT_SETTLE,
    AUTO_REMEASURE,
    AUTO_RAISE_SPINDLE,
    AUTO_ROTATE_TO_CLEAN,
    AUTO_CLEANING,
    AUTO_COMPLETE,
    AUTO_ERROR
};

/// Manual-mode substates (these are the ones the joystick will step through)
enum ManualState {
    MANUAL_IDLE = 0,         // waiting state inside manual mode
    MANUAL_COLOR_READ = 1,   // optional: read color
    MANUAL_MOVE_TURRET = 2,  // joystick X/Y control of steppers
    MANUAL_MEASURE = 3,      // measure viscosity
    MANUAL_EVALUATE = 4,     // evaluate measurement vs target
    MANUAL_COMPUTE_DOSE = 5, // compute dose (ml)
    MANUAL_DOSE = 6,         // perform dosing
    MANUAL_STIR = 7,         // stir
    MANUAL_WAIT_SETTLE = 8,  // wait to settle
    MANUAL_CLEANING = 9,     // cleaning procedure
    MANUAL_DRYING = 10,
    MANUAL_COMPLETE = 11
};

// ---------------------------------------------------------------------------
// GLOBAL STATE (default values)
// ---------------------------------------------------------------------------
 ViscometerMode Case_Viscometer;   // declared/defined in main.cpp
 AutomaticState Auto_State;        // declared/defined in main.cpp
 ManualState Manual_State;         // declared/defined in main.cpp

// ---------------------------------------------------------------------------
// Hardware / object declarations (singletons used across files)
// ---------------------------------------------------------------------------
 SimpleTimer timer;
 BDCMotor motor1;
 SimplePID motorPID;
 Stepper stepperMotor_UP_Down;
 Stepper StepperMotor_rotate;
 SimpleADC Current_VOltage_Torque;
 SimpleGPIO Button_automatic_mode;
 Joystick Joystick_System;
 SimpleGPIO Buzzer;
 SimpleI2C ColorSensor;
 QuadratureEncoder Encoder_motor;

// ---------------------------------------------------------------------------
// PWM Timer Configuration
// ---------------------------------------------------------------------------
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
// ---------------------------------------------------------------------------
// Pin definitions (extern so main.cpp defines them)
// ---------------------------------------------------------------------------
 uint8_t SpinMotorPin[2];
 uint8_t ChMotor[2];
 uint8_t Current_VOltage_Torque_PIN;
 uint8_t joystickx_pin;
 uint8_t joysticky_pin;
 uint8_t joystickbutton_pin;
 uint8_t StepperMotorUP_Down_pins[3];
 uint8_t StepperMotorRotate_pins[3];
 uint8_t StepperMotorUP_Down_ch;
 uint8_t StepperMotorRotate_ch;
 uint8_t Buzzer_pin;
 uint8_t ColorSensor_pin[2];
 uint8_t Encoder_motor_pins[2];

// Control variable
 float u;

// PID tuning defaults
const float Kp;
const float Ki;
const float Kd;

#endif // __DEFINITONS_H__
