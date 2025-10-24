#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
  definitions.h
  Objetos globales, pines y coeficientes de filtro.
  Solo editar este archivo y main.cpp.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "SimpleRGB.h"
#include "QuadratureEncoder.h"
#include "SimpleTimer.h"
#include "Hbridge.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "Joystick.h"
#include "PID_CAYETANO.h"
#include "SimpleUART.h"
#include "BDCMotor.h"
#include "SimplePID.h"
#include "SimplePWM.h"
#include "Filter.h"

// ------------------- ENUM VISCOMETER STATES -------------------
enum ViscometerState {
    STATE_POWER_ON = 0,
    STATE_SAMPLE_PLACEMENT,
    STATE_SAMPLE_POSITIONED,
    STATE_CYLINDER_LOWERING,
    STATE_STIR_LIQUID,
    STATE_MEASURE_VISCOSITY,
    STATE_MEASUREMENT_DISPLAY,
    STATE_SAMPLE_REMOVAL,
    STATE_CYLINDER_CLEANING,
    STATE_CYLINDER_DRYING,
    STATE_MANUAL_MOVEMENT, // New state for post-measurement joystick control
    STATE_PROCESS_RESTART,
    STATE_COUNT // Total number of states
};

// ------------------- TIMERS -------------------
TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 650, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
TimerConfig PWM_UP_STEP{
    .timer = LEDC_TIMER_1,
    .frequency = 650, // Frecuencia PWM motor UP (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

// ------------------- GLOBAL OBJECTS -------------------
SimpleTimer timer;
Joystick JOY;
SimplePWM PWM_Stepper_ROT;
SimpleGPIO STEPPER_ROT_DIR;
SimplePWM PWM_Stepper_UP;
SimpleGPIO STEPPER_UP_DIR;

// ------------------- PINS -------------------
uint8_t PinX = 14;
uint8_t PinY = 13;
uint8_t Button = 32;
uint8_t PWM_ROT_PIN = 25;
uint8_t ROT_PIN_DIR = 21;
uint8_t PWM_UP_PIN = 18;
uint8_t UP_PIN_DIR = 19;

// ------------------- GLOBAL VARIABLES -------------------
ViscometerState currentState;
int currentRPM;
float currentViscosityCP; // last measured viscosity in cPoise

// Buffers UART/debug
char Buffer[32];

#endif // __DEFINITIONS_H__
