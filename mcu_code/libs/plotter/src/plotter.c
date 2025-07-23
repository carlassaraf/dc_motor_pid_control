#include "plotter.h"

/**
 * @brief Initialize PID plotter
 */
void pid_plotter_init(void) {
  // Guardo la info en el plotter data
  stdio_init_all();
}

/**
 * @brief Plots data to USB
 */
void plotter_plot(plot_data_t data) {
  // Get data to plot
  printf("{\"kp\":%f,\"ki\":%f,\"kd\":%f,\"ts\":%f,\"tp\":%f,\"ref\":%f,\"position\":%f,\"error\":%f,\"pwm\":%f}\n", 
      data.kp, data.ki, data.kd, data.ts, data.tp,
      data.ref, data.pos, data.err, data.out
  );
}