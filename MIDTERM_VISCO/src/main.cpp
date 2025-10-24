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
        // Clear first line and print the state message
        len = UART.available();
    
        if (len)
        {
            UART.read(Buff_UART, len);
            sscanf(Buff_UART, "%d,%f,%f", &current_state, &RPMS, &Viscocity);
        }
        lcd_put_cursor(0, 0);
        lcd_send_string("                "); // clear 16 chars
        lcd_put_cursor(0, 0);

        switch (current_state)
        {
        case STATE_POWER_ON:
            lcd_send_string("Welcome");
            break;

        case STATE_SAMPLE_PLACEMENT:
            lcd_send_string("Place a Sample");
            RGB.setColor(0, 255, 0);

            break;

        case STATE_SAMPLE_POSITIONED:
            lcd_send_string("Sample OK");
            break;

        case STATE_CYLINDER_LOWERING:
            lcd_send_string("Down cylinder");
            break;

        case STATE_STIR_LIQUID:
            lcd_send_string("Liquid Stirring");
            break;

        case STATE_MEASURE_VISCOSITY:
            lcd_send_string("Measuring...");
            break;

        case STATE_MEASUREMENT_DISPLAY:
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

        case STATE_SAMPLE_REMOVAL:
            lcd_send_string("Remove sample");
            break;

        case STATE_CYLINDER_CLEANING:
            lcd_send_string("Cleaning cylinder");
            
            break;

        case STATE_CYLINDER_DRYING:
            lcd_send_string("Drying cylinder");
            break;

        case STATE_MANUAL_MOVEMENT:
            lcd_send_string("Manual movement");
            break;

        case STATE_PROCESS_RESTART:
            lcd_send_string("Restarting...");
            break;

        default:
            lcd_send_string("Default");
            break;
        }

        
        // advance to next state (or change this to button-driven logic)
        
    }
}
