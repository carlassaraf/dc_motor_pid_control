#ifndef _PID_H_
#define _PID_H_

#include <stdio.h>
#include "pico/stdlib.h"

// Macros to refactor degree values

#define TO_DEG(x)   (180.0 * (x - 900) / 2000)
#define TO_PER(x)   (x * 100.0 / 1000)

/**
 * @brief PID tuning constants
 */
typedef struct {
  float kp; // Proportional constant
  float ki; // Integral constant
  float kd; // Derivative constant
  float ts; // Sampling time in ms
} pid_constants_t;

/**
 * @brief PID output struct
 */
typedef struct {
  float min;
  float curr;
  float max;
} pid_output_t;

/**
 * @brief PID error struct
 */
typedef struct {
  float prev; // Previous error
  float curr; // Current error
  float accu; // Accumulative error
} pid_error_t;

/**
 * @brief PID input struct
 */
typedef struct {
  float ref;  // Reference
  float curr; // Current value
  float (*cb)(void);  // Callback function for sampling
} pid_input_t;

/**
 * @brief Configuration struct for PID
 */
typedef struct {
  pid_constants_t constants;
  pid_input_t input;
  pid_output_t output;
  pid_error_t error;
} pid_t;

// Macros

// Proportional constant from PID config struct
#define KP  (s_pid->constants.kp)
// Integral constant from PID config struct
#define KI  (s_pid->constants.ki)
// Derivative constant from PID config struct
#define KD  (s_pid->constants.kd)
// Sampling time in us from PID config struct
#define DT  (s_pid->constants.ts / 1000)

// Function prototipes

void pid_init(pid_t *pid);
float pid_get_output(void);
void pid_update_constants(float kp, float ki, float kd, float ref);
pid_t pid_run(void);

#endif