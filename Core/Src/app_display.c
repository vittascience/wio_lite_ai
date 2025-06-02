/**
 ******************************************************************************
 * @file    app_display.c
 * @author  MDG Application Team
 * @brief   Library to manage LCD display through DMA2D
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
#include "app_display.h"
#include "app_utility.h"
#include "wio_lite_ai.h"
#include "wio_lite_ai_lcd.h"
#include "lcd.h"
#include "stm32_lcd.h"
#include "wio_lite_ai_display_spi.h"
#include <assert.h>


/* Private defines -----------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void Display_CameraCaptureBuffer(AppConfig_TypeDef *, uint16_t *);
static void Display_Refresh(AppConfig_TypeDef *, bool DoInPlaceConversion);

/* Functions Definition ------------------------------------------------------*/

/**
 * @brief  Display Initialization
 * @param App_Config_Ptr pointer to application context
 */
 void Display_Init(AppConfig_TypeDef *App_Config_Ptr)
{
  BSP_LCD_LayerConfig_t config;
  int ret;

  /* Initialize LCD software component */
  config.X0          = 0;
  config.X1          = LCD_DEFAULT_WIDTH;
  config.Y0          = 0;
  config.Y1          = LCD_DEFAULT_HEIGHT;
  config.PixelFormat = LTDC_PIXEL_FORMAT_RGB565;
  config.Address     = (uint32_t) App_Config_Ptr->lcd_frame_buff;
  LcdLayerCfg[0] = config;

  ret = BSP_DISPLAY_SPI_Init();
  assert(ret == BSP_ERROR_NONE);
  
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  UTIL_LCD_SetLayer(0);
  
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetFont(&Font12);
}

/**
 * @brief Displays a Welcome screen
 *        with information about the memory and camera configuration.
 *
 * @param App_Config_Ptr pointer to application context
 */
void Display_WelcomeScreen(AppConfig_TypeDef *App_Config_Ptr)
{
  /* Send the welcome screen many times to ensure the camera application on PC display it */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  
      
  /*Display welcome message*/
  UTIL_LCD_DisplayStringAt(0, LINE(1), (uint8_t *)"IMAGE CLASSIFICATION", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, LINE(2), (uint8_t *)"VITTASCIENCE WIO LITE AI", CENTER_MODE);

  Display_Refresh(App_Config_Ptr, true);
  
  HAL_Delay(4000);
  
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
}

/**
 * @brief Display camera preview on LCD dispaly
 *
 * @param App_Config_Ptr pointer to application context
 */
void Display_CameraPreview(AppConfig_TypeDef *App_Config_Ptr)
{
#if ASPECT_RATIO_MODE == ASPECT_RATIO_PADDING
  uint8_t *camera_capture_buffer = App_Config_Ptr->camera_capture_buffer_no_borders;
#else
  uint8_t *camera_capture_buffer = App_Config_Ptr->camera_capture_buffer;
#endif

  /*Coherency purpose: invalidate the camera_capture_buffer area in L1 D-Cache before CPU reading*/
  Utility_DCache_Coherency_Maintenance((void*)App_Config_Ptr->camera_capture_buffer, 
                                     CAM_FRAME_BUFFER_SIZE + 32 - (CAM_FRAME_BUFFER_SIZE%32), INVALIDATE);
  
  /*Clear LCD display*/
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  
  /* Copy and upscale from camera frame buffer to LCD write buffer */
  Display_CameraCaptureBuffer(App_Config_Ptr, (uint16_t *)camera_capture_buffer); 
}

/**
* @brief Display Neural Network output classification results as well as other performances informations
*
* @param App_Config_Ptr pointer to application context
*/
void Display_NetworkOutput(AppConfig_TypeDef *App_Config_Ptr)
{
  char msg[70];
  
  sprintf(msg, "%s %.0f%%", App_Config.nn_top1_output_class_name, App_Config.nn_top1_output_class_proba);  UTIL_LCD_DisplayStringAt(0, LINE(1), (uint8_t *)msg, LEFT_MODE);
  
  sprintf(msg, "Inference: %ldms", App_Config_Ptr->nn_inference_time);
  UTIL_LCD_DisplayStringAt(0, LINE(2), (uint8_t *)msg, LEFT_MODE);
  
  Display_Refresh(App_Config_Ptr, true);
  
  BSP_LED_Toggle(LED_YELLOW);
}

/**
 * @brief Upscale and display image to LCD write buffer (centered)
 * 
 * @param App_Config_Ptr pointer to application context
 */
static void Display_CameraCaptureBuffer(AppConfig_TypeDef *App_Config_Ptr, uint16_t* cam_buffer)
{
  uint16_t *lcd_buffer = (uint16_t *) App_Config_Ptr->lcd_frame_buff;
  int rowlcd = 0;
  int collcd = (LCD_RES_WIDTH-CAM_RES_WIDTH)/2;

  /* Upscale to VGA, centered for display */
  for (int row = 0; row < CAM_RES_HEIGHT; row++)
  {
    for (int col = 0; col < CAM_RES_WIDTH; col++)
    {
      uint16_t pixel = *cam_buffer++;
      lcd_buffer[rowlcd * LCD_RES_WIDTH + collcd] = pixel;

      collcd += 1;
    }
    collcd = (LCD_RES_WIDTH-CAM_RES_WIDTH)/2;
    rowlcd += 1;
  }
}

/**
 * @brief Refreshes LCD screen
 *        by performing a DMA transfer from lcd write buffer to lcd read buffer
 *
 * @param App_Config_Ptr pointer to application context
 * @param DoInPlaceConversion boolean to know if a conversion may be needed (USB display)
 */
static void Display_Refresh(AppConfig_TypeDef *App_Config_Ptr, bool DoInPlaceConversion)
{
  BSP_DISPLAY_SPI_DrawImage((const char *) App_Config_Ptr->lcd_frame_buff);
}
