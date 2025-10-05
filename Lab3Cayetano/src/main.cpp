#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer.setInterrupt();
}
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    Encoder.setup(EncoderPIN, Deg_per_edge);
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(20);
    

    while (1)
    {
        if (timer.interruptAvailable())
        {
            
        
        }
    }
}
