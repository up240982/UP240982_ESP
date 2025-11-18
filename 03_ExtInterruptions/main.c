#include <stdio.h> // Incluye funciones de entrada/salida estándar
#include "freertos/FreeRTOS.h" // Incluye FreeRTOS para gestión de tareas
#include "freertos/task.h"
#include "driver/gpio.h" // Incluye funciones para manejar GPIO
#include "inttypes.h" // Incluye definiciones de tipos enteros

#define INT_PIN GPIO_NUM_5 // Define el pin GPIO5 como pin de interrupción
#define LED GPIO_NUM_26    // Define el pin GPIO26 para el LED

uint16_t int_count = 0; // Variable para contar interrupciones
bool button_state = false; // Bandera para indicar si el botón fue presionado

// Manejador de la interrupción externa
// Esta función se ejecuta cada vez que se detecta un flanco de subida en INT_PIN
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    int_count++;
    button_state = true; // Activa la bandera para notificar al bucle principal
}

// Función principal del programa
void app_main(void)
{
    // --- Configuración del pin de interrupción (botón) ---
    gpio_reset_pin(INT_PIN); // Resetea la configuración previa del pin
    gpio_set_direction(INT_PIN, GPIO_MODE_INPUT); // Configura el pin como entrada
    gpio_set_pull_mode(INT_PIN, GPIO_PULLUP_ONLY); // Activa resistencia pull-up interna
    gpio_set_intr_type(INT_PIN, GPIO_INTR_POSEDGE); // Configura interrupción por flanco de subida

    // --- Configuración del pin del LED ---
    gpio_reset_pin(LED); // Resetea la configuración previa del pin del LED
    gpio_set_direction(LED, GPIO_MODE_OUTPUT); // Configura el pin como salida

    // --- Instalación del servicio de interrupciones ---
    gpio_install_isr_service(0); // Instala el servicio de ISR para GPIO (prioridad 0)
    gpio_isr_handler_add(INT_PIN, gpio_isr_handler, NULL); // Registra nuestro manejador para el pin específico

    gpio_intr_enable(INT_PIN); // Habilita la interrupción en el pin

    while(1)
    {
        if (button_state)
        {
            while(gpio_get_level(GPIO_NUM_5) == true)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            vTaskDelay(pdMS_TO_TICKS(50));

            while(gpio_get_level(GPIO_NUM_5) == false)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
            }

            vTaskDelay(pdMS_TO_TICKS(50));*/

            if (int_count >= 3)
            {
                gpio_set_level(LED, 1); 
            }
            
            button_state = false;
        }
         printf("%d\n", int_count);
    }
}
