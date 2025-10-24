#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

// Función para cambiar de estado y mandar info por UART/printf
void transitionState()
{
    currentState = static_cast<ViscometerState>((currentState + 1) % STATE_COUNT);
    printf("STATE: %d, RPM: %d, VISC: %.2f\n", currentState, currentRPM, currentViscosityCP);
}

extern "C" void app_main()
{
    esp_task_wdt_deinit();

    // ------------------- SETUP -------------------
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(10000);

    JOY.setup(PinX, PinY, Button);
    JOY.calibrate(1000000);

    PWM_Stepper_ROT.setup(PWM_ROT_PIN, 0, &PWM_TimerA);
    PWM_Stepper_UP.setup(PWM_UP_PIN, 1, &PWM_UP_STEP);

    PWM_Stepper_UP.setDuty(50.0f);
    PWM_Stepper_ROT.setDuty(50.0f);

    STEPPER_ROT_DIR.setup(ROT_PIN_DIR, GPO);
    STEPPER_UP_DIR.setup(UP_PIN_DIR, GPO);

    STEPPER_UP_DIR.set(1);
    STEPPER_ROT_DIR.set(1);

    // ------------------- LOOP PRINCIPAL -------------------
    while (1)
    {
        if (timer.interruptAvailable())
        {
            JOY.result();
            printf("Current state: %d\n", currentState);

            // Solo transicionar de estado si se presiona el botón
            if (JOY.Pressed())
            {
                transitionState();
            }

            // ------------------- EJECUCIÓN SEGÚN ESTADO -------------------
            switch (currentState)
            {
            case STATE_POWER_ON:
            case STATE_SAMPLE_PLACEMENT:
            case STATE_SAMPLE_POSITIONED:
            case STATE_CYLINDER_LOWERING:
            case STATE_MANUAL_MOVEMENT:
            case STATE_MEASUREMENT_DISPLAY:
            case STATE_SAMPLE_REMOVAL:
            case STATE_CYLINDER_CLEANING:
            case STATE_CYLINDER_DRYING:
            case STATE_PROCESS_RESTART:
                currentRPM = 0;
                PWM_Stepper_ROT.setDuty(0.0f);
                PWM_Stepper_UP.setDuty(0.0f);
                break;

            case STATE_STIR_LIQUID:
                currentRPM = 160;
                PWM_Stepper_ROT.setDuty(50.0f);
                PWM_Stepper_ROT.setFrequency(160);
                break;

            case STATE_MEASURE_VISCOSITY:
                currentRPM = 25;
                PWM_Stepper_ROT.setFrequency(25);
                break;

            default:
                break;
            }

            // ------------------- CONTROL DE MOTORES CON JOYSTICK -------------------
            if (currentState == STATE_CYLINDER_LOWERING || currentState == STATE_MANUAL_MOVEMENT)
            {
                if (JOY.zero())
                {
                    PWM_Stepper_ROT.setDuty(0.0f);
                    PWM_Stepper_UP.setDuty(0.0f);
                }
                else if (JOY.Left())
                {
                    PWM_Stepper_ROT.setDuty(50.0f);
                    STEPPER_ROT_DIR.set(1);
                    PWM_Stepper_ROT.setFrequency(650);
                }
                else if (JOY.Right())
                {
                    PWM_Stepper_ROT.setDuty(50.0f);
                    STEPPER_ROT_DIR.set(0);
                    PWM_Stepper_ROT.setFrequency(650);
                }
                else if (JOY.Up())
                {
                    PWM_Stepper_UP.setDuty(50.0f);
                    STEPPER_UP_DIR.set(1);
                    PWM_Stepper_UP.setFrequency(650);
                }
                else if (JOY.Down())
                {
                    PWM_Stepper_UP.setDuty(50.0f);
                    STEPPER_UP_DIR.set(0);
                    PWM_Stepper_UP.setFrequency(650);
                }
            }
        }
    }
}
