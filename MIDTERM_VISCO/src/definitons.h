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
#include "TCS34725.h"
#include "QuadratureEncoder.h"
#include "SimpleTimer.h"
#include "Hbridge.h"
#include "SimpleI2C.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "PID_CAYETANO.h"
#include "SimpleUART.h"
#include "BDCMotor.h"
#include "SimplePID.h"
#include "SimplePWM.h"
#include "Filter.h"

SimpleTimer timer;
uint64_t prev_time;
uint64_t current_time;
uint64_t dt_us;
uint16_t c,r,g,b; 
SimpleI2C i2c;
TCS34725 Colorsensor;
uint64_t len = 0;

SimpleUART UART(115200);

#endif // __DEFINITIONS_H__
