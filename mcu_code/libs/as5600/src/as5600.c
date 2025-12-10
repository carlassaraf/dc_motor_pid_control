#include "as5600.h"

// Local I2C instance used
i2c_inst_t *_i2c;

/**
 * @brief Initializes I2C to communicate with the sensor
 * @param i2c I2C instance used
 * @param sda GPIO used for SDA
 * @param scl GPIO used for SCL
 */
void as5600_init(i2c_inst_t *i2c, uint8_t sda, uint8_t scl) {
  // Initialize I2C
  i2c_init(i2c, 400000);
  gpio_set_function(sda, GPIO_FUNC_I2C);
  gpio_set_function(scl, GPIO_FUNC_I2C);
  gpio_pull_up(sda);
  gpio_pull_up(scl);

  // Store I2C instance used
  _i2c = i2c;
}

/**
 * @brief Reads raw angle from AS5600
 * @return 12 bit raw angle
 */
uint16_t as5600_get_raw_angle(void) {
  // Local variables to read
  uint8_t buff[2] = {0}, reg = AS5600_RAW_ANGLE;
  // Read raw angle from register
  i2c_write_blocking(_i2c, AS5600_DEFAULT_ADDRESS, &reg, 1, true);
  i2c_read_blocking(_i2c, AS5600_DEFAULT_ADDRESS, buff, 2, false);
  return ((buff[0] & 0xf) << 8 | buff[1]);
}