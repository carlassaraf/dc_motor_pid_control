#include "pid.h"

// Local PID configuration
static pid_t *s_pid;

/**
 * @brief Initialize PID constants
 * @param config pointer to PID configuration struct
 */
void pid_init(pid_t *pid) {
  // Copy PID config variables
  pid->input.ref += 90.0;
  pid->error.accu = 0.0;
  pid->error.prev = 0.0;
  s_pid = pid;
}

/**
 * @brief Get PID output value
 * @return value from pid_config.out_min to pid_config.out_max
 */
float pid_get_output(void) {
  return s_pid->output.curr;
}

/**
 * @brief Update PID constants
 * @param kp proportional constant
 * @param ki integral constant
 * @param kd derivative constant
 * @param ref reference value
 */
void pid_update_constants(float kp, float ki, float kd, float ref) {
  s_pid->constants.kp = kp;
  s_pid->constants.ki = ki;
  s_pid->constants.kd = kd;
  s_pid->input.ref = ref;
}

/**
 * @brief Runs PID algorithm. Calls user defined function for sampling
 * @return current PID values
 */
pid_t pid_run(void) {
  // Get new sample
  s_pid->input.curr = s_pid->input.cb();
  // Calculate error
  s_pid->error.curr = s_pid->input.ref - s_pid->input.curr;
  // Add error for integral
  s_pid->error.accu += s_pid->error.curr;
  // PID formula
  float output = KP * s_pid->error.curr + KI * s_pid->error.accu * DT;
  // Add derivative if there was a previous error (avoid initial derivative kick)
  output += (s_pid->error.prev == 0.0)? 0 : KD * (s_pid->error.curr - s_pid->error.prev) / DT;
  // Update previous error
  s_pid->error.prev = s_pid->error.curr;
  // Check boundaries
  if(output > s_pid->output.max) { s_pid->output.curr = s_pid->output.max; }
  else if(output < s_pid->output.min) { s_pid->output.curr = s_pid->output.min; }
  else { s_pid->output.curr = output; }

  return *s_pid;
}