#include "definitons.h"
#include "esp_timer.h"
extern "C"
{
#include "i2c_lcd.h"
}

// Timer ISR wrapper used by SimpleTimer
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

// helper to update motor setpoint using available Motor_spin API.
// NOTE: adapt this if your Motor_spin expects duty instead of RPM.
static void applyMotorSetpoint(float rpm)
{
    if (rpm <= 0.0f)
    {
        Motor_spin.setSpeed(0.0f);
    }
    else
    {
        // clamp to safety
        if (rpm > MAX_STIR_RPM) rpm = MAX_STIR_RPM;
        Motor_spin.setSpeed(rpm);
    }
}

extern "C" void app_main()
{
    // disable watchdog for long operations during dev
    esp_task_wdt_deinit();

    // Timer setup: tick = 10 ms
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(TICK_MS * 1000); // microseconds

    // peripherals
    JOY.setup(PinX, PinY, Button);
    JOY.calibrate(1000000); // 1 second calibration

    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);
    PWM_Stepper_ROT.setDuty(0.0f);
    PWM_Stepper_UP.setDuty(0.0f);

    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);

    IV.setup(IB_PIN);

    STEPPER_ROT_DIR.set(1);
    STEPPER_UP_DIR.set(1);

    Motor_spin.setup(Spin_MotorPIns, Spin_MotorCH, PWM_Spin);
    enco.setup(Encoder_PIn, ENCODER_DEGREES_PER_EDGE);
    PID.setup(Gain, 10000);

    lcd_init();
    lcd_clear();
    lcd_put_cursor(0, 0);

    // initialize globals
    currentstate = VS_WELCOME;
    laststate = -1;
    state_tick = 0;
    visc_count = 0;
    vis_acc = 0.0f;
    viscosity_avg = 0.0f;
    viscosity_ready = false;
    motor_target_rpm = 0.0f;

    // main loop
    while (1)
    {
        if (timer.interruptAvailable())
        {
            // advance timer tick
            state_tick++;

            // update joystick
            JOY.result();

            // read encoder and compute filtered RPM (same factor used previously)
            float measured_rpm_raw = enco.getSpeed();
            currentRPM = measured_rpm_raw * 0.17f; // keep same conversion

            // handle button press: we use explicit transitions per state to avoid accidental increments
            bool btn = JOY.Pressed();

            // state machine
            switch (currentstate)
            {
                case VS_WELCOME:
                {
                    // On power up: welcome message and ask to place sample
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Welcome");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Place a sample");
                    }
                    // wait for user to press button to signal ready to place sample
                    if (btn)
                    {
                        currentstate = VS_PLACE_SAMPLE;
                    }
                    break;
                }

                case VS_PLACE_SAMPLE:
                {
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Place the sample");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Press Btn when done");
                    }
                    if (btn)
                    {
                        // user indicates sample placed
                        currentstate = VS_SAMPLE_OK;
                    }
                    break;
                }

                case VS_SAMPLE_OK:
                {
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Confirm position");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Press Btn if OK");
                        // ensure motors stopped
                        PWM_Stepper_ROT.setDuty(0.0f);
                        PWM_Stepper_UP.setDuty(0.0f);
                        Motor_spin.setSpeed(0.0f);
                    }
                    if (btn)
                    {
                        currentstate = VS_LOWER_CYL;
                    }
                    break;
                }

                case VS_LOWER_CYL:
                {
                    // user uses Y axis to lower cylinder; press button when submerged
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Lower cylinder");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Use Y axis, Btn=OK");
                    }

                    // control vertical stepper with joystick
                    if (JOY.zero())
                    {
                        PWM_Stepper_UP.setDuty(0.0f);
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

                    if (btn)
                    {
                        // user confirms submerged; go to stir preparation
                        currentstate = VS_STIR;
                    }
                    break;
                }

                case VS_STIR:
                {
                    // start stirring for STIR_TICKS at <= MAX_STIR_RPM
                    if (laststate != currentstate)
                    {
                        state_tick = 0;
                        motor_target_rpm = MAX_STIR_RPM; // cap at 160 rpm
                        applyMotorSetpoint(motor_target_rpm);
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Stirring 30s");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Mixing...");
                    }

                    // update optional PID control here if needed (we rely on Motor_spin.setSpeed)

                    if (state_tick >= STIR_TICKS)
                    {
                        // finished stirring -> go to measurement
                        currentstate = VS_MEASURE;
                    }
                    else
                    {
                        // show remaining time every 1s
                        if ((state_tick % MS_TO_TICKS(1000)) == 0)
                        {
                            uint32_t remaining_s = (STIR_TICKS - state_tick) * (TICK_MS) / 1000;
                            char tbuf[20];
                            sprintf(tbuf, "%lus remaining", (unsigned long)remaining_s);
                            lcd_put_cursor(1,0);
                            lcd_send_string(tbuf);
                        }
                    }
                    break;
                }

                case VS_MEASURE:
                {
                    // measure for MEASURE_TICKS at 20-30 RPM, collect several samples
                    if (laststate != currentstate)
                    {
                        state_tick = 0;
                        motor_target_rpm = (MIN_MEAS_RPM + MAX_MEAS_RPM) / 2.0f; // 25 rpm target
                        applyMotorSetpoint(motor_target_rpm);

                        visc_count = 0;
                        vis_acc = 0.0f;
                        viscosity_ready = false;

                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Measuring...");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Collecting data");
                    }

                    // sample at a slower rate than tick to avoid too many samples: sample every 200 ms
                    if ((state_tick % MS_TO_TICKS(200)) == 0)
                    {
                        float SB1 = (float)IV.read(ADC_READ_MV);
                        float mu = 6.25f * SB1 - 17.5f;
                        last_mu_sample = mu;
                        vis_acc += mu;
                        visc_count++;

                        // update progress on LCD
                        char pbuf[24];
                        sprintf(pbuf, "Samples: %d", visc_count);
                        lcd_put_cursor(1,0);
                        lcd_send_string(pbuf);
                    }

                    if (state_tick >= MEASURE_TICKS)
                    {
                        // stop motor and compute average
                        applyMotorSetpoint(0.0f);

                        if (visc_count > 0)
                            viscosity_avg = vis_acc / (float)visc_count;
                        else
                            viscosity_avg = 0.0f;

                        viscosity_ready = true;

                        currentstate = VS_REPORT;
                    }
                    break;
                }

                case VS_REPORT:
                {
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Mean viscosity:");
                        lcd_put_cursor(1,0);
                        char mbuf[32];
                        sprintf(mbuf, "%.2f", viscosity_avg);
                        lcd_send_string(mbuf);

                        // indicate measurement complete in whatever way (RGB ignored)
                    }
                    // wait for user to press button to remove sample
                    if (btn)
                    {
                        currentstate = VS_REMOVE;
                    }
                    break;
                }

                case VS_REMOVE:
                {
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Remove sample");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Raise cylinder");
                    }

                    // allow user to raise cylinder using Y axis
                    if (JOY.zero())
                    {
                        PWM_Stepper_UP.setDuty(0.0f);
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

                    if (btn)
                    {
                        // user confirms removed -> rotate to cleaning station
                        currentstate = VS_CLEAN_ROTATE;
                    }
                    break;
                }

                case VS_CLEAN_ROTATE:
                {
                    if (laststate != currentstate)
                    {
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Rotate to clean");
                        lcd_put_cursor(1,0);
                        lcd_send_string("Use X axis; Btn=Ready");
                    }

                    // use X axis to rotate carousel
                    if (JOY.Left())
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
                    else
                    {
                        PWM_Stepper_ROT.setDuty(0.0f);
                    }

                    // lowering to cleaning station via Y axis
                    if (JOY.Up())
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

                    // press button when at cleaning station and lowered
                    if (btn)
                    {
                        currentstate = VS_CLEAN_TIMER;
                    }
                    break;
                }

                case VS_CLEAN_TIMER:
                {
                    if (laststate != currentstate)
                    {
                        state_tick = 0;
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Cleaning 60s");
                    }

                    if (state_tick < CLEAN_TICKS)
                    {
                        uint32_t remaining_s = (CLEAN_TICKS - state_tick) * (TICK_MS) / 1000;
                        char tbuf[20];
                        sprintf(tbuf, "%lus left", (unsigned long)remaining_s);
                        lcd_put_cursor(1,0);
                        lcd_send_string(tbuf);
                    }
                    else
                    {
                        currentstate = VS_DRY_TIMER;
                    }
                    break;
                }

                case VS_DRY_TIMER:
                {
                    if (laststate != currentstate)
                    {
                        state_tick = 0;
                        lcd_clear();
                        lcd_put_cursor(0,0);
                        lcd_send_string("Drying 120s");
                    }

                    if (state_tick < DRY_TICKS)
                    {
                        uint32_t remaining_s = (DRY_TICKS - state_tick) * (TICK_MS) / 1000;
                        char tbuf[20];
                        sprintf(tbuf, "%lus left", (unsigned long)remaining_s);
                        lcd_put_cursor(1,0);
                        lcd_send_string(tbuf);
                    }
                    else
                    {
                        // after drying, return to initial welcome state
                        currentstate = VS_WELCOME;
                    }
                    break;
                }

                default:
                    break;
            } // end switch

            // detect state entry and reset state_tick on change
            if (laststate != currentstate)
            {
                state_tick = 0;
                laststate = currentstate;
            }

            // serial debug print
            printf("state=%d,rpm=%.2f,visc=%.2f\n", currentstate, currentRPM, viscosity_avg);
        } // timer interrupt available
    } // while
}
