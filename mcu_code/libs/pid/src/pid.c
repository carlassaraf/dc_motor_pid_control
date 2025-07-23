#include "pid.h"

// Local PID configuration
static pid_config_t *pid_config;
// Local PID values
static pid_values_t pid_values = {0};
// Repeating Timer used
repeating_timer_t pid_timer, plotter_timer;
// Struct with data to plot
static pid_to_plotter_t pid_data = {0};

/**
 * @brief Initialize PID constants
 * @param config pointer to PID configuration struct
 */
void pid_init(pid_config_t *config) {
  // Copy PID config variables
  config->ref += 90.0;
  pid_config = config;
}

/**
 * @brief Initialize PID plotter
 * @param t refresh time in ms
 */
void pid_plotter_init(uint32_t t) {
  // Guardo la info en el plotter data
  pid_data.tp = t / 1000.0;
}

/**
 * @brief Get PID output value
 * @return value from pid_config.out_min to pid_config.out_max
 */
float pid_get_output(void) {
  return pid_values.out;
}

/**
 * @brief Update PID constants
 * @param kp proportional constant
 * @param ki integral constant
 * @param kd derivative constant
 * @param ref reference value
 */
void pid_update_constants(float kp, float ki, float kd, float ref) {
  pid_config->kp = kp;
  pid_config->ki = ki;
  pid_config->kd = kd;
  pid_config->ref = ref;
}

/**
 * @brief Gets struct with the data for plotting interface
 * @return plot data
 */
pid_to_plotter_t pid_get_plot_data(void) {
  return pid_data;
}

/**
 * @brief Runs PID algorithm. Calls user defined function for sampling
 */
void pid_run(void) {
  // Previous error for derivative
  static float prev_error = 0;
  // Accumulated error for integral
  static float acc_error = 0;
  // Get new sample
  float new_sample = pid_config->cb();
  // Calculate error
  float curr_err = pid_config->ref - TO_DEG(new_sample);
  // Add error for integral
  acc_error += curr_err;
  // PID formula
  float output = KP * curr_err + KI * acc_error * DT;
  // Add derivative if there was a previous error (avoid initial derivative kick)
  output += (prev_error == 0.0)? 0 : KD * (curr_err - prev_error) / DT;
  // Update previous error
  prev_error = curr_err;
  // Check boundaries
  if(output > pid_config->out_max) { pid_values.out = pid_config->out_max; }
  else if(output < pid_config->out_min) { pid_values.out = pid_config->out_min; }
  else { pid_values.out = output; }

  // Update plot data
  pid_data.kp = pid_config->kp;
  pid_data.ki = pid_config->ki;
  pid_data.kd = pid_config->kd;
  pid_data.ref = pid_config->ref;
  pid_data.pos = TO_DEG(new_sample);
  pid_data.err = curr_err;
  pid_data.out = pid_values.out;
  pid_data.ts = pid_config->ts / 1000;
}

/**
 * @brief Plots data to USB
 */
void plotter_run(void) {
  // Get data to plot
  pid_to_plotter_t d = pid_get_plot_data();
  printf("{\"kp\":%f,\"ki\":%f,\"kd\":%f,\"ts\":%f,\"tp\":%f,\"ref\":%f,\"position\":%f,\"error\":%f,\"pwm\":%f}\n", 
      d.kp, d.ki, d.kd, d.ts, d.tp,
      d.ref, d.pos, d.err, TO_PER(d.out)
  );
}