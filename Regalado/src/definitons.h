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

TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 20000,                 // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
BDCMotor motor;
uint8_t motor_pins[2] = {18, 19}; // Pines de control del motor
uint8_t motor_ch[2] = {0, 1};      // Canales PWM del motor

SimpleADC adc;
uint8_t adc_pin = 34; // Pin de lectura ADC
SimpleTimer timer;
float lower_treshold = 2000.0; // Umbral inferior en mV
float upper_treshold = 4000.0; // Umbral superior en mV

#endif // __DEFINITIONS_H__
