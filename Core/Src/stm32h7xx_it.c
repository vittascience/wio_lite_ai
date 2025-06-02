/**
 ******************************************************************************
 * @file    stm32h7xx_it.c
 * @brief   Interrupt Service Routines.
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
#include "main.h"
#include "stm32h7xx_it.h"
/* Private includes ----------------------------------------------------------*/
#include "wio_lite_ai_lcd.h"
#include "i2c.h"

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private user code ---------------------------------------------------------*/

/* External variables --------------------------------------------------------*/

extern I2C_HandleTypeDef hbus_i2c1;
extern I2C_HandleTypeDef hbus_i2c4;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;

/******************************************************************************/
/*           Cortex Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{

  while (1)
  {
  }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{

  printf("HardfaultHandler\r\n");
  NVIC_SystemReset();

  while (1)
  {
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{

  printf("MemManage_Handler\r\n");

  while (1)
  {
  }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void)
{

  printf("BusFault_Handler\r\n");

  while (1)
  {
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{

  printf("UsageFault_Handler\r\n");

  while (1)
  {
  }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{

  HAL_IncTick();
}

/******************************************************************************/
/* STM32H7xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32h7xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA2 stream1 global interrupt.
 */
void DMA2_Stream1_IRQHandler(void)
{
  BSP_CAMERA_DMA_IRQHandler();
}

/**
 * @brief This function handles DCMI and PSSI global interrupt.
 */
void DCMI_PSSI_IRQHandler(void)
{
  BSP_CAMERA_IRQHandler();
}

/**
 * @brief  This function handles EXTI0_IRQ Handler.
 * @param  None
 * @retval None
 */
void EXTI1_IRQHandler(void)
{
  HAL_EXTI_IRQHandler(&hpb_exti[BUTTON_USER1]);
}

/**
  * @brief This function handles I2C1 event interrupt.
  */
void I2C1_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hbus_i2c1);
}

/**
  * @brief This function handles I2C1 error interrupt.
  */
void I2C1_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hbus_i2c1);
}

/**
 * @brief  This function handles I2C event interrupt request.
 * @param  None
 * @retval None
 * @Note   This function is redefined in "main.h" and related to I2C data transmission
 */
void I2C4_EV_IRQHandler(void)
{
  HAL_I2C_EV_IRQHandler(&hbus_i2c4);
}

/**
 * @brief  This function handles I2C error interrupt request.
 * @param  None
 * @retval None
 * @Note   This function is redefined in "main.h" and related to I2C error
 */
void I2C4_ER_IRQHandler(void)
{
  HAL_I2C_ER_IRQHandler(&hbus_i2c4);
}

/**
  * @brief This function handles DMA1 stream0 global interrupt.
  */
void DMA1_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream0_IRQn 0 */

  /* USER CODE END DMA1_Stream0_IRQn 0 */
  #ifdef BSP_CONFIG_SCREEN_SPI
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  #endif
  /* USER CODE BEGIN DMA1_Stream0_IRQn 1 */

  /* USER CODE END DMA1_Stream0_IRQn 1 */
}

/**
  * @brief This function handles SPI1 global interrupt.
  */
void SPI1_IRQHandler(void)
{
  /* USER CODE BEGIN SPI1_IRQn 0 */

  /* USER CODE END SPI1_IRQn 0 */
  #ifdef BSP_CONFIG_SCREEN_SPI
  HAL_SPI_IRQHandler(&hspi1);
  #endif
  /* USER CODE BEGIN SPI1_IRQn 1 */

  /* USER CODE END SPI1_IRQn 1 */
}

/**
  * @brief  This function handles DMA2D Handler.
  * @param  None
  * @retval None
  */
void DMA2D_IRQHandler(void)
{
  #ifdef BSP_CONFIG_SCREEN_SPI
  HAL_DMA2D_IRQHandler(&hlcd_dma2d);
  #endif
}
