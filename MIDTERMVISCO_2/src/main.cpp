#include "definitons.h"
#include "esp_timer.h"
extern "C"

// Timer ISR wrapper used by SimpleTimer
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    // disable watchdog for long operations during dev
    esp_task_wdt_deinit();

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000); // 10,000 us = 10 ms tick
    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);
    PWM_Stepper_ROT.setDuty(0.0f);
    PWM_Stepper_UP.setDuty(0.0f);
    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);
    char Buffer_message_1[32];
    char Buffer_message_2[32];
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

            float measured_rpm = enco.getSpeed();
            currentRPM = measured_rpm;
            switch (currentstate)
            {
            case 0:  //Elevation
                PWM_Stepper_UP.setDuty(0.0f);
                break;

            case 1:  //Rotation
                PWM_Stepper_ROT.setDuty(0.0f);

                break;

            case 2:  //Pump
    

                break;

            case 3:    //Velocity
                error = ref - current_position; 
                u = PID_2.computedU(error);

            break;

            case 4:
                default:
                
            break;

        } // switch
        printf("%f\n", currentRPM);
    }
}
