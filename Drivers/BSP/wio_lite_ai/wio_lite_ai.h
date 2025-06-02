/*
 * wio_lite_ai.h
 *
 *  Created on: Jan 18, 2024
 *      Author: clement.fmy@gmail.com
 */

#ifndef BSP_WIO_LITE_AI_WIO_LITE_AI_H_
#define BSP_WIO_LITE_AI_WIO_LITE_AI_H_

/* Includes ------------------------------------------------------------------*/
#include "wio_lite_ai_conf.h"
#include "wio_lite_ai_errno.h"
#include "wio_lite_ai_bus.h"
#include "wio_lite_ai_camera.h"
#include "wio_lite_ai_ospi.h"
//#include "wio_lite_ai_lcd.h"

#if (USE_BSP_COM_FEATURE > 0)
  #if (USE_COM_LOG > 0)
    #ifndef __GNUC__
      #include "stdio.h"
    #endif
  #endif
#endif

typedef enum
{
  LED0 = 0,
  LED1 = 1,
  LEDn,

  /* Color led aliases */
  LED_RED = LED0,
  LED_YELLOW = LED1,
} Led_TypeDef;

typedef enum
{
  BUTTON_USER1 = 0,
  BUTTONn
} Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

#if (USE_BSP_COM_FEATURE > 0)
typedef enum
{
  COM1 = 0U,
  COMn
}COM_TypeDef;

typedef enum
{
  COM_STOPBITS_1     =   UART_STOPBITS_1,
  COM_STOPBITS_2     =   UART_STOPBITS_2,
}COM_StopBitsTypeDef;

typedef enum
{
  COM_PARITY_NONE     =  UART_PARITY_NONE,
  COM_PARITY_EVEN     =  UART_PARITY_EVEN,
  COM_PARITY_ODD      =  UART_PARITY_ODD,
}COM_ParityTypeDef;

typedef enum
{
  COM_HWCONTROL_NONE    =  UART_HWCONTROL_NONE,
  COM_HWCONTROL_RTS     =  UART_HWCONTROL_RTS,
  COM_HWCONTROL_CTS     =  UART_HWCONTROL_CTS,
  COM_HWCONTROL_RTS_CTS =  UART_HWCONTROL_RTS_CTS,
}COM_HwFlowCtlTypeDef;

typedef enum
{
  COM_WORDLENGTH_7B = UART_WORDLENGTH_7B,
  COM_WORDLENGTH_8B = UART_WORDLENGTH_8B,
  COM_WORDLENGTH_9B = UART_WORDLENGTH_9B,
}COM_WordLengthTypeDef;

typedef struct
{
  uint32_t              BaudRate;
  COM_WordLengthTypeDef WordLength;
  COM_StopBitsTypeDef   StopBits;
  COM_ParityTypeDef     Parity;
  COM_HwFlowCtlTypeDef  HwFlowCtl;
}COM_InitTypeDef;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
typedef struct
{
  void (* pMspInitCb)(UART_HandleTypeDef *);
  void (* pMspDeInitCb)(UART_HandleTypeDef *);
}BSP_COM_Cb_t;
#endif /* (USE_HAL_UART_REGISTER_CALLBACKS == 1) */
#endif

/**
 * @brief B_WIO_LITE_AI BSP Driver version number
 */
#define __B_WIO_LITE_AI_BSP_VERSION_MAIN (0x01U) /*!< [31:24] main version */
#define __B_WIO_LITE_AI_BSP_VERSION_SUB1 (0x00U) /*!< [23:16] sub1 version */
#define __B_WIO_LITE_AI_BSP_VERSION_SUB2 (0x00U) /*!< [15:8]  sub2 version */
#define __B_WIO_LITE_AI_BSP_VERSION_RC (0x00U)   /*!< [7:0]  release candidate */
#define __B_WIO_LITE_AI_BSP_VERSION ((__B_WIO_LITE_AI_BSP_VERSION_MAIN << 24U) | (__B_WIO_LITE_AI_BSP_VERSION_SUB1 << 16U) | (__B_WIO_LITE_AI_BSP_VERSION_SUB2 << 8U) | (__B_WIO_LITE_AI_BSP_VERSION_RC))

#define LED0_PIN GPIO_PIN_13
#define LED0_GPIO_PORT GPIOC
#define LED0_GPIO_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE()
#define LED0_GPIO_CLK_DISABLE() __HAL_RCC_GPIOC_CLK_DISABLE()

#define LED1_PIN GPIO_PIN_0
#define LED1_GPIO_PORT GPIOF
#define LED1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define LED1_GPIO_CLK_DISABLE() __HAL_RCC_GPIOF_CLK_DISABLE()

/**
 * @brief Key push-buttons
 */
#define BUTTON_USER1_PIN GPIO_PIN_1
#define BUTTON_USER1_GPIO_PORT GPIOF
#define BUTTON_USER1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define BUTTON_USER1_GPIO_CLK_DISABLE() __HAL_RCC_GPIOF_CLK_DISABLE()
#define BUTTON_USER1_EXTI_LINE EXTI_LINE_1
#define BUTTON_USER1_EXTI_IRQn EXTI1_IRQn

#define H_EXTI_0 hpb_exti[BUTTON_USER1]

#define BUTTON_USERx_GPIO_CLK_ENABLE(__INDEX__) \
  do                                            \
  {                                             \
    if ((__INDEX__) == BUTTON_USER1)            \
      BUTTON_USER1_GPIO_CLK_ENABLE();           \
  } while (0)

#define BUTTON_USERx_GPIO_CLK_DISABLE(__INDEX__) \
  do                                             \
  {                                              \
    if ((__INDEX__) == BUTTON_USER1)             \
      BUTTON_USER1_GPIO_CLK_DISABLE();           \
  } while (0)

#if (USE_BSP_COM_FEATURE > 0)
/**
 * @brief Definition for COM port1, connected to USART3
 */
#define COM1_UART                      USART3
#define COM1_CLK_ENABLE()             __HAL_RCC_USART3_CLK_ENABLE()
#define COM1_CLK_DISABLE()            __HAL_RCC_USART3_CLK_DISABLE()

#define COM1_TX_PIN                   GPIO_PIN_8
#define COM1_TX_GPIO_PORT             GPIOD
#define COM1_TX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define COM1_TX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define COM1_TX_AF                    GPIO_AF7_USART3

#define COM1_RX_PIN                   GPIO_PIN_9
#define COM1_RX_GPIO_PORT             GPIOD
#define COM1_RX_GPIO_CLK_ENABLE()     __HAL_RCC_GPIOD_CLK_ENABLE()
#define COM1_RX_GPIO_CLK_DISABLE()    __HAL_RCC_GPIOD_CLK_DISABLE()
#define COM1_RX_AF                    GPIO_AF7_USART3
#define COM_POLL_TIMEOUT              1000

#define MX_UART_InitTypeDef COM_InitTypeDef
#endif

extern EXTI_HandleTypeDef hpb_exti[];
#if (USE_BSP_COM_FEATURE > 0)
extern UART_HandleTypeDef hcom_uart[COMn];
extern USART_TypeDef* COM_USART[COMn];
#endif

int32_t BSP_GetVersion(void);
const uint8_t *BSP_GetBoardName(void);
const uint8_t *BSP_GetBoardID(void);

int32_t BSP_LED_Init(Led_TypeDef Led);
int32_t BSP_LED_DeInit(Led_TypeDef Led);
int32_t BSP_LED_On(Led_TypeDef Led);
int32_t BSP_LED_Off(Led_TypeDef Led);
int32_t BSP_LED_Toggle(Led_TypeDef Led);
int32_t BSP_LED_GetState(Led_TypeDef Led);

int32_t BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t BSP_PB_DeInit(Button_TypeDef Button);
int32_t BSP_PB_GetState(Button_TypeDef Button);
void BSP_PB_Callback(Button_TypeDef Button);
void BSP_PB_IRQHandler(Button_TypeDef Button);
#if (USE_BSP_COM_FEATURE > 0)
int32_t  BSP_COM_Init(COM_TypeDef COM, COM_InitTypeDef *COM_Init);
int32_t  BSP_COM_DeInit(COM_TypeDef COM);
#if (USE_COM_LOG > 0)
int32_t  BSP_COM_SelectLogPort (COM_TypeDef COM);
#endif

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
int32_t BSP_COM_RegisterDefaultMspCallbacks(COM_TypeDef COM);
int32_t BSP_COM_RegisterMspCallbacks(COM_TypeDef COM, BSP_COM_Cb_t *Callback);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
HAL_StatusTypeDef MX_USART3_Init(UART_HandleTypeDef *huart, MX_UART_InitTypeDef *COM_Init);
#endif/* (USE_BSP_COM_FEATURE > 0)  */

#if defined(HAL_I2C_MODULE_ENABLED)
/*##################### I2C4 ###################################*/
/* User can use this section to tailor I2C4 instance used and associated
   resources */
/* Definition for I2C2 Pins */
#define WIO_LITE_AI_CAMERA_I2C_SCL_PIN GPIO_PIN_14
#define WIO_LITE_AI_CAMERA_I2C_SCL_GPIO_PORT GPIOF
#define WIO_LITE_AI_CAMERA_I2C_SDA_PIN GPIO_PIN_15
#define WIO_LITE_AI_CAMERA_I2C_SDA_GPIO_PORT GPIOF
#define WIO_LITE_AI_CAMERA_I2C_SCL_SDA_AF GPIO_AF4_I2C4
/* Definition for I2C2 clock resources */
#define WIO_LITE_AI_CAMERA_I2C I2C4
#define WIO_LITE_AI_CAMERA_I2C_CLK_ENABLE() __HAL_RCC_I2C4_CLK_ENABLE()
#define WIO_LITE_AI_CAMERA_I2C_CLK_DISABLE() __HAL_RCC_I2C4_CLK_DISABLE()
#define WIO_LITE_AI_CAMERA_I2C_SDA_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define WIO_LITE_AI_CAMERA_I2C_SCL_GPIO_CLK_ENABLE() __HAL_RCC_GPIOF_CLK_ENABLE()
#define WIO_LITE_AI_CAMERA_I2C_SDA_GPIO_CLK_DISABLE() __HAL_RCC_GPIOF_CLK_DISABLE()
#define WIO_LITE_AI_CAMERA_I2C_SCL_GPIO_CLK_DISABLE() __HAL_RCC_GPIOF_CLK_DISABLE()
#define WIO_LITE_AI_CAMERA_I2C_FORCE_RESET() __HAL_RCC_I2C4_FORCE_RESET()
#define WIO_LITE_AI_CAMERA_I2C_RELEASE_RESET() __HAL_RCC_I2C4_RELEASE_RESET()

/* Definition for I2C2's NVIC */
#define WIO_LITE_AI_CAMERA_I2C_EV_IRQn I2C4_EV_IRQn
#define WIO_LITE_AI_CAMERA_I2C_ER_IRQn I2C4_ER_IRQn

/* I2C TIMING Register define when I2C clock source is SYSCLK */
/* I2C TIMING is calculated in case of the I2C Clock source is the SYSCLK = 80 MHz */
/* Set 0x90112626 value to reach 100 KHz speed (Rise time = 640ns, Fall time = 20ns) */
#ifndef WIO_LITE_AI_CAMERA_I2C_TIMING
#define WIO_LITE_AI_CAMERA_I2C_TIMING 0x90112626
#endif /* WIO_LITE_AI_CAMERA_I2C_TIMING */

/* I2C clock speed configuration (in Hz)
   WARNING:
   Make sure that this define is not already declared in other files (ie.
   stm324xg_discovery.h file). It can be used in parallel by other modules. */
#ifndef BSP_I2C_SPEED
#define BSP_I2C_SPEED 100000
#endif /* BSP_I2C_SPEED */

/* Maximum Timeout values for flags waiting loops. These timeouts are not based
   on accurate values, they just guarantee that the application will not remain
   stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */
#define WIO_LITE_AI_CAMERA_I2C_TIMEOUT_MAX 3000
#endif /* HAL_I2C_MODULE_ENABLED */

#endif /* BSP_WIO_LITE_AI_WIO_LITE_AI_H_ */
