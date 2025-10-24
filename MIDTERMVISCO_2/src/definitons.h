#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
  definitions.h
  Global constants, pin definitions, object declarations and initial variables.
  Per your request: everything that does not change at runtime is here.
  Do NOT call peripheral setup() in this file; do that in main.cpp.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "SimpleTimer.h"
#include "SimpleGPIO.h"
#include "Joystick.h"
#include "SimplePWM.h"
#include "BDCMotor.h"
#include "QuadratureEncoder.h"
#include "PID_CAYETANO.h"
#include "SimpleUART.h"

// ------------------- ENUM VISCOMETER STATES -------------------
enum ViscometerState {
    STATE_POWER_ON = 0,
    STATE_SAMPLE_PLACEMENT = 1,
    STATE_SAMPLE_POSITIONED = 2,
    STATE_CYLINDER_LOWERING =3,
    STATE_STIR_LIQUID = 4,
    STATE_MEASURE_VISCOSITY = 5,
    STATE_MEASUREMENT_DISPLAY =6,
    STATE_MANUAL_MOVEMENT = 7,
    STATE_SAMPLE_REMOVAL = 8,
    STATE_CYLINDER_CLEANING = 9,
    STATE_CYLINDER_DRYING = 10,
    STATE_PROCESS_RESTART = 11,
    STATE_COUNT = 12
};

// ------------------- TIMERS / PWM CONFIG -------------------
static TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 650,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
static TimerConfig PWM_UP_STEP{
    .timer = LEDC_TIMER_1,
    .frequency = 650,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
static TimerConfig PWM_Spin{
    .timer = LEDC_TIMER_2,
    .frequency = 2000,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

// ------------------- GLOBAL OBJECTS (declare here) -------------------
static SimpleTimer timer;
static Joystick JOY;
static SimplePWM PWM_Stepper_ROT;
static SimpleGPIO STEPPER_ROT_DIR;
static SimplePWM PWM_Stepper_UP;
static SimpleGPIO STEPPER_UP_DIR;
static BDCMotor Motor_spin;
static QuadratureEncoder enco;
static PID_CAYETANO PID;
SimpleUART UART_MESSAGE(115200);

// ------------------- PINS & HW CONSTANTS (edit here if needed) -------------------
static const uint8_t PinX = 26;
static const uint8_t PinY = 27;
static const uint8_t Button = 32;

static const uint8_t PWM_ROT_PIN = 25;
static const uint8_t ROT_PIN_DIR = 21;

static const uint8_t PWM_UP_PIN = 18;
static const uint8_t UP_PIN_DIR = 19;

// Spin motor pins and channels for BDC driver / PWM
static const uint8_t Spin_MotorPIns[2] = {22, 23};
static const uint8_t Spin_MotorCH[2] = {2, 3};

// Encoder pins and degrees-per-edge
uint8_t Encoder_PIn[2] = {16, 17};
float ENCODER_DEGREES_PER_EDGE = 0.36445f; // degrees per edge, per your value

// ------------------- PID GAIN ARRAY (for PID.setup) -------------------
static float Gain[3] = {1.5f, 0.5f, 0.5f}; // Kp, Ki, Kd as you provided

// ------------------- INITIAL (GLOBAL) VARIABLES -------------------
static ViscometerState currentState = STATE_POWER_ON;
static int currentRPM = 0;
static float currentViscosityCP = 0.0f;
uint64_t len = 0;

// UART buffer for formatting if needed
static char Buffer[64];

#endif // __DEFINITIONS_H__
