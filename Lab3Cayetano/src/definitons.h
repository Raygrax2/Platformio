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
#include "QuadratureEncoder.h"
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
QuadratureEncoder Encoder;
SimpleTimer timer;


char Buffer[32];
uint8_t len = 0;
float Deg_per_edge = 360.0f / 2048.0f; // Grados por flanco (2048 pulsos por vuelta, 4 flancos por pulso)
// --- Pines ---
uint8_t EncoderPIN[2] = {34,36} ; // Pin del encoder

// --- Filtros (objetos) ---
Filter LPF;

// --- Coeficientes (ya definidos por ti) ---
// Low Pass Filter (IIR 2º orden)
static float a_LPF[3] = {1.0f, -1.3073f, 0.4918f};
static float b_LPF[3] = {0.0461f, 0.0923f, 0.0461f};


#endif // __DEFINITIONS_H__
