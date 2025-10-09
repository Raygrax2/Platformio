#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer.setInterrupt();
}

extern "C" void app_main()
{
    // Desactivar watchdog para pruebas
    esp_task_wdt_deinit();

    // --- Inicialización de periféricos ---
    Encoder.setup(EncoderPIN, 0.36445);
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(1000);                         // 20 ms -> 50 Hz
    PID.setup(0.8f, 0.3f, 0.0f, 0.02f, -1.0f, 1.0f); // Kp, Ki, Kd, dt (s), outMin, outMax

    // --- Configurar puente H ---
    motorA.setup(AIN, CHA, PWM_TimerA); // Usamos solo A-side
    // --- Bucle principal ---
    float speed = 0;
    float duty = 0;
    float desired = 0;
    while (1)
    {
        if (timer.interruptAvailable())
        {
            
            printf("%.2f, %.2f\n", speed,Encoder.getAngle());
            len = UART.available();
            speed = Encoder.getSpeed();
            if (len)
            {
                UART.read(Buffer, len);
                sscanf(Buffer, "%f\n", &duty);
                motorA.setSpeed(duty); // 50% velocidad adelante
            }
            // out_PID = PID.apply(desired,speed); // setpoint 100 deg/s
        }
    }
}