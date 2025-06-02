/*
 * wio_lite_ai_conf.h
 *
 *  Created on: Jan 16, 2024
 *      Author: clement.fmy@gmail.com
 */

#ifndef BSP_WIO_LITE_AI_WIO_LITE_AI_CONF_H_
#define BSP_WIO_LITE_AI_WIO_LITE_AI_CONF_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* COM define */
#define USE_COM_LOG                         1U
#define USE_BSP_COM_FEATURE                 1U

/* I2C BUS timing define */
#define I2C_VALID_TIMING_NBR                128U

#define USE_CAMERA_SENSOR_OV2640 1

/* IRQ priorities */
#define BSP_BUTTON_USERx_IT_PRIORITY 14U

#endif /* BSP_WIO_LITE_AI_WIO_LITE_AI_CONF_H_ */
