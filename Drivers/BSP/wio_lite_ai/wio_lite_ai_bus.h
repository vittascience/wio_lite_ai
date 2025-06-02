/*
 * wio_lite_ai_bus.h
 *
 *  Created on: May 20, 2024
 *      Author: clement.fmy@gmail.com
 */

#ifndef BSP_WIO_LITE_AI_WIO_LITE_AI_BUS_H_
#define BSP_WIO_LITE_AI_WIO_LITE_AI_BUS_H_

/* Includes ------------------------------------------------------------------*/
#include "wio_lite_ai_conf.h"
#include "wio_lite_ai_errno.h"

/* Exported types ------------------------------------------------------------*/

#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(I2C_HandleTypeDef *);
  void (* pMspDeInitCb)(I2C_HandleTypeDef *);
}BSP_I2C_Cb_t;
#endif /* (USE_HAL_I2C_REGISTER_CALLBACKS == 1) */

/* Exported constants --------------------------------------------------------*/

/* Definition for I2C4 clock resources */
#define BUS_I2C1                               I2C1
#define BUS_I2C1_CLK_ENABLE()                  __HAL_RCC_I2C1_CLK_ENABLE()
#define BUS_I2C1_CLK_DISABLE()                 __HAL_RCC_I2C1_CLK_DISABLE()
#define BUS_I2C1_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C1_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()
#define BUS_I2C1_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()
#define BUS_I2C1_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOB_CLK_DISABLE()

#define BUS_I2C1_FORCE_RESET()                 __HAL_RCC_I2C1_FORCE_RESET()
#define BUS_I2C1_RELEASE_RESET()               __HAL_RCC_I2C1_RELEASE_RESET()

/* Definition for I2C4 Pins */
#define BUS_I2C1_SCL_PIN                       GPIO_PIN_6
#define BUS_I2C1_SDA_PIN                       GPIO_PIN_7
#define BUS_I2C1_SCL_GPIO_PORT                 GPIOB
#define BUS_I2C1_SDA_GPIO_PORT                 GPIOB
#define BUS_I2C1_SCL_AF                        GPIO_AF4_I2C1
#define BUS_I2C1_SDA_AF                        GPIO_AF4_I2C1

#define BUS_I2C1_EV_IRQn I2C1_EV_IRQn
#define BUS_I2C1_ER_IRQn I2C1_ER_IRQn

#ifndef BUS_I2C1_FREQUENCY
   #define BUS_I2C1_FREQUENCY                  100000U /* Frequency of I2Cn = 100 KHz*/
#endif

/* Definition for I2C4 clock resources */
#define BUS_I2C4                               I2C4
#define BUS_I2C4_CLK_ENABLE()                  __HAL_RCC_I2C4_CLK_ENABLE()
#define BUS_I2C4_CLK_DISABLE()                 __HAL_RCC_I2C4_CLK_DISABLE()
#define BUS_I2C4_SCL_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOF_CLK_ENABLE()
#define BUS_I2C4_SCL_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOF_CLK_DISABLE()
#define BUS_I2C4_SDA_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOF_CLK_ENABLE()
#define BUS_I2C4_SDA_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOF_CLK_DISABLE()

#define BUS_I2C4_FORCE_RESET()                 __HAL_RCC_I2C4_FORCE_RESET()
#define BUS_I2C4_RELEASE_RESET()               __HAL_RCC_I2C4_RELEASE_RESET()

/* Definition for I2C4 Pins */
#define BUS_I2C4_SCL_PIN                       GPIO_PIN_14
#define BUS_I2C4_SDA_PIN                       GPIO_PIN_15
#define BUS_I2C4_SCL_GPIO_PORT                 GPIOF
#define BUS_I2C4_SDA_GPIO_PORT                 GPIOF
#define BUS_I2C4_SCL_AF                        GPIO_AF4_I2C4
#define BUS_I2C4_SDA_AF                        GPIO_AF4_I2C4

#define BUS_I2C4_EV_IRQn I2C4_EV_IRQn
#define BUS_I2C4_ER_IRQn I2C4_ER_IRQn

#ifndef BUS_I2C4_FREQUENCY
   #define BUS_I2C4_FREQUENCY                  100000U /* Frequency of I2Cn = 100 KHz*/
#endif


/* Exported variables --------------------------------------------------------*/

extern I2C_HandleTypeDef hbus_i2c1;
extern I2C_HandleTypeDef hbus_i2c4;

/* Exported functions --------------------------------------------------------*/

int32_t BSP_I2C1_Init(void);
int32_t BSP_I2C1_DeInit(void);
int32_t BSP_I2C1_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C1_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C1_RegisterDefaultMspCallbacks (void);
int32_t BSP_I2C1_RegisterMspCallbacks (BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_I2C1_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

int32_t BSP_I2C4_Init(void);
int32_t BSP_I2C4_DeInit(void);
int32_t BSP_I2C4_WriteReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_ReadReg(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_WriteReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_ReadReg16(uint16_t DevAddr, uint16_t Reg, uint8_t *pData, uint16_t Length);
int32_t BSP_I2C4_IsReady(uint16_t DevAddr, uint32_t Trials);
int32_t BSP_GetTick(void);
#if (USE_HAL_I2C_REGISTER_CALLBACKS == 1)
int32_t BSP_I2C4_RegisterDefaultMspCallbacks (void);
int32_t BSP_I2C4_RegisterMspCallbacks (BSP_I2C_Cb_t *Callback);
#endif /* USE_HAL_I2C_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_I2C4_Init(I2C_HandleTypeDef *hI2c, uint32_t timing);

#endif /* BSP_WIO_LITE_AI_WIO_LITE_AI_BUS_H_ */