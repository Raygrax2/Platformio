#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{

    esp_task_wdt_deinit();

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(dt);  
    uint8_t Channels = 0;
    
    SimpleGPIO Dir;
    Dir.setup(4, GPIO_MODE_OUTPUT);
    SimplePWM Step;
    Step.setup(21, Channels, &PWM_TimerA);
    int dir = 1;
    Dir.set(dir);
    Step.setDuty(75.0f);
    SimpleUART UART_1(115200);
    float frecuency = 650.0f;
    
    while (1)
    {

        if (timer.interruptAvailable())
        {

            if (UART_1.available()){
                UART_1.read(Buffer, UART_1.available());
                sscanf(Buffer, "%f,%d\n",&frecuency,&dir);
            }
            
            Step.setFrequency(frecuency);
            Dir.set(dir);
            printf("Frequency set to: %.2f %d Hz\n", frecuency, dir);
        }
    }
}
