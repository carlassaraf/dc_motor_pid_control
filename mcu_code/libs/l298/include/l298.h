#ifndef _L298_H_
#define _L298_H_

#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Clock div for PWM slice
#define CLK_DIV 125.0
// Wrap value for 20 ms PWM
#define WRAP    20000

/**
 * @brief L298 struct for library
 */
typedef struct {
  uint8_t dir1_pin; // IN1 pin in module
  uint8_t dir2_pin; // IN2 pin in module
  uint8_t en_pin;   // ENA pin in module
} l298_t;

// Function prototypes

void l298_init(l298_t l298);

// Inline functions

/**
 * @brief Stops motor from running
 * @param l298 struct representing L298 module
 */
static inline void l298_stop(l298_t l298) {
  gpio_put(l298.dir1_pin, false);
  gpio_put(l298.dir2_pin, false);
}

/**
 * @brief Sets motor direction
 * @param l298 struct representing L298 module
 * @param dir true clockwise, false counter clockwise
 */
static inline void l298_set_dir(l298_t l298, bool dir) {
  // Stops motor first
  l298_stop(l298);
  // true then clockwise, false counter clockwise
  if(dir) { gpio_put(l298.dir1_pin, true); }
  else { gpio_put(l298.dir2_pin, true); }
}

/**
 * @brief Sets motor speed from 0 to 100%
 * @param l298 struct representing L298 module
 * @param speed value from 0 to 100
 */
static inline void l298_set_speed(l298_t l298, uint8_t speed) {
  // Checks value first
  if(speed > 100) { speed = 100; }
  // Sets speed according to wrap
  pwm_set_gpio_level(l298.en_pin, (uint16_t) (speed * WRAP / 100.0));
}

#endif