#include "l298.h"

/**
 * @brief Initializes output pins for a single motor output
 * @param l298 struct representing L298 module
 */
void l298_init(l298_t l298) {
  // Starts output as zero to avoid motor from running
  gpio_put(l298.dir1_pin, false); gpio_put(l298.dir2_pin, false);
  // Initialize GPIO direction pins as output
  gpio_init(l298.dir1_pin); gpio_set_dir(l298.dir1_pin, true);
  gpio_init(l298.dir2_pin); gpio_set_dir(l298.dir2_pin, true);
  // Initialize PWM output on enable GPIO
  pwm_config config = pwm_get_default_config();
  // 1MHz base clock and 20 ms of period (50 Hz PWM)
  pwm_config_set_clkdiv(&config, CLK_DIV);
  pwm_config_set_wrap(&config, WRAP);
  pwm_init(pwm_gpio_to_slice_num(l298.en_pin), &config, true);
  // Initial speed as 0
  pwm_set_gpio_level(l298.en_pin, 0);
}