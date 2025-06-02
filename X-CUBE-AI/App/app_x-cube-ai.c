
/**
 ******************************************************************************
 * @file    app_x-cube-ai.c
 * @author  X-CUBE-AI C code generator
 * @brief   AI program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/*
 * Description
 *   v1.0 - Minimum template to show how to use the Embedded Client API
 *          model. Only one input and one output is supported. All
 *          memory resources are allocated statically (AI_NETWORK_XX, defines
 *          are used).
 *          Re-target of the printf function is out-of-scope.
 *   v2.0 - add multiple IO and/or multiple heap support
 *
 *   For more information, see the embeded documentation:
 *
 *       [1] %X_CUBE_AI_DIR%/Documentation/index.html
 *
 *   X_CUBE_AI_DIR indicates the location where the X-CUBE-AI pack is installed
 *   typical : C:\Users\<user_name>\STM32Cube\Repository\STMicroelectronics\X-CUBE-AI\7.1.0
 */

#ifdef __cplusplus
extern "C"
{
#endif

  /* Includes ------------------------------------------------------------------*/

#if defined(__ICCARM__)
#define AI_CUSTOM _Pragma("location=\"AI_CUSTOM\"")
#elif defined(__CC_ARM) || (__GNUC__)
#define AI_CUSTOM __attribute__((section(".AI_CUSTOM")))
#endif

/* System headers */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "app_x-cube-ai.h"
#include "ai_interface.h"
#include "main.h"
#include "ai_datatypes_defines.h"
#include "network.h"
#include "network_data.h"

/* USER CODE BEGIN includes */
#include "ai_utils.h"
#include "stm32ipl.h"
#include "W25Q128.h"
#include "ov2640.h"
#include "vittascience_i2c.h"
  /* USER CODE END includes */


  /* AI objects ----------------------------------------------------------------*/

  /* USER CODE BEGIN 2 */
  EXTMEM uint8_t buffer_rgb888[320 * 240 * 3] = {0};
  extern uint8_t buffer[320 * 240 * 2];
  extern uint8_t buffer_tmp[320 * 240 * 2];
  extern uint8_t buffer_ipl[IPL_BUFFER_SIZE];
  extern uint8_t buffer_ai[AI_INPUT_H * AI_INPUT_W * 3];
  extern image_t frame_rgb565;
  extern bool do_ai;
  extern char msg[70];
  static float network_result_float[AI_NETWORK_OUT_1_SIZE];

  image_t frame_ai_ready = {
      .w = AI_INPUT_W,
      .h = AI_INPUT_W,
      .bpp = IMAGE_BPP_RGB888,
      .data = buffer_ai};

  image_t frame_rgb888 = {
      .w = 320,
      .h = 240,
      .bpp = IMAGE_BPP_RGB888,
      .data = buffer_rgb888};

  rectangle_t roi = {
      .w = 224,
      .h = 224,
      .x = 48,
      .y = 8};

  int acquire_and_process_data(AppConfig_TypeDef *App_Config_Ptr)
  {
    /*
     * Here we use IPL to do image pre-processing, and then use the pre-processed results as input to the model, and finally do post-processing.
     */

    // rgb565 320*240*2 to rgb888 320*240*3
    if (STM32Ipl_Convert(&frame_rgb565, &frame_rgb888) != stm32ipl_err_Ok)
    {
      while (1)
        ;
    }
    App_Config_Ptr->buffer_tmp_in_use = false;

    // rgb888 320*240*3 to rgb888 224*224*3
    frame_ai_ready.data = App_Config_Ptr->nn_input_buffer;
    if (STM32Ipl_Resize(&frame_rgb888, &frame_ai_ready, &roi) != stm32ipl_err_Ok)
    {
      while (1)
        ;
    }
    return 0;
  }

  int send_i2c_result(AppConfig_TypeDef *App_Config_Ptr)
  {
    uint8_t buffer[VITTASCIENCE_I2C_RESULT_SIZE] = {0};
    if (AI_NET_OUTPUT_SIZE <= VITTASCIENCE_I2C_RESULT_SIZE)
    {
      for (int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
      {
        buffer[i] = (uint8_t)(*((float*)(App_Config_Ptr->nn_output_buffer)+i) * 100);
      }
      //printf("BUFFER %d %d %d %d \r\n", buffer[0], buffer[1], buffer[2], buffer[3]);
      vittascience_i2c_set_results(buffer, AI_NET_OUTPUT_SIZE);
    }
    return 1;
  }

  /**
   * @brief  Performs the dequantization of a quantized NN output
   * @param  App_Config_Ptr pointer to application context
   * @retval None
   */
  static void Output_Dequantize(AppConfig_TypeDef *App_Config_Ptr)
  {
    /**Check format of the output and convert to float if required**/
    if (ai_get_output_format() == AI_BUFFER_FMT_TYPE_Q)
    {
      float scale;
      int32_t zero_point;
      ai_i8 *nn_output_i8;
      ai_u8 *nn_output_u8;
      float *nn_output_f32;

      /*Check what type of quantization scheme is used for the output*/
      switch (ai_get_output_quantization_scheme())
      {
      case AI_FXP_Q:

        scale = ai_get_output_fxp_scale();

        /* Dequantize NN output - in-place 8-bit to float conversion */
        nn_output_i8 = (ai_i8 *)App_Config_Ptr->nn_output_buffer;
        nn_output_f32 = (float *)App_Config_Ptr->nn_output_buffer;
        for (int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
        {
          float q_value = (float)*(nn_output_i8 + i);
          *(nn_output_f32 + i) = scale * q_value;
        }
        break;

      case AI_UINT_Q:

        scale = ai_get_output_scale();
        zero_point = ai_get_output_zero_point();

        /* Dequantize NN output - in-place 8-bit to float conversion */
        nn_output_u8 = (ai_u8 *)App_Config_Ptr->nn_output_buffer;
        nn_output_f32 = (float *)App_Config_Ptr->nn_output_buffer;
        for (int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
        {
          int32_t q_value = (int32_t) * (nn_output_u8 + i);
          *(nn_output_f32 + i) = scale * (q_value - zero_point);
        }
        break;

      case AI_SINT_Q:

        scale = ai_get_output_scale();
        zero_point = ai_get_output_zero_point();

        /* Dequantize NN output - in-place 8-bit to float conversion */
        nn_output_i8 = (ai_i8 *)App_Config_Ptr->nn_output_buffer;
        nn_output_f32 = (float *)App_Config_Ptr->nn_output_buffer;
        for (int32_t i = AI_NET_OUTPUT_SIZE - 1; i >= 0; i--)
        {
          int32_t q_value = (int32_t) * (nn_output_i8 + i);
          *(nn_output_f32 + i) = scale * (q_value - zero_point);
        }
        break;

      default:
        break;
      }
    }
  }

  int post_process(AppConfig_TypeDef *App_Config_Ptr)
  {
    int ranking[NN_OUTPUT_CLASS_NUMBER];

    /**NN ouput dequantization if required**/
    Output_Dequantize(App_Config_Ptr);

    /* Add missing softmax layer to get a normalized probability distribution */
    // AI_Softmax(network_result_float);

    send_i2c_result(App_Config_Ptr);

    /**Perform ranking**/
    for (int i = 0; i < NN_OUTPUT_CLASS_NUMBER; i++)
    {
      App_Config_Ptr->ranking[i] = i;
    }

    Utility_Bubblesort((float*)(App_Config_Ptr->nn_output_buffer), App_Config_Ptr->ranking, NN_OUTPUT_CLASS_NUMBER);

    App_Config_Ptr->Tfps_stop =Utility_GetTimeStamp();
  
    App_Config_Ptr->nn_top1_output_class_name=App_Config_Ptr->nn_output_labels[App_Config_Ptr->ranking[0]];
    App_Config_Ptr->nn_top1_output_class_proba=*((float*)(App_Config_Ptr->nn_output_buffer)+0) * 100;

    sprintf(msg, "%s %.0f%%", App_Config_Ptr->nn_top1_output_class_name, App_Config_Ptr->nn_top1_output_class_proba);
    printf("%s\r\n", msg);
    return 0;
  }
  /* USER CODE END 2 */

  /* Entry points --------------------------------------------------------------*/

  void MX_X_CUBE_AI_Process(AppConfig_TypeDef* App_Config)
  {
    //Wait for new data available
    while (App_Config->new_data_available == false)
    {}
    App_Config->buffer_tmp_in_use = true;
    //BSP_CAMERA_Stop();
    /* 1 - acquire and pre-process input data */
    acquire_and_process_data(App_Config);
    /* 2 - process the data - call inference engine */
    Network_Inference(App_Config);
    /* 3- post-process the predictions */
    post_process(App_Config);
    //BSP_CAMERA_ContinuousStart(buffer);
    App_Config->new_data_available = false;
  }
#ifdef __cplusplus
}
#endif
