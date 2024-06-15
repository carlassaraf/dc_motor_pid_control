#include <stdio.h>
#include "pico/stdlib.h"

#include "app_tasks.h"

int main(void) {

    // Array para enviar a la interfaz
    char str[128] = {0};

    stdio_init_all();
    sleep_ms(1000);

    // Inicializo perifericos
    mcu_init();

    while(true) {
        // Veo si termino la conversion
        if(sampling_done) {
            // Leo la referencia
            adc_select_input(1);
            int16_t reference = adc_read();
            // Calculo el nuevo error
            error_curr = reference - sample_curr;

            // Mando datos

            sprintf(
                str,
                "{\"position\":%f,\"reference\":%f,\"error\":%f}\n",
                sample_curr * 360.0 / 4095.0 - 180.0,
                reference * 360.0 / 4095.0 - 180.0,
                error_curr * 360.0 / 4095.0
            );

            printf(str);

            sampling_done = false;
        }
    }
}
