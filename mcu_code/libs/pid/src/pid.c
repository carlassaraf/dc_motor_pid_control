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
  pid_data.pos = new_sample;
  pid_data.err = curr_err;
  pid_data.out = pid_values.out;
  pid_data.ts = pid_config->ts / 1000;
}

/**
 * @brief Callback for plotting data
 */
bool plotter_callback(repeating_timer_t *t) {
  // Get data to plot
  pid_to_plotter_t d = pid_get_plot_data();
  printf("{\"kp\":%f,\"ki\":%f,\"kd\":%f,\"ts\":%f,\"tp\":%f,\"reference\":%f,\"position\":%f,\"error\":%f,\"pwm\":%f}\n", 
      d.kp, d.ki, d.kd, d.ts, d.tp,
      TO_DEG(d.ref) - 180, 
      TO_DEG(d.pos) - 180, 
      TO_DEG(d.err), 
      TO_PER(d.out)
  );
}

/**
 * @brief Initialize PID constants
 * @param config pointer to PID configuration struct
 */
void pid_init(pid_config_t *config) {
  // Copy PID config variables
  pid_config = config;
  // Callback for sampling
  add_repeating_timer_ms(-(pid_config->ts), sampling_callback, NULL, &pid_timer);
}

/**
 * @brief Initialize PID plotter
 * @param t refresh time in ms
 */
void pid_plotter_init(uint32_t t) {
  // Callback for plotting
  add_repeating_timer_ms(-(t), plotter_callback, NULL, &plotter_timer);
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
 */
void pid_update_constants(float kp, float ki, float kd) {
  pid_config->kp = kp;
  pid_config->ki = ki;
  pid_config->kd = kd;
}

/**
 * @brief Update PID sampling time
 * @param ts sampling time in ms
 */
void pid_update_sampling_time(float ts) {
  pid_config->ts = ts;
  // Update callback
  cancel_repeating_timer(&pid_timer);
  add_repeating_timer_ms(-(pid_config->ts), sampling_callback, NULL, &pid_timer);
}

/**
 * @brief Gets struct with the data for plotting interface
 * @return plot data
 */
pid_to_plotter_t pid_get_plot_data(void) {
  return pid_data;
}