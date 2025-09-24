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
SimpleUART UART_RAW(115200);


uint8_t ADC_pin = 24; 
uint8_t pinSquare = 18;
uint8_t channelSquare = 0;
Filter LPF;
std::vector<float> a_LPF = {1.0f , -0.942809f};
std::vector<float> b_LPF = {};
std::vector<float> a_HPF = {};
std::vector<float> b_HPF = {};
std::vector<float> a_MA = {};
std::vector<float> b_MA = {};


TimerConfig timer
{
    .frequency = 4,// 4 hz frequency
    .bit_resolution = LEDC_TIMER_8_BIT,
};






#endif // __DEFINITONS_H__
