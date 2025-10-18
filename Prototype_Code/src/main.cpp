#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg) {timer.setInterrupt(); }

//añadir interrups
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(2000); //2ms period
    //Setups
    motor1.setup(SpinMotorPin, ChMotor, PWM_Timer_BDCMotor);
    motorPID.setup(Kp, Ki, Kd);
    Button_automatic_mode.setup(4, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    Current_VOltage_Torque.setup(Current_VOltage_Torque_PIN);
    while(1)
    {
        if (timer.interruptAvailable())
        {
           
        }
    }
}
