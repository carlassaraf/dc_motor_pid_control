#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "l298.h"
#include "pid.h"
#include "plotter.h"
#include "as5600.h"
#include "macros.h"
#include "app_tasks.h"

/**
 * @brief Callback for PID sampling
 */
float sampling(void) {
  return as5600_get_raw_angle();
}

// External queue for sharing PID data
extern QueueHandle_t pid_queue;

/**
 * @brief Programa principal
 */
int main(void) {

  stdio_init_all();

  // Timing constants
  const uint32_t pid_sampling_interval_ms = 1;
  const uint32_t plotting_time_interval_ms = 100;

  // Motor driver initialization
  l298_t l298 = {
    .dir1_pin = 12,
    .dir2_pin = 13,
    .en_pin = 14
  };

  // Magnetic encoder initialization
  as5600_init(i2c0, SDA_PIN, SCL_PIN);
  // PID initialization
  pid_constants_t pid_constants = { .kp = APP_KP, .ki = APP_KI, .kd = APP_KD, .ts = pid_sampling_interval_ms };
  pid_input_t pid_inputs = { .ref = 0.0, .cb = sampling, .curr = 0.0 };
  pid_output_t pid_outputs = { .min = APP_MIN_OUTPUT, .max = APP_MAX_OUTPUT, .curr = 0.0 };
  pid_error_t pid_errors = {0};
  pid_t pid = { .constants = pid_constants, .input = pid_inputs, .output = pid_outputs, .error = pid_errors };
  pid_init(&pid);  
  // Plotter initialization
  pid_plotter_init();
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

  // Queue initialization
  pid_queue = xQueueCreate(1, sizeof(pid_t));

  // Task creation

  xTaskCreate(task_pid_run, "PID Run", tskPID_RUN_STACK, (void*) &pid_sampling_interval_ms, tskPID_RUN_PRIO, NULL);
  xTaskCreate(task_pid_constants, "PID Const", tskPID_CONS_STACK, NULL, tskPID_CONS_PRIO, NULL);
  xTaskCreate(task_pid_output, "PID Out", tskPID_OUT_STACK, (void*) &l298, tskPID_OUT_PRIO, NULL);
  xTaskCreate(task_plotter, "Plotter", tskPLOTTER_STACK, (void*) &plotting_time_interval_ms, tskPLOTTER_PRIO, NULL);

  vTaskStartScheduler();
  while (true);
}