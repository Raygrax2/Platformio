// ============================================================================
// main.cpp
// ============================================================================
#include "definitons.h"
#include "esp_timer.h"
#include <string.h>

// Timer ISR
static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer.setInterrupt();
}

extern "C" void app_main()
{
    // Disable watchdog
    esp_task_wdt_deinit();

    // ========================================================================
    // HARDWARE INITIALIZATION
    // ========================================================================

    // Timer setup
    timer.setup(timerinterrupt, "MainTimer");
    timer.startPeriodic(dt);

    // Stepper motors
    Stepper_Up.setup(STEPPER_UP_DIR_PIN, STEPPER_UP_PWM_PIN,
                     0, &PWM_STEPPER_UP_TIMER,
                     STEPPER_DEGREES_PER_STEP, dt);

    Stepper_Rot.setup(STEPPER_ROT_DIR_PIN, STEPPER_ROT_PWM_PIN,
                      1, &PWM_STEPPER_ROT_TIMER,
                      STEPPER_DEGREES_PER_STEP, dt);

    // BDC Motors
    uint8_t spin_pins[2] = {SPIN_MOTOR_PIN_A, SPIN_MOTOR_PIN_B};
    uint8_t spin_channels[2] = {2, 3};
    MotorS.setup(spin_pins, spin_channels);

    uint8_t pump_pins[2] = {PUMP_PIN_A, PUMP_PIN_B};
    uint8_t pump_channels[2] = {4, 5};
    Pump.setup(pump_pins, pump_channels);

    // Encoder2
    uint8_t encoder_pins[2] = {ENCODER_PIN_A, ENCODER_PIN_B};
    enco.setup(encoder_pins, ENCODER_DEGREES_PER_EDGE);

    // PID controller
    PID.setup(PID_GAINS, dt / 1000000.0f);

    // Color sensor
    Color_sensor.begin(I2C_NUM_0, 0x29);

    // Emergency relay
    emg_relay.setup(EMERGENCY_STOP_PIN, GPO);
    emg_relay.set(0); // Initially not stopped

    // Clear buffers

    // ========================================================================
    // PUMP TIMING VARIABLES
    // ========================================================================
    uint64_t pump_start_time = 0;
    const uint64_t PUMP_DURATION_US = 3000000; // 3 seconds
    bool pump_active = false;

    // ========================================================================
    // MOVEMENT REFERENCES
    // ========================================================================
    float target_up_degrees = 0.0f;
    float target_rot_degrees = 0.0f;

    printf("System initialized. Ready for commands.\n");

    // ========================================================================
    // MAIN LOOP
    // ========================================================================
    while (1)
    {
        if (timer.interruptAvailable())
        {
            // ================================================================
            // 1. READ SENSORS
            // ================================================================
            telemetry.speed = enco.getSpeed();
            telemetry.pos_x = enco.getAngle();
            telemetry.pos_y = Stepper_Up.getPosition();

            Color_sensor.readRaw(sensor_c, sensor_r, sensor_g, sensor_b);
            telemetry.R = sensor_r;
            telemetry.G = sensor_g;
            telemetry.B = sensor_b;

            // ================================================================
            // 2. RECEIVE UART COMMANDS
            // ================================================================
            int available = UART_MESSAGE.available();
            if (available > 0)
            {
                UART_MESSAGE.read(rxbuf, available);

                int parsed = sscanf(rxbuf, "%d,%f,%d,%d,%f,%f\n",
                                    &current_state,
                                    &control.ref_rpms,
                                    &control.emergency_stop,
                                    &control.send_water,
                                    &control.ref_pos_x,
                                    &control.ref_pos_y);
                target_up_degrees = control.ref_pos_y;
                target_rot_degrees = control.ref_pos_x;
                printf("%d,%.2f,%d,%d,%.2f,%.2f\n",
                       current_state,
                       (double)control.ref_rpms,
                       control.emergency_stop,
                       control.send_water,
                       (double)control.ref_pos_x,
                       (double)control.ref_pos_y);
                if (parsed >= 1)
                {
                    // Handle emergency stop immediately
                    if (control.emergency_stop != 0)
                    {
                        emg_relay.set(1);
                        current_state = STATE_IDLE;
                        MotorS.setSpeed(0.0f);
                        Pump.setSpeed(0.0f);
                        printf("EMERGENCY STOP ACTIVATED\n");
                    }
                    else
                    {
                        emg_relay.set(0);
                    }
                }

                memset(rxbuf, 0, sizeof(rxbuf));
            }
            Stepper_Up.update();
            Stepper_Rot.update();
            // ================================================================
            // 3. STATE MACHINE

            // ================================================================
            switch (current_state)
            {
            case STATE_ELEVATION:
                // Move stepper up
                if (target_up_degrees != 0.0f)
                {
                    Stepper_Up.moveDegrees(target_up_degrees);
                    printf("%f", Stepper_Up.getPosition());
                }
                break;

            case STATE_ROTATION:
                // Rotate stepper
                if (target_rot_degrees != 0.0f)
                {
                    Stepper_Rot.moveDegrees(target_rot_degrees);
                }
                break;

            case STATE_PUMP:
                // Pump control with timing
                if (control.send_water != 0)
                {
                    if (!pump_active)
                    {
                        Pump.setSpeed(100.0f);
                        pump_start_time = esp_timer_get_time();
                        pump_active = true;
                    }
                    else
                    {
                        uint64_t elapsed = esp_timer_get_time() - pump_start_time;
                        if (elapsed >= PUMP_DURATION_US)
                        {
                            Pump.setSpeed(0.0f);
                            pump_active = false;
                            control.send_water = 0;
                        }
                    }
                }
                else
                {
                    Pump.setSpeed(0.0f);
                    pump_active = false;
                }
                break;

            case STATE_VELOCITY_CONTROL:
                // PID velocity control
                {
                    float error = control.ref_rpms - telemetry.speed;
                    float output = PID.computedU(error);
                    printf("Coumputo el u %f, enco =  %f\n", output, enco.getSpeed());

                    MotorS.setSpeed(output);
                }
                break;

            case STATE_IDLE:
            default:
                // Do nothing
                break;
            }

            // ================================================================
            // 4. SEND TELEMETRY
            // ================================================================
            // printf("%d,%d,%d,%.2f,%.2f,%.2f\n",
            //   telemetry.R, telemetry.G, telemetry.B,
            //   telemetry.pos_x, telemetry.pos_y, telemetry.speed);

        } // timer tick
    } // while(1)
}