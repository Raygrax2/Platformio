#include "definitons.h"

static void IRAM_ATTR timerInterrupt(){
    timer.setInterrupt();
}

//añadir interrups
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    float filteredValue = 0.0;
    ADC.setup(ADC_pin);
    timer.setup(timerInterrupt, "Timer1");
    timer.startPeriodic(1000);
    // Square.setup(pinSquare, channelSquare, timer);
    
    //HPF.setup(b_HPF, a_HPF);
    //MA.setup(b_MA, a_MA);
    while(1)
    {
        
        if (timer.interruptAvailable())
        {
           
        }
        
    }
}
