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
    STATE_MANUAL_MOVEMENT,
    STATE_PROCESS_RESTART,
    STATE_COUNT
};
int current_state = STATE_POWER_ON;
SimpleTimer timer;
SimpleRGB RGB;

uint8_t RGB_Pins[3] = {26,34,23};
uint8_t RGB_CH[3] = {0,1,2};
static TimerConfig PWM_Timer_RGB{
    .timer = LEDC_TIMER_0,
    .frequency = 650,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
uint64_t len = 0;

SimpleUART UART(115200);

#endif // __DEFINITIONS_H__
