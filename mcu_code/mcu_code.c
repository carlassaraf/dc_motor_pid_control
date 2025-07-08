#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "l298.h"
#include "pid.h"
#include "as5600.h"
#include "macros.h"
#include "app_tasks.h"

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

  xTaskCreate(task_init, "Init", tskINIT_STACK, (void*) &l298, tskINIT_PRIO, NULL);
  xTaskCreate(task_pid_constants, "PID Const", tskPID_CONS_STACK, NULL, tskPID_CONS_PRIO, NULL);
  xTaskCreate(task_pid_output, "PID Out", tskPID_OUT_STACK, (void*) &l298, tskPID_OUT_PRIO, NULL);

  vTaskStartScheduler();
  while (true);
}