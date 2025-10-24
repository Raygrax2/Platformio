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
    lcd_put_cursor(0, 0);
    lcd_send_string("Viscometer");

    while (1)
    {
    }
}
