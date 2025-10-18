#include "definitons.h"

static void IRAM_ATTR timerInterrupt(){
    timer.setInterrupt();
}

//añadir interrups
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    float filteredValue = 0.0;
    timer.setup(timerInterrupt, "Timer1");
    timer.startPeriodic(1000);
    //Setups
    motor1.setup();
    while(1)
    {
        if (timer.interruptAvailable())
        {
           
        }
        
    }
}
