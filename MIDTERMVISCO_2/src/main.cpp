#include "definitons.h"
#include "esp_timer.h"

// Helper: publish numeric state,RPM,viscosity via UART/printf
static void publishState()
{
    // Format: STATE:<n>,RPM:<int>,VISC:<float>\n
    printf("STATE:%d,RPM:%d,VISC:%.2f\n", (int)currentState, currentRPM, currentViscosityCP);
}

// Transition function (button-driven)
static void transitionState()
{
    currentState = static_cast<ViscometerState>((currentState + 1) % STATE_COUNT);
    // when transitioning, publish numeric state and current metrics
    publishState();
}

// Timer ISR wrapper used by SimpleTimer
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    // disable watchdog for long operations during dev
    esp_task_wdt_deinit();

    // -------------------- SETUP (all setups must be here) --------------------
    // Timer tick setup (10 ms tick)
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000); // 10,000 us = 10 ms tick

    // Joystick setup (pins & button)
    JOY.setup(PinX, PinY, Button);
    JOY.calibrate(1000000); // 1 second calibration

    // Steppers setup (PWM + direction pins) - only configure here, don't start
    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);
    PWM_Stepper_ROT.setDuty(0.0f);
    PWM_Stepper_UP.setDuty(0.0f);

    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);
    STEPPER_ROT_DIR.set(1);
    STEPPER_UP_DIR.set(1);

    // Spin motor (BDCMotor) setup (pins/channels + pwm config)
    Motor_spin.setup(Spin_MotorPIns, Spin_MotorCH, PWM_Spin);

    // Encoder setup: pass pins and degrees per edge as requested
    // Assuming QuadratureEncoder::setup(const uint8_t pins[2], float degrees_per_edge)
    enco.setup(Encoder_PIn, ENCODER_DEGREES_PER_EDGE);

    // PID setup using your library: Gains array and sample time 10000 (as requested)
    PID.setup(Gain, 10000);

    // Publish initial state
    publishState();

    // State timer baseline
    uint64_t state_start_us = esp_timer_get_time();

    // -------------------- MAIN LOOP --------------------
    while (1)
    {
        // wait for timer tick
        if (timer.interruptAvailable())
        {
            

            // update joystick state (prints internally if JOY.result() does)
            JOY.result();
            // Button -> state transition (one-shot implemented in JOY.Pressed())
            if (JOY.Pressed())
            {
                transitionState();
                state_start_us = esp_timer_get_time();
            }

            // update measured rpm from encoder
            // we assume enco.getspeed() returns RPM as float (per your earlier mention)
            float measured_rpm = enco.getSpeed();
            if (measured_rpm < 0.0f)
                measured_rpm = 0.0f;
            currentRPM = (int)(measured_rpm + 0.5f);

            // -------------------- STATE MACHINE BEHAVIOR --------------------
            uint64_t now_us = esp_timer_get_time();

            switch (currentState)
            {
            case STATE_POWER_ON:
                // keep everything off
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                Motor_spin.setSpeed(0.0f);
                break;

            case STATE_SAMPLE_PLACEMENT:
            case STATE_SAMPLE_POSITIONED:
                // idle waiting for user action
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                Motor_spin.setSpeed(0.0f);
                break;

            case STATE_CYLINDER_LOWERING:
                // pass it to the manual movement,
                //  joystick controls turret and vertical stepper ONLY in these states
                if (JOY.zero())
                {
                    PWM_Stepper_ROT.setDuty(0.0f);
                    PWM_Stepper_UP.setDuty(0.0f);
                }
                else if (JOY.Left())
                {
                    STEPPER_ROT_DIR.set(1);
                    PWM_Stepper_ROT.setDuty(50.0f);
                    PWM_Stepper_ROT.setFrequency(650);
                }
                else if (JOY.Right())
                {
                    STEPPER_ROT_DIR.set(0);
                    PWM_Stepper_ROT.setDuty(50.0f);
                    PWM_Stepper_ROT.setFrequency(650);
                }
                else if (JOY.Up())
                {
                    STEPPER_UP_DIR.set(1);
                    PWM_Stepper_UP.setDuty(50.0f);
                    PWM_Stepper_UP.setFrequency(650);
                }
                else if (JOY.Down())
                {
                    STEPPER_UP_DIR.set(0);
                    PWM_Stepper_UP.setDuty(50.0f);
                    PWM_Stepper_UP.setFrequency(650);
                }
                break;

            case STATE_MANUAL_MOVEMENT:
                // joystick controls turret and vertical stepper ONLY in these states
                if (JOY.zero())
                {
                    PWM_Stepper_ROT.setDuty(0.0f);
                    PWM_Stepper_UP.setDuty(0.0f);
                }
                else if (JOY.Left())
                {
                    STEPPER_ROT_DIR.set(1);
                    PWM_Stepper_ROT.setDuty(50.0f);
                    PWM_Stepper_ROT.setFrequency(650);
                }
                else if (JOY.Right())
                {
                    STEPPER_ROT_DIR.set(0);
                    PWM_Stepper_ROT.setDuty(50.0f);
                    PWM_Stepper_ROT.setFrequency(650);
                }
                else if (JOY.Up())
                {
                    STEPPER_UP_DIR.set(1);
                    PWM_Stepper_UP.setDuty(50.0f);
                    PWM_Stepper_UP.setFrequency(650);
                }
                else if (JOY.Down())
                {
                    STEPPER_UP_DIR.set(0);
                    PWM_Stepper_UP.setDuty(50.0f);
                    PWM_Stepper_UP.setFrequency(650);
                }
                break;

            case STATE_STIR_LIQUID:
                // Stirring step: you may choose to open-loop or use PID.
                Motor_spin.setSpeed(100.0f);
                break;

            case STATE_MEASURE_VISCOSITY:
            {
                // Apply PID to reach 180 RPM using encoder speed
                float setpoint = 180.0f;
                // error as you requested: 180 - encoder.getspeed()
                float error = setpoint - measured_rpm;
                float u = PID.computedU(error); // your PID returns control value u
                // apply u directly as motor speed setpoint (per your instruction)
                Motor_spin.setSpeed(u);
                float viscocity;

                // (optional) collect measurement samples for viscosity calculation here
                // For now currentViscosityCP can be updated elsewhere; keep it as-is or compute
                break;
            }

            case STATE_MEASUREMENT_DISPLAY:
                // measurement finished; motor off
                Motor_spin.setSpeed(0.0f);
                break;

            case STATE_SAMPLE_REMOVAL:
            case STATE_CYLINDER_CLEANING:
            case STATE_CYLINDER_DRYING:
            case STATE_PROCESS_RESTART:
                // default idle behavior; user or later states will handle
                Motor_spin.setSpeed(0.0f);
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                break;

            default:
                break;

            } // switch
        }
        // Note: publishState() already called on transitions. If you want periodic updates,
        // uncomment the next line.
        // publishState();
    } // while
}
