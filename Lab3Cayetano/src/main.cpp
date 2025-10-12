#include "definitons.h"

// ---- Globals (accessible from parseCommand) ----
ControlMode current_mode = MODE_MANUAL_PWM;
float setpoint = 0.0f;         // Desired value (PWM duty, speed (deg/s), or position (deg))
float manual_duty = 0.0f;      // Manual PWM duty cycle (-1..1 assumed)
float speed_filtered = 0.0f;   // Filtered speed measurement
float current_position = 0.0f; // Current angular position (deg)
float speed_raw = 0.0f;

float Kp = 1.0f, Ki = 0.1f, Kd = 0.05f; // PID params (updated via UART/LabVIEW)
bool pid_needs_reconfigure = true;      // flag to reconfigure PID after coef change

// Hysteresis thresholds (if needed)
float speed_threshold_high = 1000.0f;
float speed_threshold_low  = -1000.0f;

// direct/manual duty
float manual_duty_limit = 1.0f;

// Temporary buffer for parsing
char Buffer[64];
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }
// Forward declaration
void parseCommand(char* buffer);

// Parse simple CSV-style commands from LabVIEW/host.
// Format examples:
//  M,0        -> mode 0 (manual PWM), 1 -> speed, 2 -> position
//  P,1.2,0.05,0.01 -> set PID Kp,Ki,Kd (affects current mode's PID next reconfigure)
//  S,123.4    -> setpoint (interpreted according to current mode)
//  H,high,low -> hysteresis thresholds
//  D,0.5      -> direct duty (manual mode)
void parseCommand(char* buffer)
{
    char cmd = buffer[0];

    switch(cmd) {
        case 'M': case 'm': {
            int mode;
            if (sscanf(buffer, "%*c,%d", &mode) == 1) {
                if (mode >= 0 && mode <= 2) {
                    current_mode = (ControlMode)mode;
                    setpoint = 0.0f;             // reset setpoint on mode change
                    pid_needs_reconfigure = true;
                    //PID.reset();
                }
            }
            break;
        }
        case 'P': case 'p': {
            float kp, ki, kd;
            if (sscanf(buffer, "%*c,%f,%f,%f", &kp, &ki, &kd) == 3) {
                Kp = kp; Ki = ki; Kd = kd;
                pid_needs_reconfigure = true;
            }
            break;
        }
        case 'S': case 's': {
            float sp;
            if (sscanf(buffer, "%*c,%f", &sp) == 1) {
                setpoint = sp;
            }
            break;
        }
        case 'H': case 'h': {
            float high, low;
            if (sscanf(buffer, "%*c,%f,%f", &high, &low) == 2) {
                speed_threshold_high = high;
                speed_threshold_low  = low;
            }
            break;
        }
        case 'D': case 'd': {
            float duty;
            if (sscanf(buffer, "%*c,%f", &duty) == 1) {
                manual_duty = duty;
            }
            break;
        }
        default:
            // ignore
            break;
    }
}

extern "C" void app_main()
{
    // Deinit watchdog for testing (as in your snippet)
    esp_task_wdt_deinit();

    // --- Setup hardware / modules ---
    Encoder.setup(EncoderPIN, Deg_per_edge);         // encoder scale: degrees per edge
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(1000); // keep your period value (user used 1000)
    LPF.setup(LPF_coeffs_b, LPF_coeffs_a, 2, 2);    // keep your existing LPF setup call
    motorA.setup(AIN, CHA, PWM_TimerA);             // motor A setup


    // PID initial configure (dt in seconds). We'll reconfigure when coefficients change.
    const float dt = 0.02f; // your previous comment used 20 ms
    PID.setup(Kp, Ki, Kd, dt);

    // Local working vars
    float duty = 0.0f;
    float control_output = 0.0f;
    float out_PID = 0.0f;

    // Main loop: respond to periodic timer
    while (1)
    {
        if (timer.interruptAvailable())
        {
            
            
            // 1) Read sensors
            speed_raw = Encoder.getSpeed();        // e.g. deg/s
            current_position = Encoder.getAngle(); // degrees

            // 2) Apply filter to raw speed (sample-by-sample)
            speed_filtered = LPF.apply(speed_raw);

            // 3) Handle UART / LabVIEW commands (non-blocking)
            uint8_t available = UART.available();
            if (available > 0) {
                if (available >= sizeof(Buffer)) available = sizeof(Buffer)-1;
                UART.read(Buffer, available);
                Buffer[available] = '\0'; // null-term
                parseCommand(Buffer);
            }

            // 4) If PID coefficients changed, reconfigure PID
            if (pid_needs_reconfigure) {
                PID.setup(Kp, Ki, Kd, dt);
                //PID.reset();
                pid_needs_reconfigure = false;
            }

            // 5) Mode-dependent control
            switch (current_mode) {
                case MODE_MANUAL_PWM:
                    // direct duty command from LabVIEW
                    control_output = manual_duty;
                    break;

                case MODE_SPEED:
                    // PID controller: setpoint is in same units as getSpeed() (deg/s)
                    out_PID = PID.apply(setpoint, speed_filtered);
                    control_output = out_PID;
                    break;

                case MODE_POSITION:
                    // Position PID: setpoint is angle (deg); controller output is velocity/speed command
                    out_PID = PID.apply(setpoint, current_position);
                    // Option 1: treat out_PID as direct PWM duty
                    // Option 2: saturate and/or cascade to a speed loop. We'll simply map to PWM duty for now.
                    control_output = out_PID;
                    break;
            }

            // 6) Clamp control_output (assume duty in [-1, 1])
            if (control_output > 1.0f) control_output = 1.0f;
            if (control_output < -1.0f) control_output = -1.0f;

            // 7) Send to motor (assuming setSpeed expects duty in -1..1)
            motorA.setSpeed(control_output);

            // 8) Optional: Debug print (prints angle and raw speed as you had)
            printf("mode:%d set:%.2f pos:%.2f sp_raw:%.2f sp_filt:%.2f duty:%.3f\n",
                   (int)current_mode, setpoint, current_position, speed_raw, speed_filtered, control_output);
        } // end timer interrupt handler
    } // end while(1)
}
