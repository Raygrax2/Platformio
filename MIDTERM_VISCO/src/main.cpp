#include "definitons.h"
#include "Stepper.h"
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{

    esp_task_wdt_deinit();

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(dt);  
    Stepper Stepper_nema1;                 // 20 ms loop -> 50 Hz main loop (adjust as needed)
    uint8_t drive_pins_nema1[3] = {4, 26, 27}; // Example GPIO pins for NEMA 17
    uint8_t Channels = 0;
    
    SimpleGPIO Dir;
    Dir.setup(drive_pins_nema1[0], GPIO_MODE_OUTPUT);
    SimplePWM Step;
    Step.setup(26, Channels, &PWM_TimerA);
    Dir.set(1);
    Step.setDuty(50.0f);
    SimpleUART UART_1(115200);
    float frecuency = 1000.0f;
    
    while (1)
    {

        if (timer.interruptAvailable())
        {

            if (UART_1.available()){
                UART_1.read(Buffer, UART_1.available());
                sscanf(Buffer, "%f\n",&frecuency);
            }
            
            Step.setFrequency(frecuency);
        }
    }
}
