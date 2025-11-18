#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"

#define AIN1 GPIO_NUM_5
#define AIN2 GPIO_NUM_17
#define PWMA GPIO_NUM_16

static const char *ESP = "Mi ESP";

adc_oneshot_unit_handle_t mikuuu;
int adc_raw = 0; 

void configuracionADC(void) {
    adc_oneshot_unit_init_cfg_t init_config = {
        .unit_id = ADC_UNIT_2,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    adc_oneshot_new_unit(&init_config, &mikuuu);

    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };
    adc_oneshot_config_channel(mikuuu, ADC_CHANNEL_3, &channel_config);
    ESP_LOGI(ESP, "ADC configurado");
}

esp_err_t configureGpio(void) {
    gpio_reset_pin(AIN1);
    gpio_reset_pin(AIN2);
    gpio_reset_pin(PWMA);
    gpio_set_direction(AIN1, GPIO_MODE_OUTPUT);
    gpio_set_direction(AIN2, GPIO_MODE_OUTPUT);
    gpio_set_direction(PWMA, GPIO_MODE_OUTPUT);
    return ESP_OK;
}

void setupPWM(void) {
    ledc_timer_config_t ledc_timer = {
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .duty_resolution = LEDC_TIMER_8_BIT,
        .timer_num = LEDC_TIMER_0,
        .freq_hz = 5000,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_A = {
        .gpio_num = PWMA,
        .speed_mode = LEDC_LOW_SPEED_MODE,
        .channel = LEDC_CHANNEL_0,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = LEDC_TIMER_0,
        .duty = 0,
        .hpoint = 0,
        .flags.output_invert = 0
    };
    ledc_channel_config(&ledc_channel_A);
    ledc_fade_func_install(0);
}

void LecturaADCTask(void *pvParameters) {
    while (1) {
        adc_oneshot_read(mikuuu, ADC_CHANNEL_3, &adc_raw);
        vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms de retardo
    }
}

void GiroMotorTask(void *pvParameters) {
    while (1) {
        int duty = (adc_raw * 255) / 4095;

        if (adc_raw <= 2048) {
            gpio_set_level(AIN1, 1);
            gpio_set_level(AIN2, 0);
        } else {
            gpio_set_level(AIN1, 0);
            gpio_set_level(AIN2, 1);
        }

        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0);

        ESP_LOGI(ESP, "ADC: %d | Duty: %d | Giro: %s", adc_raw, duty, adc_raw <= 2048 ? "Horario" : "Antihorario");
        vTaskDelay(pdMS_TO_TICKS(100)); // 100 ms de retardo
    }
}

void app_main(void) {
    configureGpio();
    setupPWM();
    configuracionADC();

    xTaskCreate(LecturaADCTask, "MIKU", 2048, NULL, 1, NULL);
    xTaskCreate(GiroMotorTask, "AKAME", 2048, NULL, 1, NULL);
}
