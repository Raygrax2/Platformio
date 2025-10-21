// main.cpp
#include "definitons.h"

// helper prototypes
float measureViscosity();                       // measure viscosity using ADC+encoder
float computeDose_ml(float meas, float target); // compute ml to add
bool performDose(float ml);                     // call pump for ml (returns success)
void performStir();                             // stir at high RPM for a time
void waitSettle();                              // wait for foam to settle
void stopAllMotion();

// timer interrupt helper (already used)
void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

// ---------------------------------------------------------------------------
// Minimal implementations (placeholders) - replace with real implementations
// ---------------------------------------------------------------------------
static float measureViscosity()
{
    // Placeholder: read Current_VOltage_Torque ADC and encoder speed to compute viscosity
    // Replace with your real conversion (torque = Kt*(I-I0); visc = f(torque,omega))
    float adc = Current_VOltage_Torque.read(ADC_READ_MV);  // 
    float enc_deg_s = Encoder_motor.getSpeed(); // degrees per second
    float rpm = enc_deg_s * 60.0f / 360.0f;
    // Dummy relation for testing:
    float torque = adc;                                 // placeholder
    float visc = torque * 1.0f / (rpm + 1.0f) * 100.0f; // purely illustrative not real eq
    return visc;
}

static float computeDose_ml(float meas, float target) // can use PID here thats why is a separate function
{
    float diff = meas - target;
    if (diff <= 0.0f)
        return 0.0f;
    float ml = diff * 0.5f;
    if (ml < 1.0f)
        ml = 1.0f;
    return ml;
}

static bool performDose(float ml)
{
    // Placeholder: call pump driver with calibrated time for ml
    // Return true on success, false on failure
    added_ml += ml;
    PUMP.set_speed(upump_speed);//controlled by a variable of PID
    return (added_ml <= MAX_ADD_ML);
}



static void waitSettle()
{
    // Placeholder wait: vTaskDelay or non-blocking timer
    vTaskDelay(pdMS_TO_TICKS(2000)); // short wait for demonstration (2s)
}

static void stopAllMotion()
{
    stepperMotor_UP_Down.setSpeed(0.0f);
    StepperMotor_rotate.setSpeed(0.0f);
    motor1.setSpeed(0.0f);
}

// ---------------------------------------------------------------------------
// Main application entry
// ---------------------------------------------------------------------------
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(2000); // 2ms period

    // Setups (use your libs' setup APIs)
    motor1.setup(SpinMotorPin, ChMotor, PWM_Timer_BDCMotor);
    motorPID.setup(Kp, Ki, Kd);
    Button_automatic_mode.setup(4, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    Current_VOltage_Torque.setup(Current_VOltage_Torque_PIN);
    stepperMotor_UP_Down.setup(StepperMotorUP_Down_pins, StepperMotorUP_Down_ch, Stepper_down_up_PWM);
    StepperMotor_rotate.setup(StepperMotorRotate_pins, StepperMotorRotate_ch, Stepper_Rotate_PWM);
    Joystick_System.setup(joystickx_pin, joysticky_pin, joystickbutton_pin);
    Encoder_motor.setup(Encoder_motor_pins, 0.36445f);

    // default mode
    Case_Viscometer = automatic_Mode;
    Auto_State = AUTO_IDLE;
    Manual_State = MANUAL_IDLE;

    while (1)
    {
        if (timer.interruptAvailable())

            // Read interlock every tick (active low = pressed)
            bool interlock_pressed = (Button_automatic_mode.get() == 0);

        // Update joystick so .isPressed() works correctly
        Joystick_System.updateOnce();

        // Enter manual while interlock held; otherwise automatic
        if (interlock_pressed)
        {
            Case_Viscometer = manual_Mode;
        }
        else
        {
            Case_Viscometer = automatic_Mode;
        }

        // --- MANUAL MODE HANDLER ---
        if (Case_Viscometer == manual_Mode)
        {
            // Manual state machine - pressing joystick advances to next state
            switch (Manual_State)
            {
            case MANUAL_IDLE:
                if (Joystick_System.isPressed())
                {
                    Manual_State = MANUAL_COLOR_READ;
                }
                break;

            case MANUAL_COLOR_READ:
                if (Joystick_System.isPressed())
                {
                    // (optionally read color sensor and set targetViscosity)
                    Manual_State = MANUAL_MOVE_TURRET;
                }
                break;

            case MANUAL_MOVE_TURRET:
            {
                int rawX = Joystick_System.getRawX();
                int rawY = Joystick_System.getRawY();
                stepperMotor_UP_Down.setSpeed((rawY - 2048) / 2048.0f * 100.0f); // scale to -100 to +100
                StepperMotor_rotate.setSpeed((rawX - 2048) / 2048.0f * 100.0f);  // scale to -100 to +100

                if (Joystick_System.isPressed())
                {
                    stepperMotor_UP_Down.setSpeed(0.0f);
                    StepperMotor_rotate.setSpeed(0.0f);
                    Manual_State = MANUAL_MEASURE;
                }
            }
            break;

            case MANUAL_MEASURE:
                measuredViscosity = measureViscosity();
                if (Joystick_System.isPressed())
                {
                    Manual_State = MANUAL_EVALUATE;
                }
                break;

            case MANUAL_EVALUATE:
                if (measuredViscosity > targetViscosity)
                {
                    if (Joystick_System.isPressed())
                    {
                        iteration_count = 0;
                        Manual_State = MANUAL_COMPUTE_DOSE;
                    }
                }
                else
                {
                    if (Joystick_System.isPressed())
                    {
                        Manual_State = MANUAL_COMPLETE;
                    }
                }
                break;

            case MANUAL_COMPUTE_DOSE:
            {
                float dose_ml = computeDose_ml(measuredViscosity, targetViscosity);
                (void)dose_ml;
                if (Joystick_System.isPressed())
                    Manual_State = MANUAL_DOSE;
            }
            break;

            case MANUAL_DOSE:
            {
                float dose_ml = computeDose_ml(measuredViscosity, targetViscosity);
                bool ok = performDose(dose_ml);
                if (!ok)
                {
                    Manual_State = MANUAL_COMPLETE;
                }
                else
                {
                    if (Joystick_System.isPressed())
                        Manual_State = MANUAL_STIR;
                }
            }
            break;

            case MANUAL_STIR:
                if (Joystick_System.isPressed())
                {
                    //might implementate pulling to time eleapses 
                    
                    motor1.setSpeed(100.0f);
                    Manual_State = MANUAL_WAIT_SETTLE;
                }
                break;

            case MANUAL_WAIT_SETTLE:
                waitSettle();
                if (Joystick_System.isPressed())
                Manual_State = MANUAL_MEASURE;
                break;

            case MANUAL_CLEANING:
                if (Joystick_System.isPressed())
                {
                    // cleaning placeholder
                    Manual_State = MANUAL_COMPLETE;
                }
                break;

            case MANUAL_COMPLETE:
                if (Joystick_System.isPressed())
                {
                    Manual_State = MANUAL_IDLE;
                }
                break;

            default:
                Manual_State = MANUAL_IDLE;
                break;
            } // end switch Manual_State

            // Do not advance automatic while interlock held
            continue;
        } // end if manual mode
        // --- AUTOMATIC MODE (default) ---
        if (Case_Viscometer == automatic_Mode)
        {

            Auto_State = AUTO_IDLE;
        }
    } // end while
}
