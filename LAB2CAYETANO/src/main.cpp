#include "definitons.h"

/*static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer2.setInterrupt();
}
    */
//añadir interrups
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    ADC.setup(ADC_pin);
    Square.setup(pinSquare, channelSquare, timer);
    //timer2.setup(timerinterrupt, "Timer2"); //Los comentarios se quitan para poner interrupciones
    //timer2.startPeriodic(200000);
    while(1)
    {
        //if (timer2.interruptAvailable())
    }
}
