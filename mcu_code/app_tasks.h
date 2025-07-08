#include "FreeRTOS.h"
#include "task.h"

#define tskINIT_STACK     (2 * configMINIMAL_STACK_SIZE)
#define tskPID_CONS_STACK (2 * configMINIMAL_STACK_SIZE)
#define tskPID_OUT_STACK  (2 * configMINIMAL_STACK_SIZE)

#define tskINIT_PRIO      (tskIDLE_PRIORITY + 2)
#define tskPID_CONS_PRIO  (tskIDLE_PRIORITY + 1)
#define tskPID_OUT_PRIO   (tskIDLE_PRIORITY + 1)

void task_init(void *params);
void task_pid_constants(void *params);
void task_pid_output(void *params);