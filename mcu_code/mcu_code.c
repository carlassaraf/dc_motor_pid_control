#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "l298.h"
#include "pid.h"
#include "as5600.h"
#include "macros.h"

/**
 * @brief Callback for PID sampling
 */
float sampling(void) {
  return as5600_get_raw_angle();
}

/**
 * @brief Programa principal
 */
int main(void) {

  stdio_init_all();

  // Magnetic encoder initialization
  as5600_init(i2c0, SDA_PIN, SCL_PIN);

  // PID initialization
  pid_init(&(pid_config_t){
    .kp = APP_KP,
    .ki = APP_KI,
    .kd = APP_KD,
    .ts = 1,
    .ref = 2500,
    .out_min = -20000,
    .out_max = 20000,
    .cb = sampling
  });
  pid_plotter_init(100);

  // Motor driver initialization
  l298_t l298 = {
    .dir1_pin = 12,
    .dir2_pin = 13,
    .en_pin = 14
  };
  l298_init(l298);

  // PID constant selector
  gpio_init(OPT_PIN);
  gpio_set_dir(OPT_PIN, false);
  gpio_pull_up(OPT_PIN);

  // Avoid turning on built in LED
  gpio_init(16);
  gpio_set_dir(16, true);
  gpio_put(16, false);

  // Initialize ADC and channels
  adc_init();
  adc_gpio_init(KP_POT);
  adc_gpio_init(KI_POT);
  adc_gpio_init(KD_POT);
  adc_gpio_init(REF_POT);

  while (true) {
    // Constants from memory or potentiometer?
    if(gpio_get(OPT_PIN)) {
      pid_update_constants(get_kp(), get_ki(), get_kd(), get_ref());
    }
    else {
      pid_update_constants(APP_KP, APP_KI, APP_KD, get_ref());
    }

    // Update PWM output from PID output
    int16_t pwm_output = TO_PER(pid_get_output());
    // Check direction
    if(pwm_output < 0) {
      // Clockwise direction
      l298_set_dir(l298, true);
      l298_set_speed(l298, ABS(pwm_output));
    }
    else if(pwm_output > 0) {
      // Counter clockwise direction
      l298_set_dir(l298, false);
      l298_set_speed(l298, ABS(pwm_output));
    }
    else {
      // Stop
      l298_stop(l298);
      l298_set_speed(l298, 0);
    }
  }
}