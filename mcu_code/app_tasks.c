#include "hardware/adc.h"

#include "l298.h"
#include "pid.h"
#include "plotter.h"
#include "as5600.h"
#include "macros.h"

#include "app_tasks.h"

#include "cJSON.h"

// Queue to share PID data
QueueHandle_t pid_queue;

/**
 * @brief Handles PID algorithm
 */
void task_pid_run(void *params) {
  // Time interval to run PID algorithm
  uint32_t interval_ms = *((uint32_t*)params);
  // PID data
  pid_t pid = {0};

  while(1) {
    pid = pid_run();
    xQueueOverwrite(pid_queue, &pid);
    vTaskDelay(pdMS_TO_TICKS(interval_ms));
  }
}


/**
 * @brief Handles plotting data
 */
void task_plotter(void *params) {
  // Time interval to run PID algorithm
  uint32_t interval_ms = *((uint32_t*)params);
  // PID data
  pid_t pid;

  while(1) {
    xQueuePeek(pid_queue, &pid, portMAX_DELAY);
    plot_data_t data = {
      .kp = pid.constants.kp,
      .ki = pid.constants.ki,
      .kd = pid.constants.kd,
      .err = pid.error.curr,
      .out = TO_PER(pid.output.curr),
      .pos = pid.input.curr,
      .ref = pid.input.ref,
      .tp = interval_ms,
      .ts = pid.constants.ts
    };
    plotter_plot(data);
    vTaskDelay(pdMS_TO_TICKS(interval_ms));
  }
}

/**
 * @brief Reads and updates PID constants
 */
void task_pid_constants(void *params) {
  // Buffer para JSON
  char json_buffer[256];
  int32_t i = 0;
  static float kp = 0.0, ki = 0.0, kd = 0.0, ref = 0.0;

  while(1) {
    
    // Reviso el USB sin timeout
    int c = getchar_timeout_us(0);
    if (c == PICO_ERROR_TIMEOUT) {
      // Si no hay nada, vuelvo a intentar luego
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    // Fin de cadena
    if (c == '\n' || c == '\r') {
      json_buffer[i] = '\0';
      if (i > 0) {
        // Creo una instancia de un JSON like string
        cJSON *json = cJSON_Parse(json_buffer);
        if (!json) {
          i = 0;
          continue;
        }

        // Obtengo los valores desde la interfaz
        const cJSON* json_kp = cJSON_GetObjectItem(json, "kp");
        const cJSON* json_ki = cJSON_GetObjectItem(json, "ki");
        const cJSON* json_kd = cJSON_GetObjectItem(json, "kd");
        const cJSON* json_ref = cJSON_GetObjectItem(json, "ref");

        if (cJSON_IsNumber(json_kp) && cJSON_IsNumber(json_ki) && cJSON_IsNumber(json_kd) && cJSON_IsNumber(json_ref)) { 
          // Actualizo las constantes de acuerdo a lo que indique la interfaz
          kp = json_kp->valuedouble;
          ki = json_ki->valuedouble;
          kd = json_kd->valuedouble;
          ref = json_ref->valuedouble;
        }
        // Limpio el JSON
        cJSON_Delete(json);
        i = 0;
      }
    }
    else if (i < sizeof(json_buffer) - 1) {
      // Copio el caracter en el array
      json_buffer[i++] = (char)c;
    }

    if(gpio_get(OPT_PIN)) {
      pid_update_constants(kp, ki, kd, ref);
    }
    else {
      pid_update_constants(APP_KP, APP_KI, APP_KD, ref);
    }
    // Block for 10 ms
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

/**
 * @brief Updates PID output
 */
void task_pid_output(void *params) {
  // Motor driver initialization
  l298_t l298 = *(l298_t*)params;

  while(1) {
    // Update PWM output from PID output
    int16_t pwm_output = TO_PER(pid_get_output());
    // Check direction
    if(pwm_output < 0) {
      // Clockwise direction
      l298_set_dir(l298, true);
      l298_set_speed(l298, ABS(pwm_output));
    }
    else if(pwm_output > 0) {
      // Counter clockwise direction
      l298_set_dir(l298, false);
      l298_set_speed(l298, ABS(pwm_output));
    }
    else {
      // Stop
      l298_stop(l298);
      l298_set_speed(l298, 0);
    }
    // Block every 5 ms
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}