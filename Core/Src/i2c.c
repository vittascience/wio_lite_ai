/**
 ******************************************************************************
 * @file    i2c.c
 * @brief   This file provides code for the configuration
 *          of the I2C instances.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

#include "vittascience_i2c.h"

#define I2C_RECEIVE_BUFFER_LEN 20

void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode)
{
  if (hi2c->Instance == VITTASCIENCE_I2C_INSTANCE)
  {
    if (TransferDirection == I2C_DIRECTION_TRANSMIT)
    {
      vittascience_i2c_write_request();
    }
    else
    {
      vittascience_i2c_read_request();
    }
  }
}

void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == VITTASCIENCE_I2C_INSTANCE)
  {
    HAL_I2C_EnableListen_IT(hi2c);
  }
}

void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == VITTASCIENCE_I2C_INSTANCE)
  {
    vittascience_i2c_receive_cb();
    HAL_I2C_EnableListen_IT(hi2c);
  }
}

void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == VITTASCIENCE_I2C_INSTANCE)
  {
    HAL_I2C_EnableListen_IT(hi2c);
  }
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
  if (hi2c->Instance == VITTASCIENCE_I2C_INSTANCE)
  {
    HAL_I2C_EnableListen_IT(hi2c);
  }
}
