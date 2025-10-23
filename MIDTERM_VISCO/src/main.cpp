#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{

    esp_task_wdt_deinit();
    lcd_init();  // Initialize the LCD
    lcd_clear(); // Clear the LCD screen

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000);
    RGB_LED.setup(RGB_PINS, PWM_CH, &RGBTIMER, 0);
    int current_state = WELCOME;
    int pressed;
    char Buff_UART[32];
    float RPMS;

    while (1)
    {
        if (timer.interruptAvailable())
        {
            int len = UART.available();
            if (len > 0)
            {
                UART.read(Buff_UART, len);
                Buff_UART[len] = '\0'; // null-terminate
                if (sscanf(Buff_UART, "%d,%f", &pressed, &RPMS) != 2)
                {
                    pressed = 0;
                }
            }
            switch (current_state)

            {

            case WELCOME:
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Welcome to");
                lcd_put_cursor(1, 0);
                lcd_send_string("Viscometer");
                RGB_LED.setColor(255, 255, 0); // Yellow (awaiting)
                // Wait joystick confirmation to move on...
                break;

            case SAMPLE_PLACEMENT:
                lcd_clear();
                lcd_send_string("Place the sample");
                RGB_LED.setColor(255, 255, 0); // Yellow
                break;

            case SAMPLE_CONFIRMATION:
                lcd_clear();
                lcd_send_string("Confirm placement");
                // RGB_LED.blink(255, 255, 0); // Flash yellow
                break;

            case CYLINDER_LOWERING:
                lcd_clear();
                lcd_send_string("Lowering cylinder");
                RGB_LED.setColor(0, 255, 0); // Green ready
                break;

            case STIRRING:
                lcd_clear();
                lcd_send_string("Stirring liquid...");
                // RGB_LED.blink(255, 0, 0); // Flashing red
                break;

            case MEASURING:
                lcd_clear();
                lcd_send_string("Measuring...");
                // RGB_LED.blink(255, 0, 0);
                break;

            case SHOW_RESULTS:
                lcd_clear();
                lcd_send_string("Viscosity avg:");
                // Here you’d print measured value
                RGB_LED.setColor(0, 255, 0); // Green solid
                break;

            case SAMPLE_REMOVAL:
                lcd_clear();
                lcd_send_string("Remove sample");
                break;

            case CLEANING:
                lcd_clear();
                lcd_put_cursor(0, 0);
                lcd_send_string("Cleaning...");
                RGB_LED.setColor(0, 0, 255); // Blue

                static uint64_t cleaning_start = 0;
                if (cleaning_start == 0)
                {
                    cleaning_start = esp_timer_get_time(); // start counting
                }

                if (esp_timer_get_time() - cleaning_start >= 60000000)
                {                       // 60 seconds
                    cleaning_start = 0; // reset for next time
                    current_state += 1; // advance to next state
                }
                break;

            case DRYING:
                lcd_clear();
                lcd_send_string("Drying...");
                // Show timer (2 minutes)
                RGB_LED.setColor(255, 255, 255); // White
                break;

            case READY_FOR_NEXT:
                lcd_clear();
                lcd_send_string("Process complete");
                break;
            }
        }
        if (pressed)
        {
            current_state += 1;
        }
    }
}
