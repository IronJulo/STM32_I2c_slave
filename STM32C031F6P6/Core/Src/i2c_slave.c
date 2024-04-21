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

#define RxSIZE 2
uint8_t RxData[RxSIZE];
uint8_t RxCounter = 0;

#define TRANSMISSION_WRITE_SIZE 2

uint8_t registers[32] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

uint32_t error_counter = 0;

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 */
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
	HAL_I2C_EnableListen_IT(hi2c);
}

void set_register(uint8_t reg_addr, uint8_t reg_value) {
	if (reg_addr > 32) {
		return;
	}
	registers[reg_addr] = reg_value;
}

uint8_t get_register(uint8_t reg_addr) {
	if (reg_addr > 32) {
		return 0;
	}
	return registers[reg_addr];
}

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 * Called when address match our address
 */
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
	if (TransferDirection == I2C_DIRECTION_TRANSMIT)
	{
		for (int index = 0; index < RxSIZE; ++index) RxData[index] = 0;
		RxCounter = 1;
		HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData, TRANSMISSION_WRITE_SIZE, I2C_FIRST_FRAME);
	}
	else
	{
		uint8_t reg_value = get_register(RxData[0]);
		HAL_I2C_Slave_Seq_Transmit_IT(hi2c, &reg_value, 1, I2C_FIRST_AND_LAST_FRAME);
	}
}

void process_data(I2C_HandleTypeDef *hi2c) {
	set_register(RxData[0], RxData[1]);
	//HAL_I2C_EnableListen_IT(hi2c);
}

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 * Called when we complete the buffer RxData
 */
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	RxCounter++;
	switch (RxCounter) {
		case RxSIZE - 1: // before last frame
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData+RxCounter, 1, I2C_LAST_FRAME);
			break;
		case RxSIZE: // all frame received
			process_data(hi2c);
			break;
		default: // more than one frame to be received
			HAL_I2C_Slave_Seq_Receive_IT(hi2c, RxData+RxCounter, 1, I2C_NEXT_FRAME);
			break;
	}
}

/**
 * Overrides the __weak function in stm32c0xx_hal_i2c.c
 * Called when we complete the buffer RxData
 */
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
	error_counter++;

	uint32_t error_code = HAL_I2C_GetError(hi2c);

	switch (error_code) {
		case 4: // Acknowledge failure
			__HAL_I2C_CLEAR_FLAG(hi2c, I2C_FLAG_AF);
			break;
		default:
			break;
	}
	HAL_I2C_EnableListen_IT(hi2c);
}























