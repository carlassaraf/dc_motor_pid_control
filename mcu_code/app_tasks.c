#include "hardware/adc.h"

#include "l298.h"
#include "pid.h"
#include "as5600.h"
#include "macros.h"

#include "app_tasks.h"

#include "cJSON.h"

/**
 * @brief Callback for PID sampling
 */
float sampling(void) {
  return as5600_get_raw_angle();
}

/**
 * @brief Initialization task
 */
void task_init(void *params) {
  // Magnetic encoder initialization
  as5600_init(i2c0, SDA_PIN, SCL_PIN);
  // PID initialization
  pid_init(&(pid_config_t){
    .kp = APP_KP,
    .ki = APP_KI,
    .kd = APP_KD,
    .ts = 1,
    .ref = 2500,
    .out_min = -20000,
    .out_max = 20000,
    .cb = sampling
  });
  pid_plotter_init(100);
  // Motor driver initialization
  l298_init(*((l298_t*)params));
  // PID constant selector
  gpio_init(OPT_PIN);
  gpio_set_dir(OPT_PIN, false);
  gpio_pull_up(OPT_PIN);
  // Avoid turning on built in LED
  gpio_init(16);
  gpio_set_dir(16, true);
  gpio_put(16, false);
  // Initialize ADC and channels
  adc_init();
  adc_gpio_init(KP_POT);
  adc_gpio_init(KI_POT);
  adc_gpio_init(KD_POT);
  adc_gpio_init(REF_POT);
  // Delete task
  vTaskDelete(NULL);
}

/**
 * @brief Reads and updates PID constants
 */
void task_pid_constants(void *params) {
  // Buffer para JSON
  char json_buffer[256];
  int32_t i = 0;

  while(1) {
    // Constants from memory or potentiometer?
    if(gpio_get(OPT_PIN)) {
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

          if (cJSON_IsNumber(json_kp) && cJSON_IsNumber(json_ki) && cJSON_IsNumber(json_kd)) { 
            // Actualizo las constantes de acuerdo a lo que indique la interfaz
            float kp = json_kp->valuedouble;
            float ki = json_ki->valuedouble;
            float kd = json_kd->valuedouble;

            pid_update_constants(kp, ki, kd, get_ref());
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
    }
    else {
      pid_update_constants(APP_KP, APP_KI, APP_KD, get_ref());
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