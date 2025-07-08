#include "hardware/adc.h"
// Macros and labels for pins and expressions used

// Potentiometer GPIOs

#define KP_POT    26
#define KI_POT    27
#define KD_POT    28
#define REF_POT   29

// ADC channels for potentiometers

#define KP_CH     (KP_POT - 26)
#define KI_CH     (KI_POT - 26)
#define KD_CH     (KD_POT - 26)
#define REF_CH    (REF_POT - 26)

// PID constant limits

// #define KP_MAX    20000.0
#define KP_MAX    8.0
#define KI_MAX    2.0
#define KD_MAX    1.0
#define REF_MAX   3250.0

// I2C GPIOs

#define SDA_PIN   4
#define SCL_PIN   5

#define I2C i2c0

// L298 GPIOs

#define DIR1_PIN  16
#define DIR2_PIN  17
#define PWM_PIN   18

// GPIO for switch

#define OPT_PIN   6

// Absolute value

#define ABS(x)      ((x < 0)? -x : x)

// APP defined PID constants

#define APP_KP  3.5
#define APP_KI  0.15
#define APP_KD  0.05

/**
 * @brief Get value from channel
 * @param ch channel to read
 * @return 12 bit ADC value
 */
static inline uint16_t adc_get_value(uint8_t ch) {
  adc_select_input(ch);
  uint16_t prom = 0;
  for(uint8_t i = 0; i < 10; i++) { prom += adc_read(); }
  return (uint16_t) (prom / 10.0);
}

/**
 * @brief Get KP value from potentiometer
 * @return 0 to KP_MAX value
 */
static inline float get_kp(void) {
  return KP_MAX * adc_get_value(KP_CH) / 4095.0;
}

/**
 * @brief Get KI value from potentiometer
 * @return 0 to KI_MAX value
 */
static inline float get_ki(void) {
  return KI_MAX * adc_get_value(KI_CH) / 4095.0;
}

/**
 * @brief Get KD value from potentiometer
 * @return 0 to KD_MAX value
 */
static inline float get_kd(void) {
  return KD_MAX * adc_get_value(KD_CH) / 4095.0;
}

/**
 * @brief Get REF value from potentiometer
 * @return 0 to REF_MAX value
 */
static inline float get_ref(void) {
  return REF_MAX * (4095.0 - adc_get_value(REF_CH)) / 4095.0;
}