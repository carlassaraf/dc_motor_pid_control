#ifndef _AS5600_H_
#define _AS5600_H_

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define AS5600_DEFAULT_ADDRESS  0x36

#define AS5600_ZPOS         0x01
#define AS5600_MPOS         0x03
#define AS5600_RAW_ANGLE    0x0C

void as5600_init(i2c_inst_t *i2c, uint8_t sda, uint8_t scl);
uint16_t as5600_get_raw_angle(void);

#endif