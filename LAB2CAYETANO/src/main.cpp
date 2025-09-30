#include "definitions.h"

static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer2.setInterrupt();
}

extern "C" void app_main()
{
    if (timer2.interruptAvailable())
    {
        esp_task_wdt_deinit();
        timer2.setup(timerinterrupt, "Timer2");
        timer2.startPeriodic(200000);
        ADC.setup(ADC_pin);
        Square.setup(pinSquare, channelSquare, &PWM_Time, false);
        Square.setFrequency(4); // 4 Hz
        Square.setDuty(0.5f);   // duty 50% (valor entre 0.0 y 1.0)

        LPF.setup(b_LPF, a_LPF, 3, 3);
        HPF.setup(b_HPF, a_HPF, 3, 3);
        MA.setup(b_MA, a_MA, 3, 3);
        while (1)
        {
            // Leer ADC
            float rawVal;
            rawVal = (float)ADC.read(ADC_READ_RAW);

            float filteredVal = LPF.apply(rawVal);
            
            if (UART_RAW.available())
            {
                
                UART_RAW.write(Buffer, len);
            
            }
        }
    }
}
