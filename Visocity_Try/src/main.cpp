#include "Definitions.h"
static void IRAM_ATTR timerinterrupt(void *arg) { Timer.setInterrupt(); }

extern "C" void app_main()
{
    Timer.setup(timerinterrupt, "Timer_global");
    Timer.startPeriodic(dt);
    if (Timer.interruptAvailable())
    {
        
    }
}