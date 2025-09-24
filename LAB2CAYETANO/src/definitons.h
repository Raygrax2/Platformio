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
SimpleTimer timer;





#endif // __DEFINITONS_H__
