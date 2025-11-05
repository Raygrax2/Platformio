#include "TCS34725.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "driver/i2c.h"
#include <stdio.h>
extern c{
    #include "i2c_lcd.h"
}

// I2C config — ajusta pines si tu board usa otros
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_SDA_IO 22
#define I2C_MASTER_SCL_IO 21
#define I2C_MASTER_FREQ_HZ 100000 // recomiendo 100kHz estable
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

// TCS registers
#define TCS34725_CDATAL 0x14

static esp_err_t i2c_master_init(i2c_port_t i2c_num, gpio_num_t sda, gpio_num_t scl, uint32_t freq_hz)
{
    i2c_config_t conf = {};
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.scl_io_num = scl;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = freq_hz;
    esp_err_t err = i2c_param_config(i2c_num, &conf);
    if (err != ESP_OK) return err;
    err = i2c_driver_install(i2c_num, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
    return err;
}

extern "C" void app_main(void)
{
    // disable task watchdog if needed
    esp_task_wdt_deinit();

    // init I2C driver
    esp_err_t err = i2c_master_init(I2C_MASTER_NUM, (gpio_num_t)I2C_MASTER_SDA_IO, (gpio_num_t)I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ);
    if (err != ESP_OK) {
        printf("I2C init failed: %s\n", esp_err_to_name(err));
        while (true) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // create sensor object and initialize
    TCS34725 color;
    color.begin(I2C_MASTER_NUM, 0x29); // 0x29 typical address

    // configure sensor baseline (optional)
    color.setIntegrationTime(0xD5); // ~101 ms
    color.setGain(0x01);            // 4x

    printf("TCS34725 ready. Reading raw values...\n");

    while (true) {
        uint16_t c, r, g, b;
        color.readRaw(c, r, g, b);
        printf("RAW -> C: %u | R: %u | G: %u | B: %u\n", c, r, g, b);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}
