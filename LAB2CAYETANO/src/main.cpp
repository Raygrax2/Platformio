#include "definitons.h"


//añadir interrups
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    float filteredValue = 0.0;
    ADC.setup(ADC_pin);
    LPF.setup(b_LPF, a_LPF,3,3);
    ADC.setup(ADC_pin);
    Square.setup(pinSquare, channelSquare, &PWM_Time, false);
    //HPF.setup(b_HPF, a_HPF);
    //MA.setup(b_MA, a_MA);
    while(1)
    {
        filteredValue = LPF.apply(ADC.read(ADC_READ_RAW));
        //filteredValue = HPF.apply((float)ADC.read());
        //filteredValue = MA.apply((float)ADC.read());
        if(UART_RAW.available()){
            UART_RAW.write(ADC.read(ADC_READ_RAW), sizeof(ADC.read(ADC_READ_RAW));
        }
        if(UART_Filtered.available()){
            UART_Filtered.write((char*)&filteredValue, sizeof(filteredValue));
        }

    }
}
