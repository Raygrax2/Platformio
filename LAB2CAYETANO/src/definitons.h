#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
  definitions.h
  Objetos globales, pines y coeficientes de filtro.
  Solo editar este archivo y main.cpp (tal como solicitaste).
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "SimpleRGB.h"
#include "SimpleTimer.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "SimpleUART.h"
#include "SimplePWM.h"
#include "Filter.h"

// --- Timer para PWM (configuración base) ---
TimerConfig PWM_Time{
    .timer = LEDC_TIMER_3,
    .frequency = 4,                  // Frecuencia de la señal cuadrada por defecto (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

// --- Objetos globales ---
SimplePWM Square;
SimpleADC ADC;
SimpleUART UART_RAW(115200);       // UART para enviar datos (raw + filtered)
SimpleTimer timer;
SimpleTimer timer2;



char Buffer[32];
uint8_t len = 0;


static const uint8_t ADC_pin = 25;
static const uint8_t pinSquare = 17;
static const uint8_t channelSquare = 0;

// --- Filtros (objetos) ---
Filter LPF;
Filter HPF;
Filter MA;

// --- Coeficientes (ya definidos por ti) ---
// Low Pass Filter (IIR 2º orden)
static float a_LPF[3] = {1.0f, -1.3073f, 0.4918f};
static float b_LPF[3] = {0.0461f, 0.0923f, 0.0461f};

// High Pass Filter (IIR 2º orden)
static float b_HPF[3] = {1.0f, -2.0f, 1.0f};
static float a_HPF[3] = {1.0f, -1.8055f, 0.8149f};

// Moving Average filter (3 muestras)
static float b_MA[4] = {0.25f, 0.25f, 0.25f, 0.25f};
static float a_MA[1] = {1.0f};

#endif // __DEFINITIONS_H__
