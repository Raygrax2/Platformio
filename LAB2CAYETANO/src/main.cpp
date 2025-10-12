#include "definitons.h"

static void IRAM_ATTR timerinterrupt2(void *arg)
{
    timer2.setInterrupt();
}
static void IRAM_ATTR timerinterrupt(void *arg)
{
    timer.setInterrupt();
}
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    float filteredVal_LPF= 0;
    float filteredVal_HPF = 0;
    float filteredVal_MA = 0;
    timer2.setup(timerinterrupt2, "Timer");
    timer2.startPeriodic(20);
    timer.setup(timerinterrupt, "Timer1");
    timer.startPeriodic(10000);
    Square.setup(pinSquare, channelSquare, &PWM_Time, false);
    Square.setFrequency(4); // 4 Hz
    Square.setDuty(50.0f);  // duty 50% 
    Recon.setup(ReconPIN, channelRecon, &PWM_Recon_Time, false);
    Recon.setDuty(0); // duty 0% (valor entre 0.0 y 1.0)
    
    LPF.setup(b_LPF, a_LPF, 3, 3);
    HPF.setup(b_HPF, a_HPF, 3, 3);
    MA.setup(b_MA, a_MA, 4, 1);

    while (1)
    {
        if (timer.interruptAvailable())
        {
            // Leer ADC
            float rawVal;
            rawVal = (float)ADC.read(ADC_READ_RAW);

            filteredVal_LPF = LPF.apply(rawVal);
            filteredVal_HPF = HPF.apply(rawVal);
            filteredVal_MA = MA.apply(rawVal);
            printf("%.3f,%.3f,%.3f,%.3f\n", rawVal, filteredVal_LPF, filteredVal_HPF, filteredVal_MA);
        }
        if (timer2.interruptAvailable())
        {
            float duty = filteredVal_HPF / 4;
            Recon.setDigitalLevel(duty+512);
        
        }
    }
}
