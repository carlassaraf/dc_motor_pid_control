#include "pico/stdlib.h"

// Flag de conversion terminada
extern bool sampling_done;
// Muestra anterior
extern int16_t sample_prev;
// Muestra actual
extern int16_t sample_curr;
// Error anterior
extern int16_t error_prev = 0;
// Error actual
extern int16_t error_curr = 0; 

// Prototipos privados
static bool adc_convertion(repeating_timer_t *t);