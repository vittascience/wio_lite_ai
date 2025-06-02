/*
 * wio_lite_ai_camera.h
 *
 *  Created on: Jan 18, 2024
 *      Author: clement.fmy@gmail.com
 */

#ifndef BSP_WIO_LITE_AI_WIO_LITE_AI_CAMERA_H_
#define BSP_WIO_LITE_AI_WIO_LITE_AI_CAMERA_H_

/* Includes ------------------------------------------------------------------*/
#include "wio_lite_ai.h"

/* Include Camera component Driver */
#include "../Components/ov2640/ov2640.h"

typedef enum
{
  CAMERA_OK = 0x00,
  CAMERA_ERROR = 0x01,
  CAMERA_TIMEOUT = 0x02,
  CAMERA_NOT_DETECTED = 0x03,
  CAMERA_NOT_SUPPORTED = 0x04

} Camera_StatusTypeDef;

#define RESOLUTION_R160x120 CAMERA_R160x120 /* QQVGA Resolution     */
#define RESOLUTION_R320x240 CAMERA_R320x240 /* QVGA Resolution      */
#define RESOLUTION_R480x272 CAMERA_R480x272 /* 480x272 Resolution   */
#define RESOLUTION_R640x480 CAMERA_R640x480 /* VGA Resolution       */

#define CAMERA_I2C_ADDRESS ((uint16_t)0x60)

uint8_t BSP_CAMERA_Init(uint32_t Resolution);
void BSP_CAMERA_ContinuousStart(uint8_t *buff);
void BSP_CAMERA_SnapshotStart(uint8_t *buff);
void BSP_CAMERA_Suspend(void);
void BSP_CAMERA_Resume(void);
uint8_t BSP_CAMERA_Stop(void);
uint8_t BSP_CAMERA_HwReset();
uint8_t BSP_CAMERA_PwrUp(void);
uint8_t BSP_CAMERA_PwrDown(void);
void BSP_CAMERA_LineEventCallback(void);
void BSP_CAMERA_VsyncEventCallback(void);
void BSP_CAMERA_FrameEventCallback(void);
void BSP_CAMERA_ErrorCallback(void);

/* Camera features functions prototype */
void BSP_CAMERA_ContrastBrightnessConfig(uint32_t contrast_level, uint32_t brightness_level);
void BSP_CAMERA_BlackWhiteConfig(uint32_t Mode);
void BSP_CAMERA_ColorEffectConfig(uint32_t Effect);

/* To be called in DCMI_IRQHandler function */
void BSP_CAMERA_IRQHandler(void);
/* To be called in DMA2_Stream1_IRQHandler function */
void BSP_CAMERA_DMA_IRQHandler(void);

#endif /* BSP_WIO_LITE_AI_WIO_LITE_AI_CAMERA_H_ */
