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

        // ------------- Parámetros de muestreo / UART -------------
        const uint32_t SAMPLE_PERIOD_MS = 10; // 100 Hz de muestreo
        uint32_t sampleIndex = 0;
        uint8_t len = 0;

        // Nota: si quieres cambiar el filtro en tiempo de ejecución, puedes
        // editar aquí la variable para usar HPF o MA en lugar de LPF.

        while (1)
        {
            // Leer ADC
            float rawVal;
            rawVal = (float)ADC.read(ADC_READ_RAW);

            float filteredVal = LPF.apply(rawVal);
            // float filteredVal = HPF.apply(rawVal);
            // float filteredVal = MA.apply(rawVal);

            if (uart.available())
            {
                uart.read(buffer, 16);
                int time_in;
                sscanf(buffer, "%d, %d, %d, %d \n", &time_in, &real_color[0], &real_color[1], &real_color[2]);
                // printf("Time: %d, R: %d, G: %d, B: %d\n", time_in, real_color[0] , real_color[1], real_color[2]);
                dt_us = time_in * 1000;
                rgb[1].setColor(real_color[0], real_color[1], real_color[2]);
            }
            if (UART_RAW.available())
            {
                // Enviar por UART (ASCII). Si tu SimpleUART tiene otra firma, reemplázala:
                UART_RAW.write(outBuf, (size_t)len);
                // Opciones alternativas comentadas:
                // UART_RAW.writeBytes((const uint8_t*)outBuf, (size_t)len);
                // UART_RAW.write((char*)outBuf, (size_t)len);
            }
        }
    }
}
