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
//Timer for PWM
TimerConfig PWM_Time{
    .timer = LEDC_TIMER_3,
    .frequency = 100,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE};
//Pins should be changed based on disponibilty

SimplePWM Square;
SimpleADC ADC;
SimpleUART UART_RAW(115200);
SimpleUART UART_Filtered(115200);


uint8_t ADC_pin = 24; 
uint8_t pinSquare = 18;
uint8_t channelSquare = 0;
SimpleTimer timer;



Filter LPF;
Filter HPF;
Filter MA;


//Filter Coefficients
//Low Pass Filter
float a_LPF[3] = {1, -1.3073, 0.4918};
float b_LPF[3] = {0.0461, 0.0923, 0.0461};
//High Pass Filter
float b_HPF[3] = {0.978030726982, -1.956061453964, 0.978030726982};
float a_HPF[3] = {1.0, -1.955578240315, 0.956543676511};
//Moving Average Filter
float b_MA[3] = {0.3333, 0.3333, 0.3333};
float a_MA[3] = {1.0, 0.0, 0.0};


#endif // __DEFINITONS_H__
