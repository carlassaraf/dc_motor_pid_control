#include <stdio.h>
#include "pico/stdlib.h"

#include "hardware/adc.h"

#include "app_tasks.h"

// Creo un repeating timer
static repeating_timer_t timer;

int main(void) {

    stdio_init_all();

    // Inicializacion del ADC
    adc_init();
    adc_gpio_init(26);
    adc_gpio_init(27);

    // Configuro un timer para asegurar la frecuencia de muestreo
    add_repeating_timer_ms(
        20,
        adc_convertion,
        NULL,
        &timer
    );

    while (true) {
        // Veo si termino la conversion
        if(sampling_done) {
            // Leo la referencia
            adc_select_input(1);
            int16_t reference = adc_read();
            // Calculo el nuevo error
            error_curr = reference - sample_curr;
            sampling_done = false;
        }
    }
}
