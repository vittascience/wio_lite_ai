/**
 ******************************************************************************
 * @file    octospi.c
 * @brief   This file provides code for the configuration
 *          of the OCTOSPI instances.
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
#include "octospi.h"

// #include "AP6408L.h"
#include "W25Q128.h"

// uint8_t txbuf[] = "TEST";
// uint8_t txbuf2[] = "A";
// uint8_t rxbuf[100];

OSPI_HandleTypeDef hospi1;
OSPI_HandleTypeDef hospi2;

/* OCTOSPI1 init function */
void MX_OCTOSPI1_Init(void)
{

  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  hospi1.Instance = OCTOSPI1;
  hospi1.Init.FifoThreshold = 4;
  hospi1.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi1.Init.MemoryType = HAL_OSPI_MEMTYPE_APMEMORY;
  hospi1.Init.DeviceSize = 23;
  hospi1.Init.ChipSelectHighTime = 1;
  hospi1.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi1.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi1.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi1.Init.ClockPrescaler = 2;
  hospi1.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;
  hospi1.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi1.Init.ChipSelectBoundary = 10;
  hospi1.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED;
  hospi1.Init.MaxTran = 0;
  hospi1.Init.Refresh = 0;
  if (HAL_OSPI_Init(&hospi1) != HAL_OK)
  {
    Error_Handler();
  }
  sOspiManagerCfg.ClkPort = 1;
  sOspiManagerCfg.DQSPort = 1;
  sOspiManagerCfg.NCSPort = 1;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
  sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
  if (HAL_OSPIM_Config(&hospi1, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  // Psram_Init();
}
/* OCTOSPI2 init function */
void MX_OCTOSPI2_Init(void)
{

  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  hospi2.Instance = OCTOSPI2;
  hospi2.Init.FifoThreshold = 1;
  hospi2.Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi2.Init.MemoryType = HAL_OSPI_MEMTYPE_MICRON;
  hospi2.Init.DeviceSize = 24;
  hospi2.Init.ChipSelectHighTime = 8;
  hospi2.Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi2.Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi2.Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi2.Init.ClockPrescaler = 2;
  hospi2.Init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_HALFCYCLE;
  hospi2.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_DISABLE;
  hospi2.Init.ChipSelectBoundary = 0;
  hospi2.Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_BYPASSED;
  hospi2.Init.MaxTran = 0;
  hospi2.Init.Refresh = 0;
  if (HAL_OSPI_Init(&hospi2) != HAL_OK)
  {
    Error_Handler();
  }
  sOspiManagerCfg.ClkPort = 2;
  sOspiManagerCfg.NCSPort = 2;
  sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_2_HIGH;
  if (HAL_OSPIM_Config(&hospi2, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
  {
    Error_Handler();
  }

  /*W25Q128 Init */
  W25Q128_OCTO_SPI_Init(&hospi2);
  // W25Q128_OSPI_EraseSector(&hospi2,0,0x7FFFFF);

  //	W25Q128_OSPI_Read(&hospi2, rxbuf, 0, 100);
  //	printf("here: %s \r\n",rxbuf);
  //	W25Q128_OSPI_Write(&hospi2,txbuf,0,sizeof(txbuf));
  //	W25Q128_OSPI_Read(&hospi2, rxbuf, 0, 100);
  //	printf("here %s \r\n",rxbuf);

  W25Q128_OSPI_EnableMemoryMappedMode(&hospi2);
  HAL_Delay(1000);
}

static uint32_t HAL_RCC_OCTOSPIM_CLK_ENABLED = 0;

void HAL_OSPI_MspInit(OSPI_HandleTypeDef *ospiHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (ospiHandle->Instance == OCTOSPI1)
  {

    //

    //    /* OCTOSPI1 clock enable */
    //    HAL_RCC_OCTOSPIM_CLK_ENABLED++;
    //    if(HAL_RCC_OCTOSPIM_CLK_ENABLED==1){
    //      __HAL_RCC_OCTOSPIM_CLK_ENABLE();
    //    }
    //    __HAL_RCC_OSPI1_CLK_ENABLE();
    //
    //    __HAL_RCC_GPIOD_CLK_ENABLE();
    //    __HAL_RCC_GPIOF_CLK_ENABLE();
    //    __HAL_RCC_GPIOG_CLK_ENABLE();
    //    __HAL_RCC_GPIOE_CLK_ENABLE();
    //    __HAL_RCC_GPIOB_CLK_ENABLE();
    //    /**OCTOSPI1 GPIO Configuration
    //    PD5     ------> OCTOSPIM_P1_IO5
    //    PD4     ------> OCTOSPIM_P1_IO4
    //    PD7     ------> OCTOSPIM_P1_IO7
    //    PF7     ------> OCTOSPIM_P1_IO2
    //    PF6     ------> OCTOSPIM_P1_IO3
    //    PF9     ------> OCTOSPIM_P1_IO1
    //    PG6     ------> OCTOSPIM_P1_NCS
    //    PF10     ------> OCTOSPIM_P1_CLK
    //    PF8     ------> OCTOSPIM_P1_IO0
    //    PE9     ------> OCTOSPIM_P1_IO6
    //    PB2     ------> OCTOSPIM_P1_DQS
    //    */
    //    GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_7;
    //    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    //    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    //    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
    //
    //    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_9|GPIO_PIN_8;
    //    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    //    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    //    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    //
    //    GPIO_InitStruct.Pin = GPIO_PIN_6;
    //    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    //    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    //    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
    //
    //    GPIO_InitStruct.Pin = GPIO_PIN_10;
    //    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    //    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P1;
    //    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    //
    //    GPIO_InitStruct.Pin = GPIO_PIN_9;
    //    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    //    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    //    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
    //
    //    GPIO_InitStruct.Pin = GPIO_PIN_2;
    //    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    //    GPIO_InitStruct.Pull = GPIO_NOPULL;
    //    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    //    GPIO_InitStruct.Alternate = GPIO_AF10_OCTOSPIM_P1;
    //    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else if (ospiHandle->Instance == OCTOSPI2)
  {

    /* OCTOSPI2 clock enable */
    HAL_RCC_OCTOSPIM_CLK_ENABLED++;
    if (HAL_RCC_OCTOSPIM_CLK_ENABLED == 1)
    {
      __HAL_RCC_OCTOSPIM_CLK_ENABLE();
    }
    __HAL_RCC_OSPI2_CLK_ENABLE();

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    /**OCTOSPI2 GPIO Configuration
    PG10     ------> OCTOSPIM_P2_IO6
    PG11     ------> OCTOSPIM_P2_IO7
    PG12     ------> OCTOSPIM_P2_NCS
    PF4     ------> OCTOSPIM_P2_CLK
    PG0     ------> OCTOSPIM_P2_IO4
    PG1     ------> OCTOSPIM_P2_IO5
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_OCTOSPIM_P2;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P2;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_OCTOSPIM_P2;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  }
}

void HAL_OSPI_MspDeInit(OSPI_HandleTypeDef *ospiHandle)
{

  if (ospiHandle->Instance == OCTOSPI1)
  {

    /* Peripheral clock disable */
    HAL_RCC_OCTOSPIM_CLK_ENABLED--;
    if (HAL_RCC_OCTOSPIM_CLK_ENABLED == 0)
    {
      __HAL_RCC_OCTOSPIM_CLK_DISABLE();
    }
    __HAL_RCC_OSPI1_CLK_DISABLE();

    /**OCTOSPI1 GPIO Configuration
    PD5     ------> OCTOSPIM_P1_IO5
    PD4     ------> OCTOSPIM_P1_IO4
    PD7     ------> OCTOSPIM_P1_IO7
    PF7     ------> OCTOSPIM_P1_IO2
    PF6     ------> OCTOSPIM_P1_IO3
    PF9     ------> OCTOSPIM_P1_IO1
    PG6     ------> OCTOSPIM_P1_NCS
    PF10     ------> OCTOSPIM_P1_CLK
    PF8     ------> OCTOSPIM_P1_IO0
    PE9     ------> OCTOSPIM_P1_IO6
    PB2     ------> OCTOSPIM_P1_DQS
    */
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_7);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_7 | GPIO_PIN_6 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_8);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_2);
  }
  else if (ospiHandle->Instance == OCTOSPI2)
  {

    /* Peripheral clock disable */
    HAL_RCC_OCTOSPIM_CLK_ENABLED--;
    if (HAL_RCC_OCTOSPIM_CLK_ENABLED == 0)
    {
      __HAL_RCC_OCTOSPIM_CLK_DISABLE();
    }
    __HAL_RCC_OSPI2_CLK_DISABLE();

    /**OCTOSPI2 GPIO Configuration
    PG10     ------> OCTOSPIM_P2_IO6
    PG11     ------> OCTOSPIM_P2_IO7
    PG12     ------> OCTOSPIM_P2_NCS
    PF4     ------> OCTOSPIM_P2_CLK
    PG0     ------> OCTOSPIM_P2_IO4
    PG1     ------> OCTOSPIM_P2_IO5
    */
    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_0 | GPIO_PIN_1);

    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_4);
  }
}
