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
bool sampling_done = false;

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
}