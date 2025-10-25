// main.cpp
#include "definitons.h" // make sure header name matches real file ("definitions.h")
extern "C"
{
#include "i2c_lcd.h"
}
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

// define the global state (move actual definition to one .cpp file only)

extern "C" void app_main()
{
    esp_task_wdt_deinit();

    lcd_init();  // Initialize the LCD
    lcd_clear(); // Clear the LCD screen

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000);

    char Buff_UART[32];
    float RPMS;
    float Viscocity;
    RGB.setup(RGB_Pins, RGB_CH, &PWM_Timer_RGB, 1);
    // Show header once (don't clear it immediately)
    lcd_put_cursor(0, 0);
    lcd_send_string("Viscometer");
    char BUff_MESSAGE_PROCESSED_1[32];
    char BUff_MESSAGE_PROCESSED_2[32];

    while (1)
    {
        if (timer.interruptAvailable())
        {
            // Clear first line and print the state message
            len = UART.available();

            if (len)
            {
                UART.read(Buff_UART, len);
                sscanf(Buff_UART, "%d,%f,%f", &current_state, &RPMS, &Viscocity);
            }
            lcd_put_cursor(0, 0);
            lcd_clear();
            lcd_put_cursor(0, 0);

            switch (current_state)
            {
            case 0:
                lcd_send_string("Welcome");
                break;

            case 1:
                lcd_send_string("Place a Sample");

                break;

            case 2:
                lcd_send_string("Sample OK");
                break;

            case 3:
                lcd_send_string("Down cylinder");
                break;

            case 4:
                lcd_send_string("Liquid Stirring");
                break;

            case 5:
                lcd_send_string("Measuring...");
                break;

            case 6:
                // Format floats with 1 decimal place
                sprintf(BUff_MESSAGE_PROCESSED_1, "RPMS: %.1f", RPMS);
                sprintf(BUff_MESSAGE_PROCESSED_2, "Visc: %.1f", Viscocity);

                // Write first line
                lcd_put_cursor(0, 0);
                lcd_send_string("                "); // clear line
                lcd_put_cursor(0, 0);
                lcd_send_string(BUff_MESSAGE_PROCESSED_1);

                // Write second line
                lcd_put_cursor(1, 0);
                lcd_send_string("                "); // clear line
                lcd_put_cursor(1, 0);
                lcd_send_string(BUff_MESSAGE_PROCESSED_2);
                break;

            case 7:
                lcd_send_string("Remove sample");
                break;

            case 8:
                lcd_send_string("Cleaning cylinder");

                break;

            case 9:
                lcd_send_string("Drying cylinder");
                break;

            case 10:
                lcd_send_string("Manual movement");
                break;

            case 11:
                lcd_send_string("Restarting...");
                break;

            case 12:
                lcd_send_string("Nigga");
                break;

            default:
                lcd_send_string("Default");
                break;
            }
            printf("%d\n", current_state);
            // advance to next state (or change this to button-driven logic)
        }
    }
}