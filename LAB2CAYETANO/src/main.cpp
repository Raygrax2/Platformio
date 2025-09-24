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
    float filteredValue = 0.0;
    ADC.setup(ADC_pin);
    // Square.setup(pinSquare, channelSquare, timer);
    LPF.setup(b_LPF, a_LPF);
    
    //HPF.setup(b_HPF, a_HPF);
    //MA.setup(b_MA, a_MA);
    //timer2.setup(timerinterrupt, "Timer2"); //Los comentarios se quitan para poner interrupciones
    //timer2.startPeriodic(200000);
    while(1)
    {
        //if (timer2.interruptAvailable())
        filteredValue = LPF.apply(ADC.read(ADC_READ_RAW), filteredValue);
        //It can be done the same for HPF and MA
        
        if (UART_RAW.available()){
            
        }
        
    }
}
