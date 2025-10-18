#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "SimpleGPIO.h"
#include "SimpleADC.h"
#include "esp_task_wdt.h"

class Joystick {
public:
    // Constructor takes pins (defaults match your original defines)
    Joystick(gpio_num_t xPin ,
             gpio_num_t yPin ,
             gpio_num_t buttonPin);

    // Initialize peripherals (must be called before runBlocking or other methods)
    void setup();

    // Runs the same state machine as your original code (blocking)
    // This contains the infinite loop; call from a dedicated task or main.
    void runBlocking();

    // Calibrate separately if you want to call it on demand
    void calibrate();

    // Non-blocking single read (reads ADCs and updates internal values)
    void updateOnce();

    // Get last raw readings
    int getRawX() const { return lastX; }
    int getRawY() const { return lastY; }

    // Returns true if button is pressed (active low in your code: pressed == 0)
    bool isPressed() const;

    // Simple direction helper string (Deadpoint, Izquierda, Derecha, Abajo, Arriba)
    const char* getDirection() const;

private:
    gpio_num_t pinX;
    gpio_num_t pinY;
    gpio_num_t pinButton;

    SimpleADC adcX;
    SimpleADC adcY;
    SimpleGPIO button;

    // calibration
    int minX;
    int maxX;
    int minY;
    int maxY;

    // last raw readings
    int lastX;
    int lastY;

    // timing constants (microseconds)
    static constexpr uint64_t DELTA_TIME_US = 5000;   // 5000 µs = 5 ms
    static constexpr uint64_t CAL_TIME_US   = 50000;  // 50000 µs = 50 ms
};

#endif // __JOYSTICK_H__