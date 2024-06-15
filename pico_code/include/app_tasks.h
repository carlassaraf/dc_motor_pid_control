#include "pico/stdlib.h"
#include "hardware/adc.h"

// Flag de conversion terminada
extern volatile bool sampling_done;
// Muestra anterior
extern int16_t sample_prev;
// Muestra actual
extern int16_t sample_curr;
// Error anterior
extern int16_t error_prev;
// Error actual
extern int16_t error_curr; 

// Prototipos
void mcu_init(void);