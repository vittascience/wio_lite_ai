/*
 * wio_lite_ai_ospi.h
 *
 *  Created on: Jan 16, 2024
 *      Author: clement.fmy@gmail.com
 */

#ifndef BSP_WIO_LITE_AI_WIO_LITE_AI_OSPI_H_
#define BSP_WIO_LITE_AI_WIO_LITE_AI_OSPI_H_

/* Includes ------------------------------------------------------------------*/
#include "wio_lite_ai_conf.h"
#include "wio_lite_ai_errno.h"
#include "../Components/aps6408/aps6408.h"

/* Exported types ------------------------------------------------------------*/

typedef enum
{
  OSPI_ACCESS_NONE = 0, /*!<  Instance not initialized,              */
  OSPI_ACCESS_INDIRECT, /*!<  Instance use indirect mode access      */
  OSPI_ACCESS_MMP       /*!<  Instance use Memory Mapped Mode read   */
} OSPI_Access_t;

#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
typedef struct
{
  pOSPI_CallbackTypeDef pMspInitCb;
  pOSPI_CallbackTypeDef pMspDeInitCb;
} BSP_OSPI_Cb_t;
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 1) */

typedef struct
{
  uint32_t MemorySize;
  uint32_t ClockPrescaler;
  uint32_t SampleShifting;
  uint32_t TransferRate;
} MX_OSPI_InitTypeDef;

#define BSP_OSPI_RAM_BurstLength_t APS6408_BurstLength_t

typedef enum
{
  BSP_OSPI_RAM_VARIABLE_LATENCY = HAL_OSPI_VARIABLE_LATENCY,
  BSP_OSPI_RAM_FIXED_LATENCY = HAL_OSPI_FIXED_LATENCY
} BSP_OSPI_RAM_Latency_t;

typedef enum
{
  BSP_OSPI_RAM_HYBRID_BURST = 0,
  BSP_OSPI_RAM_LINEAR_BURST
} BSP_OSPI_RAM_BurstType_t;

typedef struct
{
  OSPI_Access_t IsInitialized;            /*!< Instance access Flash method */
  BSP_OSPI_RAM_Latency_t LatencyType;     /*!< Latency Type of Instance     */
  BSP_OSPI_RAM_BurstType_t BurstType;     /*!< Burst Type of Instance       */
  BSP_OSPI_RAM_BurstLength_t BurstLength; /*!< Burst Length of Instance     */
} OSPI_RAM_Ctx_t;

typedef struct
{
  BSP_OSPI_RAM_Latency_t LatencyType;     /*!< Current RAM Latency Type */
  BSP_OSPI_RAM_BurstType_t BurstType;     /*!< Current RAM Burst Type   */
  BSP_OSPI_RAM_BurstLength_t BurstLength; /*!< Current RAM Burst Length */
} BSP_OSPI_RAM_Init_t;

/* Exported constants --------------------------------------------------------*/

/* Definition for OSPI RAM clock resources */
#define OSPI_RAM_CLK_ENABLE() __HAL_RCC_OSPI1_CLK_ENABLE()
#define OSPI_RAM_CLK_DISABLE() __HAL_RCC_OSPI1_CLK_DISABLE()

#define OSPI_RAM_CLK_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define OSPI_RAM_DQS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define OSPI_RAM_CS_GPIO_CLK_ENABLE() __HAL_RCC_GPIOG_CLK_ENABLE()
#define OSPI_RAM_D0_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define OSPI_RAM_D1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define OSPI_RAM_D2_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define OSPI_RAM_D3_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define OSPI_RAM_D4_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define OSPI_RAM_D5_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define OSPI_RAM_D6_GPIO_CLK_ENABLE() __HAL_RCC_GPIOE_CLK_ENABLE()
#define OSPI_RAM_D7_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()

#define OSPI_RAM_FORCE_RESET() __HAL_RCC_OSPI1_FORCE_RESET()
#define OSPI_RAM_RELEASE_RESET() __HAL_RCC_OSPI1_RELEASE_RESET()

/* Definition for OSPI RAM Pins */
/* OSPI_CLK */
#define OSPI_RAM_CLK_PIN GPIO_PIN_10
#define OSPI_RAM_CLK_GPIO_PORT GPIOF
#define OSPI_RAM_CLK_PIN_AF GPIO_AF9_OCTOSPIM_P1
/* OSPI_DQS */
#define OSPI_RAM_DQS_PIN GPIO_PIN_2
#define OSPI_RAM_DQS_GPIO_PORT GPIOB
#define OSPI_RAM_DQS_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_CS */
#define OSPI_RAM_CS_PIN GPIO_PIN_6
#define OSPI_RAM_CS_GPIO_PORT GPIOG
#define OSPI_RAM_CS_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D0 */
#define OSPI_RAM_D0_PIN GPIO_PIN_8
#define OSPI_RAM_D0_GPIO_PORT GPIOF
#define OSPI_RAM_D0_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D1 */
#define OSPI_RAM_D1_PIN GPIO_PIN_9
#define OSPI_RAM_D1_GPIO_PORT GPIOF
#define OSPI_RAM_D1_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D2 */
#define OSPI_RAM_D2_PIN GPIO_PIN_7
#define OSPI_RAM_D2_GPIO_PORT GPIOF
#define OSPI_RAM_D2_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D3 */
#define OSPI_RAM_D3_PIN GPIO_PIN_6
#define OSPI_RAM_D3_GPIO_PORT GPIOF
#define OSPI_RAM_D3_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D4 */
#define OSPI_RAM_D4_PIN GPIO_PIN_4
#define OSPI_RAM_D4_GPIO_PORT GPIOD
#define OSPI_RAM_D4_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D5 */
#define OSPI_RAM_D5_PIN GPIO_PIN_5
#define OSPI_RAM_D5_GPIO_PORT GPIOD
#define OSPI_RAM_D5_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D6 */
#define OSPI_RAM_D6_PIN GPIO_PIN_9
#define OSPI_RAM_D6_GPIO_PORT GPIOE
#define OSPI_RAM_D6_PIN_AF GPIO_AF10_OCTOSPIM_P1
/* OSPI_D7 */
#define OSPI_RAM_D7_PIN GPIO_PIN_7
#define OSPI_RAM_D7_GPIO_PORT GPIOD
#define OSPI_RAM_D7_PIN_AF GPIO_AF10_OCTOSPIM_P1

#define OSPI_RAM_INSTANCES_NUMBER 1U

/* OSPI Burst length */
#define BSP_OSPI_RAM_BURST_16_BYTES (BSP_OSPI_RAM_BurstLength_t) APS6408_BURST_16_BYTES
#define BSP_OSPI_RAM_BURST_32_BYTES (BSP_OSPI_RAM_BurstLength_t) APS6408_BURST_32_BYTES
#define BSP_OSPI_RAM_BURST_64_BYTES (BSP_OSPI_RAM_BurstLength_t) APS6408_BURST_64_BYTES
#define BSP_OSPI_RAM_BURST_128_BYTES (BSP_OSPI_RAM_BurstLength_t) APS6408_BURST_128_BYTES

/* Exported variables --------------------------------------------------------*/

extern OSPI_HandleTypeDef hospi_ram[OSPI_RAM_INSTANCES_NUMBER];
extern OSPI_RAM_Ctx_t Ospi_Ram_Ctx[OSPI_RAM_INSTANCES_NUMBER];

/* Exported functions --------------------------------------------------------*/

int32_t BSP_OSPI_RAM_Init(uint32_t Instance);
int32_t BSP_OSPI_RAM_DeInit(uint32_t Instance);
#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
int32_t BSP_OSPI_RAM_RegisterMspCallbacks(uint32_t Instance, BSP_OSPI_Cb_t *CallBacks);
int32_t BSP_OSPI_RAM_RegisterDefaultMspCallbacks(uint32_t Instance);
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 1) */
int32_t BSP_OSPI_RAM_Read(uint32_t Instance, uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
int32_t BSP_OSPI_RAM_Write(uint32_t Instance, uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
int32_t BSP_OSPI_RAM_EnableMemoryMappedMode(uint32_t Instance);
int32_t BSP_OSPI_RAM_DisableMemoryMappedMode(uint32_t Instance);
void BSP_OSPI_RAM_SetDQS();
int32_t BSP_OSPI_RAM_Reset(uint32_t Instance);
int32_t BSP_OSPI_RAM_ReadID(uint32_t Instance, uint8_t *Id);

HAL_StatusTypeDef MX_OSPI_RAM_Init(OSPI_HandleTypeDef *hospi, MX_OSPI_InitTypeDef *Init);

#endif /* BSP_WIO_LITE_AI_WIO_LITE_AI_OSPI_H_ */
