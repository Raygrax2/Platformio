#include "definitons.h"
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    
    esp_task_wdt_deinit();

    Encoder.setup(EncoderPIN, 0.36445);

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(2000);                   // 20 ms loop -> 50 Hz main loop (adjust as needed)
    LPF.setup(LPF_coeffs_b, LPF_coeffs_a, 2, 2); // LPF to smooth speed
    motorA.setup(AIN, CHA, PWM_TimerA);          // motor A setup
    float dt = 0.02f; // 20 ms (consistent with timer period)
    float Kp = 1.0f;  // initial proportional gain
    float Ki = 0.0f;  // initial integral gain
    float Kd = 0.0f;  // initial derivative gain

    PID.setup(Kp, Ki, Kd, dt); // initial configuration
    float error = 0.0f;
    float speed_raw = 0.0f;
    uint8_t lwghtmessage = 0;
    float speed_filtered = 1.0f;
    float current_position = 0.0f;      // in degrees
    int current_mode = MODE_MANUAL_PWM; // default mode
    float ref = -100.0f;                // reference position (deg)
    // Local receive buffer (do not shadow global Buffer in definitions.h)
    char rxbuf[32];
    float u = 0.0f;

    while (1)
    {
        if (timer.interruptAvailable())
        {
            speed_raw = Encoder.getSpeed();        // deg/s
            current_position = Encoder.getAngle(); // deg
            speed_filtered = LPF.apply(speed_raw);

            // --- 3) Read UART / LabVIEW buffer (non-blocking) ---
            uint8_t available = UART.available();
            if (available)
            {

                UART.read(rxbuf, available);

                sscanf(rxbuf, "%d,%f,%f,%f,%f\n", &current_mode, &ref, &Kp, &Ki, &Kd);

                PID.setup(Kp, Ki, Kd, dt);
            }

            // --- 4) Mode-dependent control logic ---
            switch (current_mode)
            {
            case MODE_MANUAL_PWM:
            {
                // setpoint interpreted as desired duty percentage [0..100] or [-100..100]
                // keep value in range -100..100
                // normalize for motor API (-1..1)
                motorA.setSpeed(ref);
                error = 0.0f; // no error concept in manual PWM
                break;
            }

            case MODE_SPEED:
            {
                // setpoint is desired speed in deg/s
                // PID input: setpoint (deg/s) and measured value speed_filtered (deg/s)
                error = ref - speed_raw;
                u = PID.apply(ref, speed_filtered);
                // Actuate motor
                motorA.setSpeed(u);
                break;
            }

            case MODE_POSITION:
            {
                // setpoint is desired position in degrees
                // PID input: setpoint (deg) and measured value current_position (deg)
                error = ref - current_position;
                u = PID.apply(ref, current_position);
                // Actuate motor
                motorA.setSpeed(u);
                break;
            }

            default:
            {
                // Unknown mode: stop motor for safety
                
                motorA.setSpeed(0.0f);
                error = 0.0f;
                break;
            }
            } // end switch(current_mode)

            lwghtmessage = sprintf(rxbuf,"%.2f,%.2f,%.2f\n",error,current_position,speed_raw);
            UART.write(rxbuf, lwghtmessage);
        }
    }
}
