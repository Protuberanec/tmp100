/*
 * i2c.h
 *
 *  Created on: 3 ���. 2022 �.
 *      Author: Tr
 */

#ifndef I2C_H_
#define I2C_H_

#include <stm32f0xx.h>

void i2c_init();
void i2c_sendByte(uint8_t addr, uint8_t byte);
void i2c_sendStr(uint8_t addr, uint8_t *data, uint8_t size);
void i2c_getByte(uint8_t addr, uint8_t *byte);
void i2c_getStr(uint8_t addr, uint8_t *data, uint8_t size);

#endif /* I2C_H_ */
