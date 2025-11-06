#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
  definitions.h
  Minimal global constants, pin definitions, object declarations and initial variables.
  Only items referenced by main.cpp are declared here to keep it efficient.
*/

#include <stdio.h>
#include <stdint.h>

#include "SimpleTimer.h"
#include "SimpleGPIO.h"
#include "BDCMotor.h"
#include "SimplePWM.h"
#include "Stepper.h"
#include "QuadratureEncoder.h"
#include "PID_CAYETANO.h"
#include "SimpleUART.h"
#include "TCS34725.h"

// ------------------- TIMERS / PWM CONFIG -------------------
static TimerConfig PWM_TimerA{
    .timer = LEDC_TIMER_0,
    .frequency = 650,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

static TimerConfig PWM_UP_STEP{
    .timer = LEDC_TIMER_1,
    .frequency = 650,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};

static TimerConfig PWM_Spin{
    .timer = LEDC_TIMER_2,
    .frequency = 2000,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
static TimerConfig PUMP_PWM_TIMER{
    .timer = LEDC_TIMER_2,
    .frequency = 2000,
    .bit_resolution = LEDC_TIMER_14_BIT,
    .mode = LEDC_LOW_SPEED_MODE
};
// ------------------- GLOBAL OBJECTS -------------------
static SimpleTimer timer;
static TCS34725 Color_sensor;
static SimplePWM PWM_Stepper_UP;
static BDCMotor Motor_spin;
static BDCMotor Pump;
static SimplePWM PWM_Stepper_ROT;
static SimpleGPIO STEPPER_UP_DIR;
static SimpleGPIO STEPPER_ROT_DIR;
static SimpleGPIO emg_relay;
static Stepper Stepper_Up;              // your Stepper wrapper
static Stepper Stepper_Rot;
static QuadratureEncoder enco;
static PID_CAYETANO PID;
static SimpleUART UART_MESSAGE(115200);

// ------------------- PINS & HW CONSTANTS -------------------
static const uint32_t dt = 10000U;      // 10 ms tick (microseconds passed to timer.startPeriodic)
static const uint8_t INT_STEP_PIN = 5;  // external interrupt encoder pin (example)
static const uint8_t PWM_UP_PIN = 18;
static const uint8_t PWM_ROT_PIN = 25;
static const uint8_t UP_PIN_DIR = 19;
static const uint8_t ROT_PIN_DIR = 33;
static const uint8_t Emg_stop_PIn = 8;      // pump output pin example
static const uint8_t AIN[2] = {14,17};
static const uint8_t Pump_PIN[2] = {14,17};


static const uint8_t BDC_Motor_CH [2] = {0,1};
static const uint8_t Pump_CH [2] = {2,3};





// Encoder pins and scale (fill with your actual pin numbers if different)
static uint8_t EncoderPins[2] = {16, 17};
static const float ENCODER_DEGREES_PER_EDGE = 0.36445f; // degrees per edge (from earlier value)

// Stepper params used for setup
static const float STEPPER_MICROSTEP_DEG = 1.8f; // degrees per step (full step)

// PID default gains (can be tuned)
static float PID_GAINS[3] = {1.5f, 0.5f, 0.5f};

// ------------------- COMM & CONTROL VARIABLES (initialized) -------------------
// Send buffer elements (R, G, B)
uint16_t r = 0;
uint16_t g = 0;
uint16_t b = 0;
uint16_t c = 0;

static int send_R = 0;
static int send_G = 0;
static int send_B = 0;

// Position and speed to send
static float send_posx = 0.0f;
static float send_posy = 0.0f;
static float send_speed = 0.0f;

// Received control values
static float recv_ref_rpms = 0.0f;
static int recv_emergency_stop = 0; // 0 = OK, 1 = stop
static int recv_send_water = 0;     // 0/1
static float recv_ref_posx = 0.0f;
static float recv_ref_posy = 0.0f;

// State machine
static int currentstate = 0;

// Misc telemetry buffer (for printf formatting if needed)
static char txbuf[128];
static char rxbuf[128];

#endif // __DEFINITIONS_H__
