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

#include "QuadratureEncoder.h"
#include "SimpleTimer.h"
#include "Hbridge.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "SimpleUART.h"
#include "BDCMotor.h"
#include "SimplePID.h"
#include "SimplePWM.h"
#include "Filter.h"

enum ControlMode {
    MODE_MANUAL_PWM = 0,    // Direct PWM control
    MODE_SPEED = 1,         // Speed control with PID
    MODE_POSITION = 2       // Position control with PID
};
// --- Timers para PWM ---

TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 20000,                 // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

TimerConfig PWM_TimerB{
    .timer = LEDC_TIMER_1,
    .frequency = 1000,                 // Frecuencia PWM motor B (dummy)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

// --- Objetos globales ---
QuadratureEncoder Encoder;
SimpleTimer timer;
BDCMotor motorA;
SimplePWM pwmmotorA;

// --- Buffers UART o debug ---
char Buffer[32];
uint8_t len = 0;

// --- Parámetros del encoder ---
float Deg_per_edge = 360.0f / 2048.0f; // Grados por flanco
uint8_t EncoderPIN[2] = {32, 35};      // Pines del encoder
SimplePID PID;

uint8_t Ain1Pin = 27; // AIN1
uint8_t Ain2Pin = 26; // AIN2
uint8_t AIN[2] = {Ain1Pin, Ain2Pin};
uint8_t CHA[2] = {0,1}; // BIN1 (dummy, no usado)
SimpleUART UART(115200);

// --- Filtros ---
Filter LPF;
float LPF_coeffs_a[2] = {1.0f, -0.4636f};  // [a0, a1] where a0 = 1
float LPF_coeffs_b[2] = {0.2682f, 0.2682f}; // [b0, b1]

#endif // __DEFINITIONS_H__
