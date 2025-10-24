#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{

    esp_task_wdt_deinit();
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000);
    JOY.setup(PinX, PinY, Button);
    JOY.calibrate(1000000);
    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);
    PWM_Stepper_UP.setDuty(50.0f);
    PWM_Stepper_ROT.setDuty(50.0f);
    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);
    STEPPER_UP_DIR.set(1);
    STEPPER_ROT_DIR.set(1);
    Motor_spin.setup(Spin_MotorPIns, Spin_MotorCH, PWM_Spin);
    Motor_spin.setSpeed(0.0f);
    enco.setup(Encoder_PIn, 0.36445);
    
    PID.setup(Gain, 10000);

    while (1)
    {
        // JOY.result();
        Speed = enco.getSpeed();
        error = 180 - Speed;
        u = PID.computedU(error);
        printf("%f,%f,%f\n",error, Speed,u);
        Motor_spin.setSpeed(u);
        /* if (JOY.Pressed())
        {
            // PRESSED BECAUSE OF THE INVERSE LOGIC PULL UP RESISTOR
            printf("PRESSED\n");
        }
        if (JOY.zero())
        {
            PWM_Stepper_ROT.setDuty(0.0f);
            PWM_Stepper_UP.setDuty(0.0f);
        }
        else if (JOY.Left())
        {
            PWM_Stepper_ROT.setDuty(50.0f);
            STEPPER_ROT_DIR.set(1);
            PWM_Stepper_ROT.setFrequency(650);
            Motor_spin.setSpeed(-100.0f);
        }
        else if (JOY.Right())
        {
            PWM_Stepper_ROT.setDuty(50.0f);
            STEPPER_ROT_DIR.set(0);
            PWM_Stepper_ROT.setFrequency(650);

            Motor_spin.setSpeed(100.0f);
        }
        else if (JOY.Up())
        {
            PWM_Stepper_UP.setDuty(50.0f);
            STEPPER_UP_DIR.set(1);
            PWM_Stepper_UP.setFrequency(650);
        }
        else if (JOY.Right())
        {
            PWM_Stepper_UP.setDuty(50.0f);
            STEPPER_UP_DIR.set(0);
            PWM_Stepper_UP.setFrequency(650);
        } */
    }
}
