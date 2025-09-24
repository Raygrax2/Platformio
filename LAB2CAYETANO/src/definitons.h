#ifndef __DEFINITONS_H__
#define __DEFINITONS_H__

#include <stdio.h>
#include <string.h>
#include "SimpleRGB.h"
#include "SimpleTimer.h"
#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "SimpleUART.h"
#include "SimplePWM.h"
#include "Filter.h"
//Pins should be changed based on disponibilty
SimplePWM Square;
SimpleADC ADC;
uint8_t ADC_pin = 24; 
uint8_t pinSquare = 18;
uint8_t channelSquare = 0;
TimerConfig timer
{
    .frequency = 4,// 4 hz frequency
    .bit_resolution = LEDC_TIMER_8_BIT,
};






#endif // __DEFINITONS_H__
