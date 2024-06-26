#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "app_tasks.h"

// Muestra anterior
int16_t sample_prev = -1;
// Muestra actual
int16_t sample_curr = -1;
// Error anterior
int16_t error_prev = 0;
// Error actual
int16_t error_curr = 0; 

// Flag de conversion terminada
volatile bool sampling_done = false;
// Creo un repeating timer
static repeating_timer_t timer;

// Prototipos privados
static bool adc_convertion(repeating_timer_t *t);

/**
 * @brief Inicializacion de perifericos
 */
void mcu_init(void) {
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
}

float json_get_value_from_key(char *json, char *key) {
    // Busco la posicion del key
    char *key_pos = strstr(json, key);
    // No se encontro la key
    if (key_pos == NULL) { return 0.0; }

    // Encontrar la posición del colon (:) después de la clave
    char *colon_pos = strchr(key_pos, ':');
    // No se encontro
    if (colon_pos == NULL) { return 0.0; }

    // Saltar el colon y cualquier espacio en blanco
    do { colon_pos++; }
    while(*colon_pos == ' ' || *colon_pos == '\t' || *colon_pos == '\n');
    
    // Extraer el valor
    return atof(colon_pos);
}

/**
 * @brief Callback del timer para tomar una muestra
 */
static bool adc_convertion(repeating_timer_t *t) {
    // Piso la muestra anterior
    sample_prev = sample_curr;
    // Piso el error anterior
    error_prev = error_curr;
    // Elijo el ADC del potenciometro
    adc_select_input(0);
    // Tomo la muestra nueva
    sample_curr = adc_read();
    // Aviso que se termino la conversion
    sampling_done = true;
    return true;
}