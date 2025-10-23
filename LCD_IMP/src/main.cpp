#include "definitons.h"
#include "Joystick.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

extern "C" void app_main()
{

    esp_task_wdt_deinit();

    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(dt);  
    Joystick Joy;
    Joy.setup(25,26,34);
    Joy.calibrate(1000000);
    while (1)
    {

        if (timer.interruptAvailable())
        {

            Joy.result();
            if (not Joy.Pressed())
            printf("Pressed");
        }
    }
}