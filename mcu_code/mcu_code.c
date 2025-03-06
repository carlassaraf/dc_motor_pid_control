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
    .kp = 2,
    .ki = 0,
    .kd = 0,
    .ts = 200,
    .ref = 2000,
    .out_min = -1000,
    .out_max = 1000,
    .cb = sampling
  });

  // Motor driver initialization
  l298_init((l298_t){
    .dir1_pin = 16,
    .dir2_pin = 17,
    .en_pin = 18
  });

  // PID constant selector
  gpio_init(OPT_PIN);
  gpio_set_dir(OPT_PIN, false);

  // Initialize ADC and channels
  adc_init();
  adc_gpio_init(KP_POT);
  adc_gpio_init(KI_POT);
  adc_gpio_init(KD_POT);
  adc_gpio_init(REF_POT);

  while (true) {
    // Constants from memory or potentiometer?
    if(gpio_get(OPT_PIN)) {
      // Read potentiometer scaled values
      float kp = get_kp();
      float ki = get_ki();
      float kd = get_kd();
      // Update PID constants
      pid_update_constants(kp, ki, kd);
    }

    // Get data to plot
    pid_to_plotter_t d = pid_get_plot_data();

    printf("{\"kp\":%f,\"ki\":%f,\"kd\":%f,\"ts\":%f,\"reference\":%f,\"position\":%f,\"error\":%f,\"pwm\":%f}\n", 
        d.kp, d.ki, d.kd, d.ts,
        TO_DEG(d.ref) - 180, 
        TO_DEG(d.pos) - 180, 
        TO_DEG(d.err), 
        TO_PER(d.out)
    );
    sleep_ms(200);
  }
}