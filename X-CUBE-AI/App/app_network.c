/**
 ******************************************************************************
 * @file    app_network.c
 * @author  MDG Application Team
 * @brief   FP VISION AI configuration
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
#include "app_network.h"
#include "app_utility.h"
#include "layers.h"
#include <stdio.h>
#include <string.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief  Initializes the generated C model for a neural network
  * @param  App_Config_Ptr pointer to application context
  * @retval None
  */
void Network_Init(AppConfig_TypeDef *App_Config_Ptr)
{
  void *input_data_ptr, *output_data_ptr;
  
  ai_init((void*)(App_Config_Ptr->activation_buffer), &input_data_ptr, &output_data_ptr);
  
  if(input_data_ptr!= NULL)
    App_Config_Ptr->nn_input_buffer=input_data_ptr;
  
  if(output_data_ptr!= NULL)
    App_Config_Ptr->nn_output_buffer=output_data_ptr;
  
  if (App_Config_Ptr->nn_input_buffer == NULL || App_Config_Ptr->nn_output_buffer == NULL)
  {
    while(1);
  }
}

  void Network_Inference(AppConfig_TypeDef *App_Config_Ptr)
{
  App_Config_Ptr->Tinf_start =Utility_GetTimeStamp();
  
  /***********************************/
  /*********Run NN inference**********/
  /***********************************/
  ai_run((void*)App_Config_Ptr->nn_input_buffer, (void*)App_Config_Ptr->nn_output_buffer);
  
  App_Config_Ptr->Tinf_stop =Utility_GetTimeStamp();
  App_Config_Ptr->nn_inference_time = App_Config_Ptr->Tinf_stop - App_Config_Ptr->Tinf_start;
}
