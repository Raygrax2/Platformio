#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer2.setInterrupt();
}

extern "C" void app_main()
{
    esp_task_wdt_deinit();
    timer2.setup(timerinterrupt, "Timer2");
    timer2.startPeriodic(200000);
    ADC.setup(ADC_pin);
    Square.setup(pinSquare, channelSquare, &PWM_Time, false);
    Square.setFrequency(4); // 4 Hz
    Square.setDuty(50.0f);  // duty 50% (valor entre 0.0 y 1.0)

    LPF.setup(b_LPF, a_LPF, 3, 3);
    HPF.setup(b_HPF, a_HPF, 3, 3);
    MA.setup(b_MA, a_MA, 4, 1);

    while (1)
    {
        if (timer2.interruptAvailable())
        {
            // Leer ADC
            float rawVal;
            rawVal = (float)ADC.read(ADC_READ_RAW);

            float filteredVal_LPF = LPF.apply(rawVal);
            float filteredVal_HPF = HPF.apply(rawVal);
            float filteredVal_MA = MA.apply(rawVal);
            printf("%.3f,%.3f,%.3f,%.3f\n", rawVal, filteredVal_LPF, filteredVal_HPF, filteredVal_MA);
        }
    }
}
