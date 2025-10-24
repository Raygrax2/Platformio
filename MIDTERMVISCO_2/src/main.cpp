#include "definitons.h"
#include "esp_timer.h"



// Timer ISR wrapper used by SimpleTimer
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    // disable watchdog for long operations during dev
    esp_task_wdt_deinit();

   
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000); // 10,000 us = 10 ms tick

    
    JOY.setup(PinX, PinY, Button);
    JOY.calibrate(1000000); // 1 second calibration
    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);
    PWM_Stepper_ROT.setDuty(0.0f);
    PWM_Stepper_UP.setDuty(0.0f);
    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);
    STEPPER_ROT_DIR.set(1);
    STEPPER_UP_DIR.set(1);
    Motor_spin.setup(Spin_MotorPIns, Spin_MotorCH, PWM_Spin);
    enco.setup(Encoder_PIn, ENCODER_DEGREES_PER_EDGE);
    PID.setup(Gain, 10000);

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
                currentState += 1;
            }
            float measured_rpm = enco.getSpeed();
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
                float current_viscocity, 
                float error = setpoint - measured_rpm;
                float u = PID.computedU(error); 
                Motor_spin.setSpeed(u);
                break;
            }

            case STATE_MEASUREMENT_DISPLAY:
                Motor_spin.setSpeed(0.0f);
                break;

            case STATE_SAMPLE_REMOVAL:
            case STATE_CYLINDER_CLEANING:
            case STATE_CYLINDER_DRYING:
            case STATE_PROCESS_RESTART:
                Motor_spin.setSpeed(0.0f);
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                break;

            default:
                break;

            } // switch
        }
    }
}
