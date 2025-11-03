#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

/*
  definitions.h
  Global constants, pin definitions, object declarations and initial variables.
  Keep only non-runtime-changing definitions here. Peripheral setup should
  happen in main (app_main).
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "SimpleTimer.h"
#include "SimpleGPIO.h"
#include "Joystick.h"
#include "SimplePWM.h"
#include "BDCMotor.h"
#include "QuadratureEncoder.h"
#include "PID_CAYETANO.h"
#include "SimpleUART.h"
#include "SimpleADC.h"

// ------------------- ENUM VISCOMETER STATES -------------------
enum ViscometerState : int {
    VS_WELCOME = 0,       // 0 welcome / power on
    VS_PLACE_SAMPLE = 1,  // 1 place sample
    VS_SAMPLE_OK = 2,     // 2 sample confirmed placed
    VS_LOWER_CYL = 3,     // 3 lower cylinder (user control)
    VS_STIR = 4,          // 4 stir (30s @ <=160 RPM)
    VS_MEASURE = 5,       // 5 measure (5s @ 20-30 RPM)
    VS_REPORT = 6,        // 6 display average & wait for removal
    VS_REMOVE = 7,        // 7 raise and remove sample
    VS_CLEAN_ROTATE = 8,  // 8 rotate to cleaning station / lower
    VS_CLEAN_TIMER = 9,   // 9 clean soak timer (60s)
    VS_DRY_TIMER = 10     // 10 dry timer (120s) then return to start
};

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

// ------------------- GLOBAL OBJECTS (declare here) -------------------
static SimpleTimer timer;                 // system periodic timer (used with ISR)
static Joystick JOY;                      // joystick object
static SimplePWM PWM_Stepper_ROT;         // carousel rotation stepper PWM
static SimpleGPIO STEPPER_ROT_DIR;        // carousel rotation direction
static SimplePWM PWM_Stepper_UP;          // vertical stepper PWM
static SimpleGPIO STEPPER_UP_DIR;         // vertical stepper direction
static BDCMotor Motor_spin;               // spinner motor abstraction
static QuadratureEncoder enco;            // encoder for spinner motor
static PID_CAYETANO PID;                  // PID for rpm control
static SimpleUART UART_MESSAGE(115200);   // serial debug
static SimpleADC IV;                      // ADC object for viscosity sensor

// ------------------- PINS & HW CONSTANTS (edit here if needed) -------------------
static const uint8_t PinX = 26;      // Joystick X axis
static const uint8_t PinY = 27;      // Joystick Y axis
static const uint8_t Button = 32;    // Joystick button

static const uint8_t PWM_ROT_PIN = 25;
static const uint8_t ROT_PIN_DIR = 33;

static const uint8_t PWM_UP_PIN = 18;
static const uint8_t UP_PIN_DIR = 19;

static const uint8_t IB_PIN = 14;    // ADC input pin for sensor

// Spin motor pins and channels for BDC driver / PWM
static const uint8_t Spin_MotorPIns[2] = {5, 23};
static const uint8_t Spin_MotorCH[2] = {2, 3};

// Encoder pins and degrees-per-edge
static uint8_t Encoder_PIn[2] = {16, 17};
static float ENCODER_DEGREES_PER_EDGE = 0.36445f; // degrees per edge

// ------------------- PID GAIN ARRAY (for PID.setup) -------------------
static float Gain[3] = {1.5f, 0.5f, 0.5f}; // Kp, Ki, Kd (tune as needed)

// ------------------- TIMING / DURATION CONSTANTS -------------------
// Tick is 10 ms (configured in main via timer.startPeriodic(10000))
static const uint32_t TICK_MS = 10u;
static inline uint32_t MS_TO_TICKS(uint32_t ms) { return (ms + (TICK_MS-1)) / TICK_MS; }

static const uint32_t STIR_MS = 30 * 1000;      // 30 seconds
static const uint32_t MEASURE_MS = 5 * 1000;    // 5 seconds
static const uint32_t CLEAN_MS = 60 * 1000;     // 60 seconds
static const uint32_t DRY_MS = 120 * 1000;      // 120 seconds

static const uint32_t STIR_TICKS = MS_TO_TICKS(STIR_MS);
static const uint32_t MEASURE_TICKS = MS_TO_TICKS(MEASURE_MS);
static const uint32_t CLEAN_TICKS = MS_TO_TICKS(CLEAN_MS);
static const uint32_t DRY_TICKS = MS_TO_TICKS(DRY_MS);

// ------------------- INITIAL (GLOBAL) VARIABLES -------------------
static int currentstate = VS_WELCOME;    // start state at power-up
static int laststate = -1;               // track state transitions
float currentRPM = 0.0f;                 // measured RPM (filtered if needed)
static float currentViscosityCP = 0.0f;  // final displayed viscosity

// ------------------- AVERAGING / SAMPLING -------------------
// accumulate viscosity samples (float to avoid truncation)
static int visc_count = 0;               // number of viscosity samples collected
static float vis_acc = 0.0f;             // accumulator of sampled viscosity
static float viscosity_avg = 0.0f;       // computed mean viscosity
static bool viscosity_ready = false;     // flag when avg available
static float last_mu_sample = 0.0f;      // most recent per-sample viscosity

// ------------------- MOTOR CONTROL / SETPOINT -------------------
static float motor_target_rpm = 0.0f;    // desired motor RPM
static float motor_duty = 0.0f;          // duty used if open-loop control required

// safety limits
static const float MAX_STIR_RPM = 160.0f;
static const float MIN_MEAS_RPM = 20.0f;
static const float MAX_MEAS_RPM = 30.0f;

// ------------------- STATE TIMER -------------------
// non-blocking tick counter used to measure durations within a state
static uint32_t state_tick = 0; // incremented every timer tick (10 ms)

// ------------------- MISC / BUFFERS -------------------
static char Buffer[64];                 // general purpose UART/text buffer

#endif // __DEFINITIONS_H__
