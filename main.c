/*
 * 2021 - 12 - 30
 *
 */

#include "stm32f0xx.h"

/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/

void gpio_i2c_init() {
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER6_1 /*scl*/ | GPIO_MODER_MODER7_1 /*sda*/;
	GPIOB->AFR[0] |= (1 << 24) | (1 << 28);	//for pb6,pb7 - af1
}

void i2c1_init() {
	gpio_i2c_init();
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

	I2C1->CR1 &= ~I2C_CR1_PE;	//disable i2c
	I2C1->CR1 &= ~I2C_CR1_ANFOFF;	//enable analog filter
	RCC->CFGR3 &= ~RCC_CFGR3_I2C1SW;	//get clock from HSI - 8MHz

//	I2C1->TIMINGR = 0x0010020B;	//363kHz
//	I2C1->TIMINGR = 0x01130F0204;	//173kHz
//	I2C1->TIMINGR = 0x00201D2B;	//99kHz
//	I2C1->TIMINGR = 0x00901850;
	//presc = 0
	//scldel (data setup time) 0x01 = 2*125
	//sdadel (data hold time) 0x04 = 5*125
	//scll = 0x0B //12*125ns scl low period
	//sclh = 0x05 //6*12ns scl high period
	I2C1->TIMINGR = 0x0001040409;

//	I2C1->OAR1 |= 0x15 << 1;
//	I2C1->OAR1 |= I2C_OAR1_OA1EN;

	I2C1->CR1 |= I2C_CR1_PE;	//enable i2c1
}

void i2c1_sendByte(uint8_t addr, uint8_t byte) {
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
//	for (uint8_t i = 0; i < size; i++)
	{
		while((I2C1->ISR & I2C_ISR_TXE) != I2C_ISR_TXE);
		I2C1->CR2 = I2C_CR2_AUTOEND | addr << 1 | 1 << 16;
		I2C1->CR2 |= I2C_CR2_START;
		I2C1->TXDR = byte;

	}
}

void i2c1_send_str(uint8_t addr, uint8_t *data, uint8_t size) {
	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);
	I2C1->CR2 = I2C_CR2_AUTOEND | addr << 1 | size << 16;
	I2C1->CR2 |= I2C_CR2_START;

	for (uint8_t i = 0; i < size; i++)
	{
		while((I2C1->ISR & I2C_ISR_TXE) != I2C_ISR_TXE);
		I2C1->TXDR = data[i];
	}
}

void i2c1_get(uint8_t addr, uint8_t *data, uint8_t size) {
//	I2C1->CR2 = I2C_CR2_STOP;

	while((I2C1->ISR & I2C_ISR_BUSY) == I2C_ISR_BUSY);

	I2C1->CR2 = I2C_CR2_AUTOEND | addr << 1 | size << 16 | I2C_CR2_RD_WRN;
	I2C1->CR2 |= I2C_CR2_START;

	for (uint8_t i = 0; i < size;)
	{
		if ((I2C1->ISR & I2C_ISR_RXNE) == I2C_ISR_RXNE) {
			data[i++] = I2C1->RXDR;
		}
	}
}

void tmp101_init() {
	//must be 400kHz
}

void vl53L0x_init() {

}

	float temperature;

int main(void)
{
	i2c1_init();
	tmp101_init();
	uint8_t temp_data[2] = {0x01, (3 << 5)};

	i2c1_send_str(0x48, &temp_data[0], 2);

	i2c1_sendByte(0x48, 0x01);
	i2c1_get(0x48, &temp_data[0], 1);

	while (1)
	{
//		i2c1_sendByte(0x00, 0x04, 1);
		i2c1_sendByte(0x48, 0x00);
		i2c1_get(0x48, &temp_data[0], 2);

		uint16_t temp = (temp_data[1] >> 4 | temp_data[0] << 4);
		temperature = (temp_data[1] >> 4 | temp_data[0] << 4) * 0.0625;

		for (int i = 0; i < 250000; i++);

	}

	return 0;
}
