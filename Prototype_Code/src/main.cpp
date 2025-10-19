#include "definitons.h"

static void IRAM_ATTR timerinterrupt(void *arg) { timer.setInterrupt(); }

// añadir interrups
extern "C" void app_main()
{
    esp_task_wdt_deinit();
    timer.setup(timerinterrupt, "Timer");
    timer.startPeriodic(2000); // 2ms period

    // Setups
    motor1.setup(SpinMotorPin, ChMotor, PWM_Timer_BDCMotor);
    motorPID.setup(Kp, Ki, Kd);
    Button_automatic_mode.setup(4, GPIO_MODE_INPUT, GPIO_PULLUP_ONLY);
    Current_VOltage_Torque.setup(Current_VOltage_Torque_PIN);
    stepperMotor_UP_Down.setup(StepperMotorUP_Down_pins, StepperMotorUP_Down_ch, Stepper_down_up_PWM);
    StepperMotor_rotate.setup(StepperMotorRotate_pins, StepperMotorRotate_ch, Stepper_Rotate_PWM);
    Joystick_System.setup(joystickx, joysticky, joystickbutton);
    Encoder_motor.setup(Encoder_motor_pins, 0.36445f);

    // Ensure Case_Viscometer and manual_Mode / automatic_Mode are declared elsewhere (enum/const).
    // Example (if not already defined):
    // enum ViscometerCase { automatic_Mode = 0, manual_Mode = 1 };
    // ViscometerCase Case_Viscometer = automatic_Mode;

    while (1)
    {
        if (timer.interruptAvailable())
        {
            // Constantly read interlock state (active low assumed: 0 pressed)
            bool interlock_pressed = (Button_automatic_mode.read() == 0);

            // Update joystick reading once per tick
            Joystick_System.updateOnce();

            switch (ViscometerMode)
            {
            case manual_Mode:
                // If interlock isn't pressed, skip manual operations and return to automatic
                if (!interlock_pressed) {
                    Case_Viscometer = automatic_Mode;
                    break;
                }

                // Manual mode operations: joystick X -> rotation, Y -> up/down
                if (Joystick_System.getRawX() != 0) {
                    float speed = ((float)Joystick_System.getRawX() - 2048.0f) / 2048.0f * 100.0f; // Map to -100..100
                    StepperMotor_rotate.setSpeed(speed);
                } else {
                    // Optionally stop if in deadzone
                    StepperMotor_rotate.setSpeed(0.0f);
                }

                if (Joystick_System.getRawY() != 0) {
                    float speed = ((float)Joystick_System.getRawY() - 2048.0f) / 2048.0f * 100.0f; // Map to -100..100
                    stepperMotor_UP_Down.setSpeed(speed);
                } else {
                    stepperMotor_UP_Down.setSpeed(0.0f);
                }

                // If joystick button pressed -> run spindle at PID-controlled 30 RPM
                if (Joystick_System.isPressed())
                {
                    // Convert encoder reading (deg/s) to RPM: RPM = deg/s * 60 / 360
                    float measured_rpm = Encoder_motor.getSpeed() * 60.0f / 360.0f;
                    float u = motorPID.apply(180.0f, measured_rpm); // you used 180.0f previously; keeping same
                    motor1.setSpeed(u); // apply control (replace with your API if necessary)
                }

            case automatic_Mode:
            switch (AutomaticState)
            {
            case AUTO_IDLE:
                
                break;
            
            default:
                break;
            }

            default:
                break;
            } // end switch
        } // end timer interruptAvailable
    } // end while
} // end app_main
