#ifndef _PLOTTER_H_
#define _PLOTTER_H_

#include <stdio.h>
#include "pico/stdlib.h"

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
} plot_data_t;

// Prototipes

void pid_plotter_init(void);
void plotter_plot(plot_data_t data);

#endif