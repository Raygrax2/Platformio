#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

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

enum ControlMode
{
  MODE_MANUAL_PWM = 0, // Direct PWM control
  MODE_SPEED = 1,      // Speed control with PID
  MODE_POSITION = 2    // Position control with PID
};

TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 20000, // Frecuencia PWM motor A (Hz)
    .bit_resolution = LEDC_TIMER_10_BIT,
    .mode = LEDC_LOW_SPEED_MODE};
// Global objects
QuadratureEncoder Encoder;
SimpleTimer timer;
BDCMotor motorA;
SimplePWM pwmmotorA;
PID_CAYETANO PID_2;
SimpleUART UART(115200);

// --- Buffers UART o debug ---
char rxbuf[32];
uint8_t len = 0;

// --- Parámetros del encoder ---
uint8_t EncoderPIN[2] = {32, 14}; // Pines del encoder
uint8_t AIN[2] = {27, 26};
uint8_t CHA[2] = {0, 1};
uint32_t dt = 5000; // 2 ms (consistent with timer period)

// Constant floats
float PID_GAINS[3] = {1.0f, 0.0f, 0.0f}; // Kp, Ki, Kd
float Kp = 1.0f; // initial proportional gain
float Ki = 0.0f; // initial integral gain
float Kd = 0.0f; // initial derivative gain
float error = 0.0f;
float speed_raw = 0.0f;
float current_position = 0.0f;      // in degrees
int current_mode = MODE_MANUAL_PWM; // default mode
float ref = -70.0f;                 // reference position (deg)
float u = 0.0f;

// --- Filtros --- Not used now
/* Filter LPF;
float LPF_coeffs_a[2] = {1.0000f, -0.7608f}; // [a0, a1] where a0 = 1
float LPF_coeffs_b[2] = {0.1196f, 0.1196f};  // [b0, b1] */

#endif // __DEFINITIONS_H__
