#include "definitons.h"
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{

    esp_task_wdt_deinit();
    Encoder.setup(EncoderPIN, 0.36445);
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(dt);
    motorA.setup(AIN, CHA, PWM_TimerA);
    PID_2.setup(PID_GAINS, dt); // PID will convert dt to seconds internally

    while (1)
    {
        if (timer.interruptAvailable())
        {
            speed_raw = Encoder.getSpeed();
            current_position = Encoder.getAngle(); // deg

            switch (current_mode)
            {
            case MODE_MANUAL_PWM:
                u = ref;
                break;
            case MODE_SPEED:
                error = ref - speed_raw;
                u = PID_2.computedU(error);
                break;

            case MODE_POSITION:
                error = ref - current_position;
                u = PID_2.computedU(error);
                break;
            default:
                break;
            }
            motorA.setSpeed(u);
            printf("%.2f,%.2f,%.2f\n", error, current_position, speed_raw);
            uint8_t available = UART.available();
            if (available)
            {
                UART.read(rxbuf, available);
                sscanf(rxbuf, "%d,%f,%f,%f,%f\n", &current_mode, &ref, &Kp, &Ki, &Kd);
                PID_GAINS[0] = Kp;
                PID_GAINS[1] = Ki;
                PID_GAINS[2] = Kd;
                PID_2.setup(PID_GAINS, dt);
            }
        }
    }
}