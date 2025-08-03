#ifndef _APP_TASKS_H_
#define _APP_TASKS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define tskPID_RUN_STACK  (2 * configMINIMAL_STACK_SIZE)
#define tskUSB_STACK      (4 * configMINIMAL_STACK_SIZE)
#define tskADC_STACK      (2 * configMINIMAL_STACK_SIZE)
#define tskGUI_STACK      (2 * configMINIMAL_STACK_SIZE)
#define tskPID_OUT_STACK  (2 * configMINIMAL_STACK_SIZE)
#define tskPLOTTER_STACK  (4 * configMINIMAL_STACK_SIZE)

#define tskPID_RUN_PRIO   (tskIDLE_PRIORITY + 2)
#define tskUSB_PRIO       (tskIDLE_PRIORITY + 4)
#define tskADC_PRIO       (tskIDLE_PRIORITY + 2)
#define tskGUI_PRIO       (tskIDLE_PRIORITY * 2)
#define tskPID_OUT_PRIO   (tskIDLE_PRIORITY + 1)
#define tskPLOTTER_PRIO   (tskIDLE_PRIORITY + 3)

void task_pid_run(void *params);
void task_plotter(void *params);
void task_usb(void *params);
void task_adc(void *params);
void task_gui(void *params);
void task_pid_output(void *params);

typedef struct json_shareable {
  float kp;
  float ki;
  float kd;
  float ref;
} json_shareable_t;

#endif