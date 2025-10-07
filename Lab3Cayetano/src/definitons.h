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
#include "SimplePID.h"
#include "SimplePWM.h"
#include "Filter.h"

// --- Timers para PWM ---
TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 1000,                 // Frecuencia PWM motor A (Hz)
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
HBridge motorDriver;
SimplePWM pwmmotorA;
SimplePWM pwmmotorB;  // Dummy (para compatibilidad con HBridge)

// --- Buffers UART o debug ---
char Buffer[32];
uint8_t len = 0;

// --- Parámetros del encoder ---
float Deg_per_edge = 360.0f / 2048.0f; // Grados por flanco
uint8_t EncoderPIN[2] = {34, 36};      // Pines del encoder

// --- Pines del motor y puente H ---
uint8_t motorPinA = 25; // PWM del motor A
uint8_t motorChannelA = 0;

uint8_t Ain1Pin = 27; // AIN1
uint8_t Ain2Pin = 26; // AIN2

// --- Pines dummy del motor B (no usados) ---
uint8_t motorPinB = 0;
uint8_t motorChannelB = 1;
uint8_t Bin1Pin = 0;
uint8_t Bin2Pin = 0;

// --- Filtros ---
Filter LPF;

#endif // __DEFINITIONS_H__
