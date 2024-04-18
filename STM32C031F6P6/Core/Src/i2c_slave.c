/*
 * i2c_slave.c
 *
 *  Created on: Apr 13, 2024
 *      Author: Jules
 */
#include <stdint.h>

#include "stm32c0xx_hal.h"
#include "main.h"
#include "i2c_slave.h"

#define RxSIZE 6
uint8_t RxData[RxSIZE];

uint32_t counterSCC = 0;

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}


/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 * Called when address match our address
 */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)
	{
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData, RxSIZE, I2C_FIRST_AND_LAST_FRAME);
	}
	else
	{
		Error_Handler();
	}
}

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 * Called when we complete the buffer RxData
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	counterSCC++;
}

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 * Called when we complete the buffer RxData
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}























