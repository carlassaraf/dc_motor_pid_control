#include "pid.h"

// Local PID configuration
static pid_config_t *pid_config;
// Local PID values
static pid_values_t pid_values = {0};
// Repeating Timer used
repeating_timer_t timer;

/**
 * @brief Called every ts for sampling. Invokes user callback
 */
bool sampling_callback(repeating_timer_t *t) {
  // Previous error for derivative
  static float prev_error = 0;
  // Accumulated error for integral
  static float acc_error = 0;
  // Get new sample
  float new_sample = pid_config->cb();
  // Calculate error
  float curr_err = pid_config->ref - new_sample;
  // Add error for integral
  acc_error += curr_err;
  // PID formula
  float output = KP * curr_err + KI * acc_error * DT + KD * (curr_err - prev_error) / DT;
  // Check boundaries
  if(output > pid_config->out_max) { pid_values.out = pid_config->out_max; }
  else if(output < pid_config->out_min) { pid_values.out = pid_config->out_min; }
  else { pid_values.out = output; }
}

/**
 * @brief Initialize PID constants
 * @param config pointer to PID configuration struct
 */
void pid_init(pid_config_t *config) {
  // Copy PID config variables
  pid_config = config;
  // Callback for sampling
  add_repeating_timer_us(-(pid_config->ts), sampling_callback, NULL, &timer);
}

/**
 * @brief Get PID output value
 * @return value from pid_config.out_min to pid_config.out_max
 */
float pid_get_output(void) {
  return pid_values.out;
}