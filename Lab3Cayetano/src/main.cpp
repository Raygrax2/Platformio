#include "definitons.h"
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{
    // Deinit watchdog (for testing)
    esp_task_wdt_deinit();

    timer.setInterrupt();               // --- Setup hardware / modules ---
    Encoder.setup(EncoderPIN, 0.36445); // encoder scale: degrees per edge
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(20);                     // 20 ms loop -> 50 Hz main loop (adjust as needed)
    LPF.setup(LPF_coeffs_b, LPF_coeffs_a, 2, 2); // LPF to smooth speed
    motorA.setup(AIN, CHA, PWM_TimerA);          // motor A setup

    // PID initial configure (dt in seconds).
    float dt = 0.02f;  // 20 ms (consistent with timer period)
    float Kp = 0.1f;   // initial proportional gain
    float Ki = 0.01f;  // initial integral gain
    float Kd = 0.005f; // initial derivative gain

    PID.setup(Kp, Ki, Kd, dt); // initial configuration

    // Local working vars
    float duty_percent = 0.0f; // duty to send to motor in percent [-100..100]
    float duty_norm = 0.0f;    // normalized duty for motorA.setSpeed() [-1..1]

    float out_PID = 0.0f;
    float error = 0.0f;
    float setpoint = 0.0f; // desired value, interpreted according to mode
    float desired = 0.0f;  // from UART, interpreted according to mode
    float speed_raw = 0.0f;
    float speed_filtered = 0.0f;
    float current_position = 0.0f;      // in degrees
    int current_mode = MODE_MANUAL_PWM; // default mode

    // Local receive buffer (do not shadow global Buffer in definitions.h)
    char rxbuf[64];

    while (1)
    {
        if (timer.interruptAvailable())
        {
            // --- 1) Read sensors ---
            speed_raw = Encoder.getSpeed();        // deg/s
            current_position = Encoder.getAngle(); // deg

            // --- 2) Apply filter to raw speed (sample-by-sample) ---
            speed_filtered = LPF.apply(speed_raw);

            // --- 3) Read UART / LabVIEW buffer (non-blocking) ---
            uint8_t available = UART.available();
            if (available)
            {

                UART.read(rxbuf, available);
                rxbuf[available] = '\0';

                // Expecting CSV: Mode,Desired,Kp,Ki,Kd
                // Mode: integer (0=PWM,1=Speed,2=Position)
                // Desired: interpreted according to Mode
                int parsedMode = -1;
                float desired = 0.0f;

                sscanf(rxbuf, "%d,%f,%f,%f,%f", &parsedMode, &desired, &Kp, &Ki, &Kd);

                PID.setup(Kp, Ki, Kd, dt);
            }
            else
            {
                // Even if no UART input, reconfigure PID as requested (inefficient but required)
                PID.setup(Kp, Ki, Kd, dt);
            }

            // --- 4) Mode-dependent control logic ---
            switch (current_mode)
            {
            case MODE_MANUAL_PWM:
            {
                // setpoint interpreted as desired duty percentage [0..100] or [-100..100]
                // keep value in range -100..100
                float setpoint = desired;
                duty_percent = setpoint;

                // normalize for motor API (-1..1)
                motorA.setSpeed(duty_percent);
                error = 0.0f; // no error concept in manual PWM
                break;
            }

            case MODE_SPEED:
            {
                // setpoint is desired speed in deg/s
                // PID input: setpoint (deg/s) and measured value speed_filtered (deg/s)
                error = setpoint - speed_filtered;
                out_PID = PID.apply(setpoint, speed_filtered);

                // Convert to percentage for logging
                duty_norm = out_PID;
                duty_percent = duty_norm;

                // Send to motor
                motorA.setSpeed(duty_percent);
                break;
            }

            case MODE_POSITION:
            {
                // setpoint is desired position in degrees
                // PID input: setpoint (deg) and measured value current_position (deg)
                error = setpoint - current_position;
                out_PID = PID.apply(setpoint, current_position);

            

                duty_norm = out_PID;
                duty_percent = duty_norm * 100.0f;

                // Actuate motor
                motorA.setSpeed(duty_percent);
                break;
            }

            default:
            {
                // Unknown mode: stop motor for safety
                duty_norm = 0.0f;
                duty_percent = 0.0f;
                motorA.setSpeed(0.0f);
                error = 0.0f;
                break;
            }
            } // end switch(current_mode)

            // --- 5) Prepare telemetry for LabVIEW: error, position, speed (deg/s) ---
            // Send CSV: error,position,speed,duty_percent,mode
            // Example line: "1.23,45.67,100.32,12.34,1\n"
            printf("%.3f,%.3f,%.3f,\n",
                   error,
                   current_position,
                   speed_filtered);

        } // end if(timer.interruptAvailable())

    } // end while(1)
} // end app_main
