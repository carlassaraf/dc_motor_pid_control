#include "FreeRTOS.h"
#include "task.h"

#define tskPID_RUN_STACK  (2 * configMINIMAL_STACK_SIZE)
#define tskPID_CONS_STACK (4 * configMINIMAL_STACK_SIZE)
#define tskPID_OUT_STACK  (2 * configMINIMAL_STACK_SIZE)
#define tskPLOTTER_STACK  (4 * configMINIMAL_STACK_SIZE)

#define tskPID_RUN_PRIO   (tskIDLE_PRIORITY + 2)
#define tskPID_CONS_PRIO  (tskIDLE_PRIORITY + 4)
#define tskPID_OUT_PRIO   (tskIDLE_PRIORITY + 1)
#define tskPLOTTER_PRIO   (tskIDLE_PRIORITY + 3)

void task_pid_run(void *params);
void task_plotter(void *params);
void task_pid_constants(void *params);
void task_pid_output(void *params);