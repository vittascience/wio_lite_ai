/*
 * wio_lite_ai_camera.c
 *
 *  Created on: Jan 18, 2024
 *      Author: clement.fmy@gmail.com
 */

/* Includes ------------------------------------------------------------------*/
#include "wio_lite_ai_camera.h"
#include "wio_lite_ai_errno.h"

static DCMI_HandleTypeDef hDcmiHandler;
CAMERA_DrvTypeDef *camera_drv;
uint32_t current_resolution;

static void DCMI_MspInit(void);
static uint32_t GetSize(uint32_t resolution);

/**
 * @brief  Initializes the Camera.
 * @param  Camera: Pointer to the Camera configuration structure
 * @retval Camera status
 */
uint8_t BSP_CAMERA_Init(uint32_t Resolution)
{
  DCMI_HandleTypeDef *phdcmi;
  uint8_t ret = CAMERA_ERROR;

  /* Get the DCMI handle structure */
  phdcmi = &hDcmiHandler;

  /* Initialize the IO functionalities */
  BSP_I2C4_Init();

  /* Set up the Camera */
  //  BSP_CAMERA_HwReset();
  //  HAL_Delay(100);
  //  BSP_CAMERA_PwrUp();
  //  HAL_Delay(1000);

  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_RESET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_12, GPIO_PIN_SET);
  HAL_Delay(100);
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
  HAL_Delay(1000);

  /*** Configures the DCMI to interface with the Camera module ***/
  /* DCMI configuration */
  phdcmi->Instance = DCMI;
  phdcmi->Init.SynchroMode = DCMI_SYNCHRO_HARDWARE;
  phdcmi->Init.PCKPolarity = DCMI_PCKPOLARITY_RISING;
  phdcmi->Init.VSPolarity = DCMI_VSPOLARITY_LOW;
  phdcmi->Init.HSPolarity = DCMI_HSPOLARITY_LOW;
  phdcmi->Init.CaptureRate = DCMI_CR_ALL_FRAME;
  phdcmi->Init.ExtendedDataMode = DCMI_EXTEND_DATA_8B;
  phdcmi->Init.JPEGMode = DCMI_JPEG_DISABLE;
  phdcmi->Init.ByteSelectMode = DCMI_BSM_ALL;
  phdcmi->Init.ByteSelectStart = DCMI_OEBS_ODD;
  phdcmi->Init.LineSelectMode = DCMI_LSM_ALL;
  phdcmi->Init.LineSelectStart = DCMI_OELS_ODD;
  if (HAL_DCMI_Init(phdcmi) != HAL_OK)
  {
    return CAMERA_ERROR;
  }

  uint16_t id = ov2640_drv.ReadID(CAMERA_I2C_ADDRESS);
  if (id == OV2640_ID)
  {
    /* Initialize the Camera driver structure */
    camera_drv = &ov2640_drv;

    /* Camera Init */
    camera_drv->Init(CAMERA_I2C_ADDRESS, Resolution);

    /* Return CAMERA_OK status */
    ret = CAMERA_OK;
  }

  current_resolution = Resolution;

  return ret;
}

/**
 * @brief  Starts the Camera capture in continuous mode.
 * @param  buff: pointer to the Camera output buffer
 * @retval None
 */
void BSP_CAMERA_ContinuousStart(uint8_t *buff)
{
  /* Start the Camera capture */
  HAL_DCMI_Start_DMA(&hDcmiHandler, DCMI_MODE_CONTINUOUS, (uint32_t)buff, GetSize(current_resolution));
}

/**
 * @brief  Starts the Camera capture in snapshot mode.
 * @param  buff: pointer to the Camera output buffer
 * @retval None
 */
void BSP_CAMERA_SnapshotStart(uint8_t *buff)
{
  /* Start the Camera capture */
  HAL_DCMI_Start_DMA(&hDcmiHandler, DCMI_MODE_SNAPSHOT, (uint32_t)buff, GetSize(current_resolution));
}

/**
 * @brief  Suspends the Camera capture.
 * @param  None
 * @retval None
 */
void BSP_CAMERA_Suspend(void)
{
  /* Suspend the Camera Capture */
  HAL_DCMI_Suspend(&hDcmiHandler);
}

/**
 * @brief  Resumes the Camera capture.
 * @param  None
 * @retval None
 */
void BSP_CAMERA_Resume(void)
{
  /* Start the Camera Capture */
  HAL_DCMI_Resume(&hDcmiHandler);
}

/**
 * @brief  Stops the Camera capture.
 * @param  None
 * @retval Camera status
 */
uint8_t BSP_CAMERA_Stop(void)
{
  DCMI_HandleTypeDef *phdcmi;

  uint8_t ret = CAMERA_ERROR;

  /* Get the DCMI handle structure */
  phdcmi = &hDcmiHandler;

  if (HAL_DCMI_Stop(phdcmi) == HAL_OK)
  {
    ret = CAMERA_OK;
  }

  return ret;
}

/**
 * @brief  CAMERA hardware reset
 * @retval BSP status
 */
uint8_t BSP_CAMERA_HwReset()
{
  uint8_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpio_init_structure;

  /* Init DCMI PWR_ENABLE Pin */
  /* Enable GPIO clock */
  __HAL_RCC_GPIOH_CLK_ENABLE();

  gpio_init_structure.Pin = GPIO_PIN_12;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOH, &gpio_init_structure);

  /* De-assert the camera POWER_DOWN pin (active low) */
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_RESET);

  HAL_Delay(100); /* POWER_DOWN de-asserted during 100 ms */

  /* Assert the camera POWER_DOWN pin (active low) */
  HAL_GPIO_WritePin(GPIOH, GPIO_PIN_14, GPIO_PIN_SET);
  HAL_Delay(20);
  return ret;
}

/**
 * @brief  CANERA power up
 * @retval None
 */
uint8_t BSP_CAMERA_PwrUp(void)
{
  uint8_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpio_init_structure;

  /* Init DCMI PWR_ENABLE Pin */
  /* Enable GPIO clock */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  gpio_init_structure.Pin = GPIO_PIN_7;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* Assert the camera POWER_DOWN pin (active high) */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
  return ret;
}

/**
 * @brief  CAMERA power down
 * @retval None
 */
uint8_t BSP_CAMERA_PwrDown(void)
{
  uint8_t ret = BSP_ERROR_NONE;
  GPIO_InitTypeDef gpio_init_structure;
  /* Init DCMI PWR_ENABLE Pin */
  /* Enable GPIO clock */
  __HAL_RCC_GPIOE_CLK_ENABLE();

  gpio_init_structure.Pin = GPIO_PIN_7;
  gpio_init_structure.Mode = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull = GPIO_NOPULL;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &gpio_init_structure);

  /* Assert the camera POWER_DOWN pin (active high) */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_14, GPIO_PIN_SET);
  return ret;
}

/**
 * @brief  Configures the Camera contrast and brightness.
 * @param  contrast_level: Contrast level
 *          This parameter can be one of the following values:
 *            @arg  CAMERA_CONTRAST_LEVEL4: for contrast +2
 *            @arg  CAMERA_CONTRAST_LEVEL3: for contrast +1
 *            @arg  CAMERA_CONTRAST_LEVEL2: for contrast  0
 *            @arg  CAMERA_CONTRAST_LEVEL1: for contrast -1
 *            @arg  CAMERA_CONTRAST_LEVEL0: for contrast -2
 * @param  brightness_level: Brightness level
 *          This parameter can be one of the following values:
 *            @arg  CAMERA_BRIGHTNESS_LEVEL4: for brightness +2
 *            @arg  CAMERA_BRIGHTNESS_LEVEL3: for brightness +1
 *            @arg  CAMERA_BRIGHTNESS_LEVEL2: for brightness  0
 *            @arg  CAMERA_BRIGHTNESS_LEVEL1: for brightness -1
 *            @arg  CAMERA_BRIGHTNESS_LEVEL0: for brightness -2
 * @retval None
 */
void BSP_CAMERA_ContrastBrightnessConfig(uint32_t contrast_level, uint32_t brightness_level)
{
  if (camera_drv->Config != NULL)
  {
    camera_drv->Config(CAMERA_I2C_ADDRESS, CAMERA_CONTRAST_BRIGHTNESS, contrast_level, brightness_level);
  }
}

/**
 * @brief  Configures the Camera white balance.
 * @param  Mode: black_white mode
 *          This parameter can be one of the following values:
 *            @arg  CAMERA_BLACK_WHITE_BW
 *            @arg  CAMERA_BLACK_WHITE_NEGATIVE
 *            @arg  CAMERA_BLACK_WHITE_BW_NEGATIVE
 *            @arg  CAMERA_BLACK_WHITE_NORMAL
 * @retval None
 */
void BSP_CAMERA_BlackWhiteConfig(uint32_t Mode)
{
  if (camera_drv->Config != NULL)
  {
    camera_drv->Config(CAMERA_I2C_ADDRESS, CAMERA_BLACK_WHITE, Mode, 0);
  }
}

/**
 * @brief  Configures the Camera color effect.
 * @param  Effect: Color effect
 *          This parameter can be one of the following values:
 *            @arg  CAMERA_COLOR_EFFECT_ANTIQUE
 *            @arg  CAMERA_COLOR_EFFECT_BLUE
 *            @arg  CAMERA_COLOR_EFFECT_GREEN
 *            @arg  CAMERA_COLOR_EFFECT_RED
 * @retval None
 */
void BSP_CAMERA_ColorEffectConfig(uint32_t Effect)
{
  if (camera_drv->Config != NULL)
  {
    camera_drv->Config(CAMERA_I2C_ADDRESS, CAMERA_COLOR_EFFECT, Effect, 0);
  }
}

/**
 * @brief  Handles DCMI interrupt request.
 * @param  None
 * @retval None
 */
void BSP_CAMERA_IRQHandler(void)
{
  HAL_DCMI_IRQHandler(&hDcmiHandler);
}

/**
 * @brief  Handles DMA interrupt request.
 * @param  None
 * @retval None
 */
void BSP_CAMERA_DMA_IRQHandler(void)
{
  HAL_DMA_IRQHandler(hDcmiHandler.DMA_Handle);
}

/**
 * @brief  Get the capture size.
 * @param  current_resolution: the current resolution.
 * @retval cpature size
 */
static uint32_t GetSize(uint32_t resolution)
{
  uint32_t size = 0;

  /* Get capture size */
  switch (resolution)
  {
  case CAMERA_R160x120:
  {
    size = 0x2580;
  }
  break;
  case CAMERA_R320x240:
  {
    size = 0x9600;
  }
  break;
  default:
  {
    break;
  }
  }

  return size;
}

void HAL_DCMI_MspInit(DCMI_HandleTypeDef *dcmiHandle)
{
  static DMA_HandleTypeDef hdma;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (dcmiHandle->Instance == DCMI)
  {
    /* DCMI clock enable */
    __HAL_RCC_DCMI_CLK_ENABLE();

    /* DMA controller clock enable */
    __HAL_RCC_DMA2_CLK_ENABLE();

    __HAL_RCC_GPIOE_CLK_ENABLE();
    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOH_CLK_ENABLE();
    /**DCMI GPIO Configuration
    PE4     ------> DCMI_D4
    PE6     ------> DCMI_D7
    PE5     ------> DCMI_D6
    PG9     ------> DCMI_VSYNC
    PE1     ------> DCMI_D3
    PE0     ------> DCMI_D2
    PD3     ------> DCMI_D5
    PA9     ------> DCMI_D0
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PH10     ------> DCMI_D1
    */
    GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_1 | GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_4 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF13_DCMI;
    HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

    /* DCMI DMA Init */
    /* DCMI Init */
    hdma.Instance = DMA2_Stream1;
    hdma.Init.Request = DMA_REQUEST_DCMI_PSSI;
    hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma.Init.Mode = DMA_CIRCULAR;
    hdma.Init.Priority = DMA_PRIORITY_VERY_HIGH;
    hdma.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma.Init.MemBurst = DMA_MBURST_SINGLE;
    hdma.Init.PeriphBurst = DMA_PBURST_SINGLE;
    HAL_DMA_Init(&hdma);

    __HAL_LINKDMA(dcmiHandle, DMA_Handle, hdma);

    /* DCMI interrupt Init */
    HAL_NVIC_SetPriority(DCMI_PSSI_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DCMI_PSSI_IRQn);

    /* DMA2_Stream1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Stream1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Stream1_IRQn);
  }
}

void HAL_DCMI_MspDeInit(DCMI_HandleTypeDef *dcmiHandle)
{

  if (dcmiHandle->Instance == DCMI)
  {
    /* Peripheral clock disable */
    __HAL_RCC_DCMI_CLK_DISABLE();

    /**DCMI GPIO Configuration
    PE4     ------> DCMI_D4
    PE6     ------> DCMI_D7
    PE5     ------> DCMI_D6
    PG9     ------> DCMI_VSYNC
    PE1     ------> DCMI_D3
    PE0     ------> DCMI_D2
    PD3     ------> DCMI_D5
    PA9     ------> DCMI_D0
    PA4     ------> DCMI_HSYNC
    PA6     ------> DCMI_PIXCLK
    PH10     ------> DCMI_D1
    */
    HAL_GPIO_DeInit(GPIOE, GPIO_PIN_4 | GPIO_PIN_6 | GPIO_PIN_5 | GPIO_PIN_1 | GPIO_PIN_0);

    HAL_GPIO_DeInit(GPIOG, GPIO_PIN_9);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_3);

    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9 | GPIO_PIN_4 | GPIO_PIN_6);

    HAL_GPIO_DeInit(GPIOH, GPIO_PIN_10);

    /* DCMI DMA DeInit */
    HAL_DMA_DeInit(dcmiHandle->DMA_Handle);

    /* DCMI interrupt Deinit */
    HAL_NVIC_DisableIRQ(DCMI_PSSI_IRQn);
  }
}

/**
 * @brief  Line event callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_LineEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  BSP_CAMERA_LineEventCallback();
}

/**
 * @brief  Line Event callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_LineEventCallback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_LineEventCallback could be implemented in the user file
   */
}

/**
 * @brief  VSYNC event callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_VsyncEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  BSP_CAMERA_VsyncEventCallback();
}

/**
 * @brief  VSYNC Event callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_VsyncEventCallback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_VsyncEventCallback could be implemented in the user file
   */
}

/**
 * @brief  Frame event callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi)
{
  BSP_CAMERA_FrameEventCallback();
}

/**
 * @brief  Frame Event callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_FrameEventCallback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_FrameEventCallback could be implemented in the user file
   */
}

/**
 * @brief  Error callback
 * @param  hdcmi: pointer to the DCMI handle
 * @retval None
 */
void HAL_DCMI_ErrorCallback(DCMI_HandleTypeDef *hdcmi)
{
  BSP_CAMERA_ErrorCallback();
}

/**
 * @brief  Error callback.
 * @param  None
 * @retval None
 */
__weak void BSP_CAMERA_ErrorCallback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
            the HAL_DCMI_ErrorCallback could be implemented in the user file
   */
}

// #if (USE_CAMERA_SENSOR_OV2640 == 1)
// /**
//   * @brief  Register Bus IOs if component ID is OK
//   * @retval error status
//   */
// static int32_t OV2640_Probe(uint32_t Resolution, uint32_t PixelFormat)
// {
//   int32_t ret;
//   OV2640_IO_t              IOCtx;
//   uint32_t                  id;
//   static OV9655_Object_t   OV9655Obj;

//   /* Configure the audio driver */
//   IOCtx.Address     = CAMERA_OV9655_ADDRESS;
//   IOCtx.Init        = BSP_I2C4_Init;
//   IOCtx.DeInit      = BSP_I2C4_DeInit;
//   IOCtx.ReadReg     = BSP_I2C4_ReadReg;
//   IOCtx.WriteReg    = BSP_I2C4_WriteReg;
//   IOCtx.GetTick     = BSP_GetTick;

//   if(OV9655_RegisterBusIO (&OV9655Obj, &IOCtx) != OV9655_OK)
//   {
//     ret = BSP_ERROR_COMPONENT_FAILURE;
//   }
//   else if(OV9655_ReadID(&OV9655Obj, &id) != OV9655_OK)
//   {
//     ret = BSP_ERROR_COMPONENT_FAILURE;
//   }
//   else
//   {
//     if((id != OV9655_ID) && (id != OV9655_ID_2))
//     {
//       ret = BSP_ERROR_UNKNOWN_COMPONENT;
//     }
//     else
//     {
//       Camera_Drv = (CAMERA_Drv_t *) &OV9655_CAMERA_Driver;
//       Camera_CompObj = &OV9655Obj;
//       if(Camera_Drv->Init(Camera_CompObj, Resolution, PixelFormat) != OV9655_OK)
//       {
//         ret = BSP_ERROR_COMPONENT_FAILURE;
//       }
//       else if(Camera_Drv->GetCapabilities(Camera_CompObj, CameraCap) != OV9655_OK)
//       {
//         ret = BSP_ERROR_COMPONENT_FAILURE;
//       }
//       else
//       {
//         ret = BSP_ERROR_NONE;
//       }
//     }
//   }

//   return ret;
// }
// #endif
