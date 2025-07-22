#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "l298.h"
#include "pid.h"
#include "as5600.h"
#include "macros.h"
#include "app_tasks.h"

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

  // Motor driver initialization
  l298_t l298 = {
    .dir1_pin = 12,
    .dir2_pin = 13,
    .en_pin = 14
  };

  // Magnetic encoder initialization
  as5600_init(i2c0, SDA_PIN, SCL_PIN);
  // PID initialization
  pid_init(&(pid_config_t){
    .kp = APP_KP,
    .ki = APP_KI,
    .kd = APP_KD,
    .ts = 1,
    .ref = 90.0,
    .out_min = -20000,
    .out_max = 20000,
    .cb = sampling
  });
  pid_plotter_init(100);
  // Motor driver initialization
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

  xTaskCreate(task_pid_run, "PID Run", tskPID_RUN_STACK, NULL, tskPID_RUN_PRIO, NULL);
  xTaskCreate(task_pid_constants, "PID Const", tskPID_CONS_STACK, NULL, tskPID_CONS_PRIO, NULL);
  xTaskCreate(task_pid_output, "PID Out", tskPID_OUT_STACK, (void*) &l298, tskPID_OUT_PRIO, NULL);
  xTaskCreate(task_plotter, "Plotter", tskPLOTTER_STACK, NULL, tskPLOTTER_PRIO, NULL);

  vTaskStartScheduler();
  while (true);
}