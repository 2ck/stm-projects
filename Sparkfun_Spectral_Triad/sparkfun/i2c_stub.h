#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "stm32h7xx_hal.h"

void I2C_MspInit();

void i2c_begin();

/* TODO: sendStop */
uint8_t i2c_endTransmission(bool sendStop);

int32_t i2c_read(uint8_t addr);

uint32_t i2c_write(uint8_t addr, uint8_t data);
uint32_t i2c_write2(uint8_t addr, uint8_t data1, uint8_t data2);

bool i2c_deviceReady(uint8_t addr);
