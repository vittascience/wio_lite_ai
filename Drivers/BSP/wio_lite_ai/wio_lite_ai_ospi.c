/*
 * wio_lite_ai_ospi.c
 *
 *  Created on: Jan 16, 2024
 *      Author: clement.fmy@gmail.com
 */

/**
 * @brief  Initializes the OSPI interface.
 * @param  Instance   OSPI Instance
 * @retval BSP status
 */
/* Includes ------------------------------------------------------------------*/
#include "wio_lite_ai_ospi.h"

/* Exported variables --------------------------------------------------------*/

OSPI_HandleTypeDef hospi_ram[OSPI_RAM_INSTANCES_NUMBER] = {0};
OSPI_RAM_Ctx_t Ospi_Ram_Ctx[OSPI_RAM_INSTANCES_NUMBER] = {{OSPI_ACCESS_NONE,
                                                           BSP_OSPI_RAM_VARIABLE_LATENCY,
                                                           BSP_OSPI_RAM_HYBRID_BURST,
                                                           APS6408_BURST_16_BYTES}};
/* Private constants --------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint32_t HAL_RCC_OCTOSPIM_CLK_ENABLED = 0;

#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
static uint32_t OspiRam_IsMspCbValid[OSPI_RAM_INSTANCES_NUMBER] = {0};
#endif /* USE_HAL_OSPI_REGISTER_CALLBACKS */

/* Private functions ---------------------------------------------------------*/

static void OSPI_RAM_MspInit(OSPI_HandleTypeDef *hospi);
static void OSPI_RAM_MspDeInit(OSPI_HandleTypeDef *hospi);
static void DLYB_OCTOSPI1_Calibration(uint8_t phase);
// TODO static int32_t OSPI1_DLYB_Enable(uint32_t Instance);

int32_t BSP_OSPI_RAM_Init(uint32_t Instance)
{
  MX_OSPI_InitTypeDef ospi_init;
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if the instance is already initialized */
    if (Ospi_Ram_Ctx[Instance].IsInitialized == OSPI_ACCESS_NONE)
    {
#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 0)
      /* Msp OSPI initialization */
      OSPI_RAM_MspInit(&hospi_ram[Instance]);
#else
      /* Register the OSPI MSP Callbacks */
      if (OspiRam_IsMspCbValid[Instance] == 0UL)
      {
        if (BSP_OSPI_RAM_RegisterDefaultMspCallbacks(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_PERIPH_FAILURE;
        }
      }
#endif /* USE_HAL_OSPI_REGISTER_CALLBACKS */

      /* Fill config structure */
      ospi_init.ClockPrescaler = 2;
      ospi_init.MemorySize = 23; /* 64 MBits */
      ospi_init.SampleShifting = HAL_OSPI_SAMPLE_SHIFTING_NONE;

      /* STM32 OSPI interface initialization */
      if (MX_OSPI_RAM_Init(&hospi_ram[Instance], &ospi_init) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
      /* Update current status parameter */
      Ospi_Ram_Ctx[Instance].IsInitialized = OSPI_ACCESS_INDIRECT;
      Ospi_Ram_Ctx[Instance].LatencyType = BSP_OSPI_RAM_FIXED_LATENCY;
      Ospi_Ram_Ctx[Instance].BurstType = BSP_OSPI_RAM_LINEAR_BURST;

    }
  }
  DLYB_OCTOSPI1_Calibration(1);
	HAL_Delay(5);
	BSP_OSPI_RAM_Reset(Instance);
	HAL_Delay(5);

  /* Configure Read Latency and drive Strength */
  if (APS6408_WriteReg(&hospi_ram[Instance], 0x00, 0x08) != HAL_OK)
  {
    ret = BSP_ERROR_PERIPH_FAILURE;
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  De-Initializes the OSPI interface.
 * @param  Instance   OSPI Instance
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_DeInit(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Check if the instance is already initialized */
    if (Ospi_Ram_Ctx[Instance].IsInitialized != OSPI_ACCESS_NONE)
    {
      /* Disable Memory mapped mode */
      if (Ospi_Ram_Ctx[Instance].IsInitialized == OSPI_ACCESS_MMP)
      {
        if (BSP_OSPI_RAM_DisableMemoryMappedMode(Instance) != BSP_ERROR_NONE)
        {
          return BSP_ERROR_COMPONENT_FAILURE;
        }
      }
      /* Set default Ospi_Ram_Ctx values */
      Ospi_Ram_Ctx[Instance].IsInitialized = OSPI_ACCESS_NONE;
      Ospi_Ram_Ctx[Instance].LatencyType = BSP_OSPI_RAM_FIXED_LATENCY;
      Ospi_Ram_Ctx[Instance].BurstType = BSP_OSPI_RAM_LINEAR_BURST;

#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 0)
      OSPI_RAM_MspDeInit(&hospi_ram[Instance]);
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 0) */

      /* Call the DeInit function to reset the driver */
      if (HAL_OSPI_DeInit(&hospi_ram[Instance]) != HAL_OK)
      {
        ret = BSP_ERROR_PERIPH_FAILURE;
      }
    }
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  Initializes the OSPI interface.
 * @param  hospi          OSPI handle
 * @param  Init           OSPI config structure
 * @retval BSP status
 */
__weak HAL_StatusTypeDef MX_OSPI_RAM_Init(OSPI_HandleTypeDef *hospi, MX_OSPI_InitTypeDef *Init)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  OSPIM_CfgTypeDef sOspiManagerCfg = {0};

  /* OctoSPI initialization */
  hospi->Instance = OCTOSPI1;

  hospi->Init.FifoThreshold = 4;
  hospi->Init.DualQuad = HAL_OSPI_DUALQUAD_DISABLE;
  hospi->Init.MemoryType = HAL_OSPI_MEMTYPE_APMEMORY;
  hospi->Init.DeviceSize = Init->MemorySize;
  hospi->Init.ChipSelectHighTime = 1;
  hospi->Init.FreeRunningClock = HAL_OSPI_FREERUNCLK_DISABLE;
  hospi->Init.ClockMode = HAL_OSPI_CLOCK_MODE_0;
  hospi->Init.WrapSize = HAL_OSPI_WRAP_NOT_SUPPORTED;
  hospi->Init.ClockPrescaler = Init->ClockPrescaler;
  hospi->Init.SampleShifting = Init->SampleShifting;
  hospi->Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
  hospi->Init.ChipSelectBoundary = 10;
  hospi->Init.DelayBlockBypass = HAL_OSPI_DELAY_BLOCK_USED;
  hospi->Init.MaxTran = 0;
  hospi->Init.Refresh = 0;

  ret = HAL_OSPI_Init(hospi);

  if (ret == HAL_OK)
  {
    sOspiManagerCfg.ClkPort = 1;
    sOspiManagerCfg.DQSPort = 1;
    sOspiManagerCfg.NCSPort = 1;
    sOspiManagerCfg.IOLowPort = HAL_OSPIM_IOPORT_1_LOW;
    sOspiManagerCfg.IOHighPort = HAL_OSPIM_IOPORT_1_HIGH;
    ret = HAL_OSPIM_Config(hospi, &sOspiManagerCfg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE);
  }
  return ret;
}

#if (USE_HAL_OSPI_REGISTER_CALLBACKS == 1)
/**
 * @brief Default BSP OSPI Msp Callbacks
 * @param Instance      OSPI Instance
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_RegisterDefaultMspCallbacks(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if (HAL_OSPI_RegisterCallback(&hospi_ram[Instance], HAL_OSPI_MSP_INIT_CB_ID, OSPI_RAM_MspInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_OSPI_RegisterCallback(&hospi_ram[Instance],
                                       HAL_OSPI_MSP_DEINIT_CB_ID, OSPI_RAM_MspDeInit) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      OspiRam_IsMspCbValid[Instance] = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief BSP OSPI Msp Callback registering
 * @param Instance     OSPI Instance
 * @param CallBacks    pointer to MspInit/MspDeInit callbacks functions
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_RegisterMspCallbacks(uint32_t Instance, BSP_OSPI_Cb_t *CallBacks)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    /* Register MspInit/MspDeInit Callbacks */
    if (HAL_OSPI_RegisterCallback(&hospi_ram[Instance], HAL_OSPI_MSP_INIT_CB_ID, CallBacks->pMspInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else if (HAL_OSPI_RegisterCallback(&hospi_ram[Instance], HAL_OSPI_MSP_DEINIT_CB_ID,
                                       CallBacks->pMspDeInitCb) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    else
    {
      OspiRam_IsMspCbValid[Instance] = 1U;
    }
  }

  /* Return BSP status */
  return ret;
}
#endif /* (USE_HAL_OSPI_REGISTER_CALLBACKS == 1) */

/**
 * @brief  Reads an amount of data from the OSPI memory.
 * @param  Instance  OSPI instance
 * @param  pData     Pointer to data to be read
 * @param  ReadAddr  Read start address
 * @param  Size      Size of data to read
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_Read(uint32_t Instance, uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (APS6408_Read(&hospi_ram[Instance], pData, ReadAddr, Size, DUMMY_CLOCK_CYCLES_READ, 1) != APS6408_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  Writes an amount of data to the OSPI memory.
 * @param  Instance  OSPI instance
 * @param  pData     Pointer to data to be written
 * @param  WriteAddr Write start address
 * @param  Size      Size of data to write
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_Write(uint32_t Instance, uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (APS6408_Write(&hospi_ram[Instance], pData, WriteAddr, Size, DUMMY_CLOCK_CYCLES_WRITE, 1) != APS6408_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  Configure the OSPI in memory-mapped mode
 * @param  Instance  OSPI instance
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_EnableMemoryMappedMode(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  // TODO
  //   /* OSPI Delay Block enable */
  //   else if (OSPI1_DLYB_Enable(Instance) != BSP_ERROR_NONE)
  //   {
  //     ret = BSP_ERROR_COMPONENT_FAILURE;
  //   }
  else
  {
    //DLYB_OCTOSPI1_Calibration(1);
    //HAL_Delay(5);

    if (APS6408_EnableMemoryMappedMode(&hospi_ram[Instance], DUMMY_CLOCK_CYCLES_READ, DUMMY_CLOCK_CYCLES_WRITE,
                                       0) != APS6408_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  Exit the memory-mapped mode
 * @param  Instance  OSPI instance
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_DisableMemoryMappedMode(uint32_t Instance)
{
  int32_t ret = BSP_ERROR_NONE;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else
  {
    if (Ospi_Ram_Ctx[Instance].IsInitialized != OSPI_ACCESS_MMP)
    {
      ret = BSP_ERROR_OSPI_MMP_UNLOCK_FAILURE;
    }
    /* Abort MMP back to indirect mode */
    else if (HAL_OSPI_Abort(&hospi_ram[Instance]) != HAL_OK)
    {
      ret = BSP_ERROR_PERIPH_FAILURE;
    }
    /* Update OSPI HyperRAM context if all operations are well done */
    else
    {
      Ospi_Ram_Ctx[Instance].IsInitialized = OSPI_ACCESS_INDIRECT;
    }
  }

  /* Return BSP status */
  return ret;
}

void BSP_OSPI_RAM_SetDQS()
{
  uint8_t reg[2] =
      {0x00, 0x00};
  uint8_t regs = 0x08;
  while (reg[0] != regs)
  {
    APS6408_WriteReg(&hospi_ram[0], APS6408_MR0_ADDRESS, regs);
    HAL_Delay(1);
    APS6408_ReadReg(&hospi_ram[0], APS6408_MR0_ADDRESS, reg, 6U);
  }
}

/**
 * @brief  Reset PSRAM
 * @param  Instance  OSPI instance
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_Reset(uint32_t Instance)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (APS6408_Reset(&hospi_ram[Instance]) != APS6408_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  Get RAM ID 3 Bytes:
 *         Vendor ID, Device ID, Device Density
 * @param  Instance  OSPI instance
 * @param  Id Pointer to RAM ID bytes
 * @retval BSP status
 */
int32_t BSP_OSPI_RAM_ReadID(uint32_t Instance, uint8_t *Id)
{
  int32_t ret;

  /* Check if the instance is supported */
  if (Instance >= OSPI_RAM_INSTANCES_NUMBER)
  {
    ret = BSP_ERROR_WRONG_PARAM;
  }
  else if (APS6408_ReadID(&hospi_ram[Instance], Id, 6U) != APS6408_OK)
  {
    ret = BSP_ERROR_COMPONENT_FAILURE;
  }
  else
  {
    ret = BSP_ERROR_NONE;
  }

  /* Return BSP status */
  return ret;
}

/**
 * @brief  Initializes the OSPI MSP.
 * @param  hospi OSPI handle
 * @retval None
 */
static void OSPI_RAM_MspInit(OSPI_HandleTypeDef *hospi)
{
  GPIO_InitTypeDef GPIO_InitStruct;

  /* hospi unused argument(s) compilation warning */
  UNUSED(hospi);

  /* Enable the OctoSPI memory interface clock */
  HAL_RCC_OCTOSPIM_CLK_ENABLED++;
  if (HAL_RCC_OCTOSPIM_CLK_ENABLED == 1)
  {
    __HAL_RCC_OCTOSPIM_CLK_ENABLE();
  }

  OSPI_RAM_CLK_ENABLE();

  /* Reset the OctoSPI memory interface */
  OSPI_RAM_FORCE_RESET();
  OSPI_RAM_RELEASE_RESET();

  /* Enable GPIO clocks */
  OSPI_RAM_CLK_GPIO_CLK_ENABLE();
  OSPI_RAM_DQS_GPIO_CLK_ENABLE();
  OSPI_RAM_CS_GPIO_CLK_ENABLE();
  OSPI_RAM_D0_GPIO_CLK_ENABLE();
  OSPI_RAM_D1_GPIO_CLK_ENABLE();
  OSPI_RAM_D2_GPIO_CLK_ENABLE();
  OSPI_RAM_D3_GPIO_CLK_ENABLE();
  OSPI_RAM_D4_GPIO_CLK_ENABLE();
  OSPI_RAM_D5_GPIO_CLK_ENABLE();
  OSPI_RAM_D6_GPIO_CLK_ENABLE();
  OSPI_RAM_D7_GPIO_CLK_ENABLE();

  /* OctoSPI CS GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_CS_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = OSPI_RAM_CS_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_CS_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI DQS GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_DQS_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_DQS_PIN_AF;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OSPI_RAM_DQS_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI CLK GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_CLK_PIN;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Alternate = OSPI_RAM_CLK_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_CLK_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D0 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D0_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D0_PIN_AF;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(OSPI_RAM_D0_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D1 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D1_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D1_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D1_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D2 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D2_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D2_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D2_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D3 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D3_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D3_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D3_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D4 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D4_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D4_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D4_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D5 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D5_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D5_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D5_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D6 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D6_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D6_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D6_GPIO_PORT, &GPIO_InitStruct);

  /* OctoSPI D7 GPIO pin configuration  */
  GPIO_InitStruct.Pin = OSPI_RAM_D7_PIN;
  GPIO_InitStruct.Alternate = OSPI_RAM_D7_PIN_AF;
  HAL_GPIO_Init(OSPI_RAM_D7_GPIO_PORT, &GPIO_InitStruct);
}

/**
 * @brief  De-Initializes the OSPI MSP.
 * @param  hospi OSPI handle
 * @retval None
 */
static void OSPI_RAM_MspDeInit(OSPI_HandleTypeDef *hospi)
{
  /* hospi unused argument(s) compilation warning */
  UNUSED(hospi);

  /* OctoSPI GPIO pins de-configuration  */
  HAL_GPIO_DeInit(OSPI_RAM_CLK_GPIO_PORT, OSPI_RAM_CLK_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_DQS_GPIO_PORT, OSPI_RAM_DQS_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_CS_GPIO_PORT, OSPI_RAM_CS_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D0_GPIO_PORT, OSPI_RAM_D0_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D1_GPIO_PORT, OSPI_RAM_D1_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D2_GPIO_PORT, OSPI_RAM_D2_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D3_GPIO_PORT, OSPI_RAM_D3_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D4_GPIO_PORT, OSPI_RAM_D4_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D5_GPIO_PORT, OSPI_RAM_D5_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D6_GPIO_PORT, OSPI_RAM_D6_PIN);
  HAL_GPIO_DeInit(OSPI_RAM_D7_GPIO_PORT, OSPI_RAM_D7_PIN);

  /* Reset the OctoSPI memory interface */
  OSPI_RAM_FORCE_RESET();
  OSPI_RAM_RELEASE_RESET();

  /* Peripheral clock disable */
  HAL_RCC_OCTOSPIM_CLK_ENABLED--;
  if (HAL_RCC_OCTOSPIM_CLK_ENABLED == 0)
  {
    __HAL_RCC_OCTOSPIM_CLK_DISABLE();
  }
  /* Disable the OctoSPI memory interface clock */
  OSPI_RAM_CLK_DISABLE();
}

static void DLYB_OCTOSPI1_Calibration(uint8_t phase)
{
  int flag = 0;
  uint32_t LNGG = 0;
  uint32_t LNG[3];
  uint32_t UnitDelayNum = 0;
  uint32_t TimeOut = 0;

  /*校准前置 —— 复位OSPI延迟模块旁路位，设置自由运行时钟*/
  CLEAR_BIT(OCTOSPI1->DCR1, OCTOSPI_DCR1_DLYBYP);
  SET_BIT(OCTOSPI1->DCR1, OCTOSPI_DCR1_FRCK);

  /*配置延迟线长度为1个完整输入时钟周期*/
  DLYB_OCTOSPI1->CR |= 0x03;       // 使能delay block和length sampling
  DLYB_OCTOSPI1->CFGR &= (~(0xf)); // 设置SEL为12，即使能所有的Delay Unit
  DLYB_OCTOSPI1->CFGR |= 12;

  for (uint8_t i = 0; i < 128; i++)
  {
    DLYB_OCTOSPI1->CFGR &= (~(0x7f << 8));        // 清零UNIT
    DLYB_OCTOSPI1->CFGR |= (i << 8);              // 设置UNIT
    while (!((DLYB_OCTOSPI1->CFGR >> 31) & 0x01)) // LNGF被置为1
    {
      TimeOut++;
      if (TimeOut > 0xffff)
      {
        break;
      }
    }
    if (((DLYB_OCTOSPI1->CFGR >> 31) & 0x01)) // LNGF被置为1
    {
      flag = 1;
    }

    if (flag)
    {
      LNGG = (DLYB_OCTOSPI1->CFGR >> 16) & 0xfff;
      LNG[0] = LNGG & 0x7ff;
      LNG[1] = (LNGG >> 10) & 0x01;
      LNG[2] = (LNGG >> 11) & 0x01;
      if ((LNG[0] > 0) && ((LNG[1] == 0) || (LNG[2] == 0))) // 判断Delay Line Length是否合理
      {
        // printf("The Delay Line is set one input clock period\r\n");
        break;
      }
      else
      {
        flag = 0;
      }
    }
    //		else
    //			printf("The Delay Line is set err\r\n");
  }

  /*确定有多少个Unit Delay，跨越一个输入时钟周期*/
  for (int8_t i = 10; i >= 0; i--)
  {
    if ((LNGG >> i) & 0x01)
    {
      UnitDelayNum = i;
      break;
    }
  }

  /*选择输出时钟相位*/
  DLYB_OCTOSPI1->CFGR &= (~(0xf));
  DLYB_OCTOSPI1->CFGR |= phase;

  // 失能Sampler length enable bit
  DLYB_OCTOSPI1->CR &= (~(1 << 1));

  /*失能自由运行时钟*/
  SET_BIT(OCTOSPI1->CR, OCTOSPI_CR_ABORT);
  CLEAR_BIT(OCTOSPI1->DCR1, OCTOSPI_DCR1_FRCK);
}

///**
//  * @brief  This function enables delay block.
//  * @param  Instance  OSPI instance
//  * @retval BSP status
//  */
// static int32_t OSPI1_DLYB_Enable(uint32_t Instance)
//{
//  LL_DLYB_CfgTypeDef dlyb_cfg, dlyb_cfg_test;
//  int32_t ret = BSP_ERROR_NONE;
//
//  UNUSED(Instance);
//  /* Delay block configuration ------------------------------------------------ */
//  if (HAL_OSPI_DLYB_GetClockPeriod(&hospi_ram[Instance], &dlyb_cfg) != HAL_OK)
//  {
//    ret = BSP_ERROR_PERIPH_FAILURE;
//  }
//
//  /* when DTR, PhaseSel is divided by 4 (emperic value) */
//  dlyb_cfg.PhaseSel /= 4UL;
//
//  /* save the present configuration for check */
//  dlyb_cfg_test = dlyb_cfg;
//
//  /* set delay block configuration */
//  if (HAL_OSPI_DLYB_SetConfig(&hospi_ram[Instance], &dlyb_cfg) != HAL_OK)
//  {
//    ret = BSP_ERROR_PERIPH_FAILURE;
//  }
//  /* check the set value */
//  if (HAL_OSPI_DLYB_GetConfig(&hospi_ram[Instance], &dlyb_cfg) != HAL_OK)
//  {
//    ret = BSP_ERROR_PERIPH_FAILURE;
//  }
//  if ((dlyb_cfg.PhaseSel != dlyb_cfg_test.PhaseSel) || (dlyb_cfg.Units != dlyb_cfg_test.Units))
//  {
//    ret = BSP_ERROR_PERIPH_FAILURE;
//  }
//
//  return ret;
//}
