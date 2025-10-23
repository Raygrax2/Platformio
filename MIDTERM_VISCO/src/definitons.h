#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
  definitions.h
  Objetos globales, pines y coeficientes de filtro.
  Solo editar este archivo y main.cpp.
*/
extern "C"
{
#include "i2c_lcd.h"
#include <stdio.h>
#include "driver/i2c.h"
}


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "SimpleRGB.h"
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
enum ViscometerState {
    WELCOME = 0,               // 1. Energized, waiting to start
    SAMPLE_PLACEMENT =1 ,      // 2. Waiting for sample
    SAMPLE_CONFIRMATION = 2,   // 2b. Confirm sample placement
    CYLINDER_LOWERING = 3,     // 3. Lowering cylinder
    STIRRING = 4,              // 4. Mixing process
    MEASURING = 5,             // 5. Viscosity measurement
    SHOW_RESULTS = 6,          // 6. Display results
    SAMPLE_REMOVAL = 7,        // 7. Removing sample
    CLEANING = 8,              // 8. Cleaning process
    DRYING = 9,                // 9. Drying process
    READY_FOR_NEXT = 10,        // 10. Process finished
};


TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 500, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE};
TimerConfig RGBTIMER{
    .timer = LEDC_TIMER_0,
    .frequency = 500,
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE};

QuadratureEncoder Encoder;
SimpleTimer timer;
BDCMotor motorA;
SimplePWM pwmmotorA;
PID_CAYETANO PID_2;
SimpleRGB RGB_LED;
SimpleUART UART_1(115200);

// --- Buffers UART o debug ---
char Buffer[32];
uint8_t len = 0;

uint8_t RGB_PINS[3] = {26, 25, 33};
uint8_t color[3] = {255, 0, 0};

uint8_t Ain1Pin = 27; // AIN1
uint8_t Ain2Pin = 26; // AIN2
uint8_t AIN[2] = {Ain1Pin, Ain2Pin};
uint8_t CHA[2] = {0, 1};
uint8_t PWM_CH[3] = {2, 3, 4};
SimpleUART UART(115200);

#endif // __DEFINITIONS_H__
