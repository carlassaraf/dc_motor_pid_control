#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "app_tasks.h"

// Constante proporcional
float kp = 0.0;
// Constante integral
float ki = 0.0;
// Constante derivativa
float kd = 0.0;

int main(void) {

    // Array para enviar a la interfaz
    char str[128] = {0};
    // Array para recibir de la interfaz
    char input[128] = {0};
    // Index
    uint8_t i = 0;

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

        // Intento leer el stdin
        int in = getchar_timeout_us(0);
        if(in != PICO_ERROR_TIMEOUT) {
            // Copio lo recibido
            input[i++] = (char)in;
            // Veo si ya es el final
            if((char)in == '\n') {
                // Extraigo los valores
                kp = json_get_value_from_key(input, "kp");
                ki = json_get_value_from_key(input, "ki");
                kd = json_get_value_from_key(input, "kd");
                // Reinicio el index y la variable
                memset((void*)input, 0, sizeof(input));
                i = 0;
            }
        }
    }
}
