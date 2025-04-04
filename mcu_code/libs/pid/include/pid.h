#ifndef _PID_H_
#define _PID_H_

#include <stdio.h>
#include "pico/stdlib.h"

// Macros to refactor degree values

#define TO_DEG(x)   (x * 360.0 / 4095)
#define TO_PER(x)   (x * 100.0 / 1000)

/**
 * @brief Configuration struct for PID
 */
typedef struct {
  float kp;           // Proportional constant
  float ki;           // Integral constant
  float kd;           // Derivative constant
  float ts;           // Sampling time in ms
  float ref;          // Reference value
  float out_min;      // Min output value from PID
  float out_max;      // Max output value from PID
  float (*cb)(void);  // Callback function for sampling every ts
} pid_config_t;

/**
 * @brief PID values struct
 */
typedef struct {
  float prev_err;     // Previous error
  float out;          // PID output value
} pid_values_t;

/**
 * @brief PID data for plotter app
 */
typedef struct {
  float kp;
  float ki;
  float kd;
  float ref;
  float pos;
  float err;
  float out;
  float ts;
  float tp;
} pid_to_plotter_t;

// Macros

// Proportional constant from PID config struct
#define KP  (pid_config->kp)
// Integral constant from PID config struct
#define KI  (pid_config->ki)
// Derivative constant from PID config struct
#define KD  (pid_config->kd)
// Sampling time in us from PID config struct
#define DT  (pid_config->ts / 1000)

// Function prototipes

void pid_init(pid_config_t *pid_config);
void pid_plotter_init(uint32_t t);
float pid_get_output(void);
void pid_update_constants(float kp, float ki, float kd);
void pid_update_sampling_time(float ts);
pid_to_plotter_t pid_get_plot_data(void);

#endif