#include "definitons.h"
#include "esp_timer.h"
#include <string.h>
#include <stdlib.h>

// Timer ISR wrapper used by SimpleTimer
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    // disable watchdog for long operations during dev
    esp_task_wdt_deinit();

    // ---------- HARDWARE SETUP ----------
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(dt); // dt in microseconds from definitions.h

    // Stepper setup: (dir_pin, pwm_pin, pwm_channel, timerRef, step_deg, dt)
    // Use your Stepper::setup signature; here I mirror the call you used before
    Stepper_Up.setup(UP_PIN_DIR, PWM_UP_PIN, 4, &PWM_UP_STEP, STEPPER_MICROSTEP_DEG, dt);
    Stepper_Rot.setup(ROT_PIN_DIR, PWM_ROT_PIN, 5, &PWM_TimerA, STEPPER_MICROSTEP_DEG, dt);

    // Spin motor
    Motor_spin.setup(AIN, BDC_Motor_CH, PWM_Spin);
    // Pump GPIO
    Pump.setup(Pump_PIN, Pump_CH, PUMP_PWM_TIMER);
    Color_sensor.begin(I2C_NUM_0, 0x29);
    // Encoder
    enco.setup(EncoderPins, ENCODER_DEGREES_PER_EDGE);

    // PID
    PID.setup(PID_GAINS, dt / 1000000.0f);

    // UART already constructed in definitions.h as UART_MESSAGE
    // Clear rx buffer
    rxbuf[0] = '\0';

    // Emg stop
    emg_relay.setup(Emg_stop_PIn, GPO);
    // Timing helpers for pump polling (case 2)
    uint64_t pump_start_time = 0;
    const uint64_t pump_duration_us = 3000000; // 3 seconds

    // Example references (you can override via received UART)
    float UP_deg = 0.0f; // degrees to move when state asks
    float ROT_deg = 0.0f;
    float pos_rot = Stepper_Rot.getPosition();
    float pos_up = Stepper_Up.getPosition();

    // Print header so LabVIEW knows comms started (optional)
    printf("COMM: Ready\n");

    // -------------------- MAIN LOOP --------------------
    while (1)
    {
        // Wait for timer tick
        if (timer.interruptAvailable())
        {
            // ------------------ 1) Read encoder and update telemetry ------------------
            float measured_rpm = enco.getSpeed();
            send_speed = measured_rpm; // report actual rpm in send packet
            // You can also compute a pos from stepper or encoder; here we use encoder angle as posx/posy demo
            send_posx = enco.getAngle(); // posx = encoder angle
            send_posy = 0.0f;            // posy unused in this minimal example
            Color_sensor.readRaw(c, r, g, b);

            // ------------------ 2) UART receive (non-blocking, safe) ------------------
            int available = UART_MESSAGE.available();
            if (available)
            {
                UART_MESSAGE.read(rxbuf, available);
                int parsed = sscanf(rxbuf, "%f,%d,%d,%f,%f",
                                    &recv_ref_rpms,
                                    &recv_emergency_stop,
                                    &recv_send_water,
                                    &recv_ref_posx,
                                    &recv_ref_posy);

                if (parsed >= 1)
                {
                    // apply received values as needed
                    // If emergency stop is set, override states immediately
                    if (recv_emergency_stop != 0)
                    {
                        emg_relay.set(1);
                        currentstate = 4;
                    }
                }
                // clear rxbuf for next read
                rxbuf[0] = '\0';
            }

            // ------------------ 3) State Machine ------------------
            switch (currentstate)
            {
            case 0: // Elevation: move up by UP_deg degrees (blocking inside stepper)
                // Use Stepper_Up's moveDegrees method (blocking as you implemented previously)
                Stepper_Up.moveDegrees(UP_deg);
                break;

            case 1: // Rotation: rotate by ROT_deg degrees
                Stepper_Rot.moveDegrees(ROT_deg);
                break;

            case 2: // Pump: turn pump on for 3 seconds (polling)
                if (recv_send_water != 0)
                {
                    // start pump if not already
                    if (pump_start_time == 0)
                    {
                        Pump.setSpeed(100.0f);
                        pump_start_time = esp_timer_get_time();
                    }
                    else
                    {
                        uint64_t now = esp_timer_get_time();
                        if (now - pump_start_time >= pump_duration_us)
                        {
                            // done
                            Pump.setSpeed(0.0f);
                            pump_start_time = 0;
                            // optionally reset recv_send_water to 0 locally
                            recv_send_water = 0;
                        }
                    }
                }
                else
                {
                    // ensure pump off
                    Pump.setSpeed(0.0f);
                    pump_start_time = 0;
                }
                break;

            case 3: // Velocity control example: use PID to track recv_ref_rpms
                // Use encoder speed as feedback; compute u and apply to spin motor
                {
                    float error = recv_ref_rpms - measured_rpm;
                    float out = PID.computedU(error);
                    // apply out to Motor_spin: map as appropriate (here minimal)
                    // This line assumes Motor_spin.setSpeed exists; if not, replace accordingly
                    Motor_spin.setSpeed(out);
                }
                break;

            case 4:
            default:
                // idle / no action
                break;
            } // switch

            // ------------------ 4) Telemetry send (printf) ------------------
            // Send buffer: R,G,B,PosX,PosY,Speed\n
            // Use printf (LabVIEW reads from serial)
            printf("%d,%d,%d,%.2f,%.2f,%.2f\n",
                   send_R, send_G, send_B,
                   send_posx, send_posy, send_speed);

        } // timer tick
        // yield to other tasks
    } // while
}
