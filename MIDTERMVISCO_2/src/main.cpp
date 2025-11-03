#include "definitons.h"
#include "esp_timer.h"
extern "C"
{
#include "i2c_lcd.h"
}

// Timer ISR wrapper used by SimpleTimer
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    // disable watchdog for long operations during dev
    esp_task_wdt_deinit();

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(1000); // 10,000 us = 10 ms tick
    JOY.setup(PinX, PinY, Button);
    JOY.calibrate(1000000); // 1 second calibration
    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);
    PWM_Stepper_ROT.setDuty(0.0f);
    PWM_Stepper_UP.setDuty(0.0f);
    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);
    PID.setup(Gain, 1000 / 1000000);
    char Buffer_message_1[32];
    char Buffer_message_2[32];
    IV.setup(IB_PIN);
    uint64_t prev_time = esp_timer_get_time();
    uint64_t current_time;
    float SB1 = 0;
    float mu_rel = 0;
    float mu_acc = 0;
    float mu_sta = 0;
    float mu_tot = 0;

    STEPPER_ROT_DIR.set(1);
    STEPPER_UP_DIR.set(1);
    Motor_spin.setup(Spin_MotorPIns, Spin_MotorCH, PWM_Spin);
    enco.setup(Encoder_PIn, ENCODER_DEGREES_PER_EDGE);
    PID.setup(Gain, 10000);
    lcd_init();
    lcd_clear();
    lcd_put_cursor(0, 0);

    // -------------------- MAIN LOOP --------------------
    while (1)
    {
        // wait for timer tick
        if (timer.interruptAvailable())
        {
            JOY.result();
            if (JOY.Pressed())
            {
                if (currentstate >= 9)
                    currentstate = 0;
                else
                    currentstate++;
            }

            float measured_rpm = enco.getSpeed();
            currentRPM = measured_rpm * 0.17;
            switch (currentstate)
            {
            case 0:
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                Motor_spin.setSpeed(0.0f);
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Welcome");
                break;

            case 1:
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Place a Sample");
                break;

            case 2:
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Sample OK");
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                Motor_spin.setSpeed(0.0f);
                break;

            case 3:
                lcd_clear();
                lcd_put_cursor(0, 0);

                lcd_send_string("Down cylinder");

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

            case 4:
            {
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                currentRPM = enco.getSpeed() * 0.17;
                error = 180 - enco.getSpeed();

                u = PID.computedU(error);
                Motor_spin.setSpeed(u);
                SB1 = IV.read(ADC_READ_RAW);

                if (mu_sta < 1000)
                {
                    mu_rel = 0.7688f * SB1 - 848.81f; //
                    mu_acc += mu_rel;
                    mu_sta++;
                }
                printf("%f", mu_rel);
                
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Measure");

                lcd_put_cursor(1, 0);
                current_time = esp_timer_get_time();
                if (current_time - prev_time >= 250000)
                {
                    prev_time = current_time;
                    sprintf(Buffer_message_1, "RPM: %.1f", currentRPM);
                }
                lcd_send_string(Buffer_message_1);
                //
                break;
            }

            case 5:
            {
                mu_tot = mu_acc / 100;
                lcd_clear();
                lcd_put_cursor(0, 0);
                sprintf(Buffer_message_2, "VISCOSITY: %f", mu_rel);
                lcd_send_string(Buffer_message_2);
                Motor_spin.setSpeed(0.0f);
                break;
            }

            case 6:
            {
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Remove sample");
                Motor_spin.setSpeed(0.0f);
                mu_acc = 0;
                mu_sta = 0;
                break;
            }

            case 7:
            {
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Clean Control");
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
            }

            case 8:
            {
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Dry Control");
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
            }

            case 9:
            {
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Process Complete");
                Motor_spin.setSpeed(0.0f);
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                break;
            }

            default:
                break;
            }

        } // switch
        printf("%d,%f,%f,%f\n", currentstate, currentRPM, mu_rel,SB1);
    }
}
