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
    Encoder.setup(EncoderPIN, Deg_per_edge);
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(20);  // 20 ms -> 50 Hz

    // --- Configurar PWM motor A y dummy PWM B ---
    pwmmotorA.setup(motorPinA, motorChannelA, &PWM_TimerA);
    pwmmotorB.setup(motorPinB, motorChannelB, &PWM_TimerB);  // Dummy, sin efecto real

    // --- Configurar puente H ---
    motorDriver.setup(Ain1Pin, Ain2Pin, Bin1Pin, Bin2Pin, pwmmotorA, pwmmotorB, 1.0f);

    // --- Bucle principal ---
    while (1)
    {
        // Girar en un sentido
        motorDriver.setSpeed(75.0f);

        

       
        
    }
}