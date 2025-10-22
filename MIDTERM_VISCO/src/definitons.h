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
#include "PID_CAYETANO.h"
#include "SimpleUART.h"
#include "BDCMotor.h"
#include "SimplePID.h"
#include "SimplePWM.h"
#include "Filter.h"

enum ControlMode
{
    MODE_MANUAL_PWM = 0, // Direct PWM control
    MODE_SPEED = 1,      // Speed control with PID
    MODE_POSITION = 2    // Position control with PID
};
// --- Timers para PWM ---

TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 500, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE};

TimerConfig PWM_TimerB{
    .timer = LEDC_TIMER_1,
    .frequency = 1000, // Frecuencia PWM motor B (dummy)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE};

// --- Objetos globales ---
QuadratureEncoder Encoder;
SimpleTimer timer;
BDCMotor motorA;
SimplePWM pwmmotorA;
PID_CAYETANO PID_2;
float PID_GAINS[3] = {1.0f, 0.0f, 0.0f}; // Kp, Ki, Kd

// --- Buffers UART o debug ---
char Buffer[32];
uint8_t len = 0;

// --- Parámetros del encoder ---
float Deg_per_edge = 360.0f / 2048.0f; // Grados por flanco
uint8_t EncoderPIN[2] = {32, 35};      // Pines del encoder
SimplePID PID;

uint32_t dt = 5000; // 2 ms (consistent with timer period)
float Kp = 1.0f;  // initial proportional gain
float Ki = 0.0f;  // initial integral gain
float Kd = 0.0f;  // initial derivative gain
float error = 0.0f;
float speed_raw = 0.0f;
uint8_t lwghtmessage = 0;
float speed_filtered = 1.0f;
float current_position = 0.0f;      // in degrees
int current_mode = MODE_POSITION; // default mode
float ref = -70.0f;                // reference position (deg)
// Local receive buffer (do not shadow global Buffer in definitions.h)
char rxbuf[48];
float u = 0.0f;

uint8_t Ain1Pin = 27; // AIN1
uint8_t Ain2Pin = 26; // AIN2
uint8_t AIN[2] = {Ain1Pin, Ain2Pin};
uint8_t CHA[2] = {0, 1}; // BIN1 (dummy, no usado)
SimpleUART UART(115200);

// --- Filtros ---
Filter LPF;
float LPF_coeffs_a[2] = {1.0000f, -0.7608f}; // [a0, a1] where a0 = 1
float LPF_coeffs_b[2] = {0.1196f, 0.1196f};  // [b0, b1]

#endif // __DEFINITIONS_H__
