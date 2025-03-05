#include <stdio.h>
#include "pico/stdlib.h"

#include "l298.h"
#include "labels.h"

/**
 * @brief Main program
 */
int main(void) {

    stdio_init_all();

    // L298 pins
    l298_t l298 = { 
        .dir1_pin = DIR1_PIN, 
        .dir2_pin = DIR2_PIN, 
        .en_pin = PWM_PIN
    };
    // L298 initialization
    l298_init(l298);

    while (true) {

    }
}
