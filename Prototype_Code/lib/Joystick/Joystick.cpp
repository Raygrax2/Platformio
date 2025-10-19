#include "Joystick.h"

// Constructor with default pins
Joystick::Joystick(){}

void Joystick::setup(uint8_t xPin ,
                     uint8_t yPin ,
                     uint8_t buttonPin) :{
    // Setup ADC and GPIO objects
    adcX.setup(pinX);
    adcY.setup(pinY);
    button.setup(pinButton, GPIO_MODE_INPUT);

    // Disable task watchdog like your original code did
    esp_task_wdt_deinit();
}

bool Joystick::isPressed() const {
    // original code considered 0 to be pressed
    return (button.get()== 0);
}

void Joystick::updateOnce() {
    lastX = adcX.read(ADC_READ_RAW);
    lastY = adcY.read(ADC_READ_RAW);
}

void Joystick::calibrate() {
    // reset calibration
    minX = 4095; minY = 4095;
    maxX = 0;    maxY = 0;

    uint64_t start = esp_timer_get_time();
    uint64_t last = 0;

    printf("Calibration start (%.0f ms)\n", float(CAL_TIME_US) / 1000.0f);

    while (esp_timer_get_time() - start <= CAL_TIME_US) {
        uint64_t now = esp_timer_get_time();
        if (now - last >= DELTA_TIME_US) {
            int vx = adcX.read(ADC_READ_RAW);
            int vy = adcY.read(ADC_READ_RAW);

            if (vx < minX) minX = vx;
            if (vx > maxX) maxX = vx;
            if (vy < minY) minY = vy;
            if (vy > maxY) maxY = vy;

            last = now;
        }
        // small yield to allow other tasks (optional)
        // vTaskDelay(1);
    }

    printf("Calibration done: minX=%d maxX=%d minY=%d maxY=%d\n", minX, maxX, minY, maxY);
}

const char* Joystick::getDirection() const {
    // If reading hasn't been updated, treat as deadpoint
    if (lastX <= 0 && lastY <= 0) return "Deadpoint";

    if (lastX < minX) return "Izquierda";
    if (lastX > maxX) return "Derecha";
    if (lastY < minY) return "Abajo";
    if (lastY > maxY) return "Arriba";
    return "Deadpoint";
}

void Joystick::runBlocking() {
    // replicate original state machine: 0 = wait for press, 1 = calibrate, 2 = position
    int state = 0;
    bool init = false;
    uint64_t last = 0;

    while (true) {
        switch (state) {
            case 0: {
                printf("Initial state: Waiting for button pressed\n");
                init = (button.get() == 0);
                if (init == true) state = 1;
                break;
            }

            case 1: {
                // Calibration
                printf("Calibration state: Calibrating joystick...\n");
                // We'll follow the same exact timing and logic as original code
                uint64_t start = esp_timer_get_time();
                // reset calibration bounds
                minX = 4095; minY = 4095;
                maxX = 0;    maxY = 0;

                while (esp_timer_get_time() - start <= CAL_TIME_US) {
                    uint64_t now = esp_timer_get_time();
                    if (now - last >= DELTA_TIME_US) {
                        lastX = adcX.read(ADC_READ_RAW);
                        lastY = adcY.read(ADC_READ_RAW);

                        if (lastX < minX) minX = lastX;
                        if (lastX > maxX) maxX = lastX;
                        if (lastY < minY) minY = lastY;
                        if (lastY > maxY) maxY = lastY;

                        last = now;
                    }
                }
                state = 2;
                break;
            }

            case 2: {
                // Position state: while button not pressed (original used pushButton.get() == 1)
                printf("Position state: Monitoring joystick until button pressed again\n");
                while (button.get() == 1) {
                    lastX = adcX.read(ADC_READ_RAW);
                    lastY = adcY.read(ADC_READ_RAW);

                    // print the direction(s) similar to original code
                    if (lastX < minX) printf("Position state: Izquierda\n");
                    if (lastX > maxX) printf("Position state: Derecha\n");
                    if (lastY < minY) printf("Position state: Abajo\n");
                    if (lastY > maxY) printf("Position state: Arriba\n");

                    // avoid flooding the console: small delay
                    // vTaskDelay(20 / portTICK_PERIOD_MS);
                }
                // once button pressed, return to initial state
                state = 0;
                break;
            }

            default:
                state = 0;
                break;
        }
    }
}
