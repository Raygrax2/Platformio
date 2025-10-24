#include <SimpleADC.h>
#include <SimpleGPIO.h>
#include <esp_timer.h>

#include "Joystick.h"
#include <esp_task_wdt.h>
static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

#define five_mili_delay 5000 // 5 miliseconds
#define JOYSTICK_X_PIN GPIO_NUM_14
#define JOYSTICK_Y_PIN GPIO_NUM_13

extern "C" void app_main()
{
    SimpleTimer timer;
    esp_task_wdt_deinit();
    Joystick Joy;
    Joy.setup(JOYSTICK_X_PIN,JOYSTICK_Y_PIN,34);
    Joy.calibrate(1000000);
    while (true)
    {
        Joy.result();
    }
}
