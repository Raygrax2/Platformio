#include "definitons.h"
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }
extern "C" void app_main()
{
    // Deinit watchdog for testing (as in your snippet)
    esp_task_wdt_deinit();
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(1000);           // keep your period value (user used 1000)
    adc.setup(adc_pin, ADC_BITWIDTH_12); // Initialize ADC with pin and bitwidth
    // Main loop: respond to periodic timer
    motor.setup(motor_pins, motor_ch, PWM_TimerA);
    while (1)
    {
        if (timer.interruptAvailable())
        {
            // Your periodic code here
            int adc_value = adc.read(ADC_READ_MV);
            printf("ADC Value: %d\n", adc_value);
            if (lower_treshold > adc_value)
            {
                motor.setSpeed(0.0f); // Stop motor
            }
            else if (lower_treshold <= adc_value && adc_value <= upper_treshold)
            {
                motor.setSpeed(50.0f); // Half speed
            }
            else if (upper_treshold < adc_value)
            {
                motor.setSpeed(100.0f); // Full speed
            }

        } // end while(1)
    }
