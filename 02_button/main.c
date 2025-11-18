#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define LED GPIO_NUM_27
#define BUTTON GPIO_NUM_5

void punto()
{
    gpio_set_level(LED, 1);
    vTaskDelay(pdMS_TO_TICKS(200));
    gpio_set_level(LED, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
}

void raya()
{
    gpio_set_level(LED, 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(LED, 0);
    vTaskDelay(pdMS_TO_TICKS(500));
}

void sos()
{
    for(int i = 0; i < 3; i++) {
        punto();
    }
    for (int i = 0; i < 3; i++) {
        raya();
    }
    for(int i = 0; i < 3; i++) {
        punto();
    }
}


void app_main(void)
{
    gpio_reset_pin(LED);
    gpio_reset_pin(BUTTON);

    gpio_set_direction(LED, GPIO_MODE_OUTPUT);

    gpio_set_direction(BUTTON, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON, GPIO_PULLUP_ONLY);

    bool status = gpio_get_level(BUTTON);
    bool led = false;

    while(true)
    {
        if(gpio_get_level(BUTTON) == false)
        {
            vTaskDelay(pdMS_TO_TICKS(50));

            if (gpio_get_level(BUTTON) == false)
            {
                while(gpio_get_level(BUTTON) == false)
                {
                    vTaskDelay(pdMS_TO_TICKS(10));
                }

                bool status2 = false;

                for(int i = 0 ; i < 40; i++)
                {
                    if(gpio_get_level(BUTTON) == false)
                    {
                        status2 = true; 
                        break;
                    }
                    vTaskDelay(pdMS_TO_TICKS(10));
                }
                
                if(status2 == true)
                {
                   if(led == true)
                   {
                       led = false;
                   }

                   while(gpio_get_level(BUTTON) == false) 
                   { 
                    vTaskDelay(pdMS_TO_TICKS(10)); 
                   }
                }
                else 
                {
                  if(led == false) 
                  {
                      led = true; 
                  }
                }
                gpio_set_level(LED, led);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}
