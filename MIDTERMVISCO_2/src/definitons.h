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

TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 650, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE};
TimerConfig PWM_UP_STEP{
    .timer = LEDC_TIMER_1,
    .frequency = 650, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE};
TimerConfig PWM_Spin{
    .timer = LEDC_TIMER_2,
    .frequency = 650, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE};

SimpleTimer timer;
Joystick JOY;
SimplePWM PWM_Stepper_ROT;
SimpleGPIO STEPPER_ROT_DIR;
SimplePWM PWM_Stepper_UP;
SimpleGPIO STEPPER_UP_DIR;
BDCMotor Motor_spin;
QuadratureEncoder enco;
PID_CAYETANO PID;

// PINS
uint8_t PinX = 14;
uint8_t PinY = 13;
uint8_t Button = 32;
uint8_t PWM_ROT_PIN = 25;
uint8_t ROT_PIN_DIR = 21;
uint8_t PWM_UP_PIN = 18;
uint8_t UP_PIN_DIR = 19;
uint8_t Spin_MotorPIns[2] = {22, 23};
uint8_t Spin_MotorCH[2] = {2, 3};
uint8_t Encoder_PIn[2] = {16, 17};

float Gain[3] = {1.5f, 0.5f, 0.5f};
float Speed;
float error;
float u;

// --- Buffers UART o debug ---
char Buffer[32];

#endif // __DEFINITIONS_H__
