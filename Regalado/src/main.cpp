#include "definitons.h"
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }
extern "C" void app_main()
{
    // Deinit watchdog for testing (as in your snippet)
    esp_task_wdt_deinit();
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(1000); // keep your period value (user used 1000)
adc.setup(adc_pin, ADC_BITWIDTH_12); // Initialize ADC with pin and bitwidth
    // Main loop: respond to periodic timer
    while (1)
    {
        if (timer.interruptAvailable())
        {
            // Your periodic code here
            int adc_value = adc.read(ADC_READ_RAW);
            printf("ADC Value: %d\n", adc_value);
            
        } // end while(1)
    }
