/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "crc.h"
#include "dma.h"
#include "ltdc.h"
#include "octospi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "app_x-cube-ai.h"

/* Private includes ----------------------------------------------------------*/
#include "wio_lite_ai.h"
#include "W25Q128.h"
#include "vittascience_i2c.h"
#include "stm32ipl.h"
#include "stdbool.h"

#ifdef BSP_CONFIG_SCREEN_LTDC
#include "BSP_LCD.h"
#else // BSP_CONFIG_SCREEN_SPI
#include "app_display.h"
#include "stm32_lcd.h"
#include "lcd.h"
#endif

/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/

/* Global variables ----------------------------------------------------------*/

/*Application context*/
AppConfig_TypeDef App_Config;

/*Table of classes for the NN model*/
const char* classes_table[AI_NETWORK_OUT_1_SIZE] = {"telephone","stylo"};

/* Private variables ---------------------------------------------------------*/

 /***Buffer to store the NN input frame***/
__attribute__((section(".NN_InputImage_Buffer")))
__attribute__ ((aligned (32)))
#ifdef AI_NETWORK_INPUTS_IN_ACTIVATIONS
 uint8_t *NN_InputImage_Buffer=NULL;
#else 
 uint8_t NN_InputImage_Buffer[AI_INPUT_BUFFER_SIZE + 32 - (AI_INPUT_BUFFER_SIZE%32)];
#endif 
 
 /***Buffer to store the NN ouput data***/
__attribute__((section(".NN_OutputData_Buffer")))
__attribute__ ((aligned (32)))
#ifdef AI_NETWORK_OUTPUTS_IN_ACTIVATIONS
 uint8_t *NN_OutputData_Buffer=NULL;
#else 
 uint8_t NN_OutputData_Buffer[AI_OUTPUT_BUFFER_SIZE + 32 - (AI_OUTPUT_BUFFER_SIZE%32)]= {0};
#endif 
  
 /***Buffer to store the NN Activation data***/
ai_handle NN_Activation_Buffer[] = {(ai_handle)0x90100000};

/***LCD display buffers***/
volatile DMA_BUFFER uint8_t lcd_display_global_memory[LCD_FRAME_BUFFER_SIZE + 32 - (LCD_FRAME_BUFFER_SIZE%32)];
volatile DMA_BUFFER uint8_t buffer_tmp[320 * 240 * 2] = {0};
// AI
uint8_t buffer_ipl[IPL_BUFFER_SIZE];
EXTMEM uint8_t buffer_ai[AI_INPUT_H * AI_INPUT_W * 3] = {0};
// ai_i8 reserve some space for ai_float,Prevent arrays from crossing the bounds
// static ai_u8 network_output[AI_NETWORK_OUT_1_SIZE * 4];

char msg[70]; // display msg on LCD

image_t frame_rgb565 = {
    .w = 320,
    .h = 240,
    .bpp = IMAGE_BPP_RGB565,
    .data = NULL};


/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void PeriphCommonClock_Config(void);
static void MPU_Config(void);
static void Software_Init(AppConfig_TypeDef *);
static void Hardware_Init(AppConfig_TypeDef *);


/* Private user code ---------------------------------------------------------*/

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* MPU Configuration--------------------------------------------------------*/
  MPU_Config();
  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_InvalidateICache();
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_InvalidateDCache();
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();


  /* Configure the system clock */
  SystemClock_Config();

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* Perfom SW configuration related to the application  */
  Software_Init(&App_Config);
  
  /* Perfom HW configuration (display, camera) related to the application  */
  Hardware_Init(&App_Config);
  
  /* Initialize the Neural Network library  */
  printf("Network_Init : ...");
  Network_Init(&App_Config);
  printf(": OK\r\n");


  printf("Vittascience Wio Lite AI Application ready\r\n");
  BSP_LED_On(LED_RED);

  // initial IPL and AI
  STM32Ipl_InitLib(buffer_ipl, IPL_BUFFER_SIZE);

  frame_rgb565.data = buffer_tmp;

  vittascience_i2c_init();

#ifdef BSP_CONFIG_SCREEN_LTDC
  sprintf(msg, "VITTASCIENCE \r\n WIO LITE AI");
  LCD_Frame_Draw_String(10, 10, 300, 220, 35, msg, ROTATION_RIGHT);
  LCD_Frame_Draw_Rectangle(48, 8, 224, 224, ROTATION_RIGHT);
  LCD_Frame_Show();
#else //BSP_CONFIG_SCREEN_SPI
  Display_WelcomeScreen(&App_Config);
#endif

  BSP_CAMERA_ContinuousStart(lcd_display_global_memory);

  /* Infinite loop */
  while (1)
  {
    MX_X_CUBE_AI_Process(&App_Config);
  }
}
void BSP_PB_Callback(Button_TypeDef Button)
{
  if (Button == BUTTON_USER1)
  {
    BSP_LED_Toggle(LED_RED);
  }
}

void BSP_CAMERA_FrameEventCallback(void)
{
  char msg[70];
  BSP_CAMERA_Suspend();
  if ( App_Config.buffer_tmp_in_use == false)
  {
    App_Config.buffer_tmp_in_use = true;
    memcpy(buffer_tmp, lcd_display_global_memory, sizeof(lcd_display_global_memory));
    App_Config.new_data_available = true;
    App_Config.buffer_tmp_in_use = false;
  }

#ifdef BSP_CONFIG_SCREEN_LTDC
  LCD_Frame_Draw_Image(0, 0, 320, 240, (uint16_t *)App_Config.lcd_frame_buff, ROTATION_RIGHT);
  sprintf(msg, "%s %.0f%%", App_Config.nn_top1_output_class_name, App_Config.nn_top1_output_class_proba);
  LCD_Frame_Draw_String(190, 200, 300, 16, 16, msg, ROTATION_RIGHT);
  sprintf(msg, "Inference: %ldms", App_Config.nn_inference_time);
  LCD_Frame_Draw_String(190, 220, 300, 16, 16, msg, ROTATION_RIGHT);
  LCD_Frame_Draw_Rectangle(48, 8, 224, 224, ROTATION_RIGHT);
  LCD_Frame_Show();
#else // BSP_CONFIG_SCREEN_SPI
  sprintf(msg, "%s %.0f%%", App_Config.nn_top1_output_class_name, App_Config.nn_top1_output_class_proba);
  UTIL_LCD_DisplayStringAt(10, LINE(0), msg, LEFT_MODE);
  sprintf(msg, "Inference: %ldms", App_Config.nn_inference_time);
  UTIL_LCD_DisplayStringAt(10, LINE(1), msg, LEFT_MODE);
  UTIL_LCD_DrawRect(48, 8, 224, 224, UTIL_LCD_COLOR_GREEN);
  BSP_DISPLAY_SPI_DrawImage((const char *) App_Config.lcd_frame_buff);
#endif
  BSP_CAMERA_Resume();
}


/* Private functions ---------------------------------------------------------*/
/**
 * @brief Initializes the application context structure
 * @param App_Config_Ptr pointer to application context
 */
static void Software_Init(AppConfig_TypeDef *App_Config_Ptr)
{
  App_Config_Ptr->mode_continuous = true;
  App_Config_Ptr->do_ai = false;
  App_Config_Ptr->new_data_available = false;
  App_Config_Ptr->buffer_tmp_in_use = false;
  // App_Config_Ptr->mirror_flip = CAMERA_MIRRORFLIP_FLIP;
  // App_Config_Ptr->new_frame_ready = 0;
 
  // App_Config_Ptr->lcd_sync = 0;
  
  // App_Config_Ptr->lut = pixel_conv_lut;
  
  // App_Config_Ptr->nn_input_type = QUANT_INPUT_TYPE;
  // App_Config_Ptr->nn_output_type = QUANT_OUTPUT_TYPE;
  
  App_Config_Ptr->nn_output_labels = classes_table;
  
  /*Preproc*/
// #if PP_COLOR_MODE == RGB_FORMAT
//   App_Config_Ptr->red_blue_swap = 1; /* See UM2611 section 3.2.6 Pixel data order */
// #else
//   App_Config_Ptr->red_blue_swap = 0;
// #endif
  
// #if PP_COLOR_MODE == GRAYSCALE_FORMAT
//   App_Config_Ptr->PixelFormatConv = SW_PFC;
// #else
//   App_Config_Ptr->PixelFormatConv = HW_PFC;
// #endif
  
  /*Memory buffer init*/
  App_Config_Ptr->nn_input_buffer = NN_InputImage_Buffer; 
  App_Config_Ptr->nn_output_buffer = NN_OutputData_Buffer;
  // App_Config_Ptr->camera_capture_buffer = CapturedImage_Buffer;
  // App_Config_Ptr->camera_capture_buffer_no_borders = App_Config_Ptr->camera_capture_buffer+((CAM_RES_WIDTH - CAM_RES_HEIGHT)/2)*CAM_RES_WIDTH*RGB_565_BPP;
  // App_Config_Ptr->rescaled_image_buffer = RescaledImage_Buffer;
  App_Config_Ptr->activation_buffer = NN_Activation_Buffer;
  App_Config_Ptr->lcd_frame_buff = lcd_display_global_memory;
}

/**
 * @brief Initializes the WH peripherals
 * @param App_Config_Ptr pointer to application context
 */
static void Hardware_Init(AppConfig_TypeDef *App_Config_Ptr)
{
  // /*LEDs Init*/
  // BSP_LED_Init(LED_GREEN);
  // BSP_LED_Init(LED_RED);
  // BSP_LED_Init(LED_YELLOW);
  

  
  // /*Camera init*/
  // Camera_Init(App_Config_Ptr);

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  
  COM_InitTypeDef COM_Init;
  COM_Init.BaudRate = 115200;
  COM_Init.HwFlowCtl = COM_HWCONTROL_NONE;
  COM_Init.Parity = COM_PARITY_NONE;
  COM_Init.StopBits = COM_STOPBITS_1;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  BSP_COM_Init(COM1,&COM_Init);
  printf("Wio Lite Ai initialising\r\n");

  printf("Led And Button : ...");
  BSP_LED_Init(LED_RED);
  BSP_LED_Init(LED_YELLOW);
  BSP_PB_Init(BUTTON_USER1, BUTTON_MODE_EXTI);
  printf(": OK\r\n");
  printf("OSPI_RAM : ...");
  BSP_OSPI_RAM_Init(0);
  BSP_OSPI_RAM_EnableMemoryMappedMode(0);
  printf(": OK\r\n");
  printf("Timer 2 : ...");
  MX_TIM2_Init();
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); // XCLK
  printf(": OK\r\n");
  printf("Camera : ...");
  BSP_CAMERA_Init(RESOLUTION_R320x240);
  printf(": OK\r\n");
  printf("CRC : ...");
  MX_CRC_Init();
  printf(": OK\r\n");
  printf("OCTOSPI2 : ...");
  MX_OCTOSPI2_Init();
  printf(": OK\r\n");
#ifdef BSP_CONFIG_SCREEN_LTDC
  printf("LTDC : ...");
  MX_LTDC_Init();
  printf(": OK\r\n");
#else // BSP_CONFIG_SCREEN_SPI
  Display_Init(App_Config_Ptr);
  HAL_GPIO_WritePin(ILI9341_LED_GPIO_Port, ILI9341_LED_Pin, GPIO_PIN_SET);
#endif
#ifdef BSP_CONFIG_VITTASCIENCE
  BSP_I2C1_Init();
#endif
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
   */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
   */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
  {
  }

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 2;
  RCC_OscInitStruct.PLL.PLLN = 44;
  RCC_OscInitStruct.PLL.PLLP = 1;
  RCC_OscInitStruct.PLL.PLLQ = 5;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
static void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
   */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_OSPI;
  PeriphClkInitStruct.PLL2.PLL2M = 25;
  PeriphClkInitStruct.PLL2.PLL2N = 266;
  PeriphClkInitStruct.PLL2.PLL2P = 2;
  PeriphClkInitStruct.PLL2.PLL2Q = 2;
  PeriphClkInitStruct.PLL2.PLL2R = 1;
  PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_0;
  PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
  PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
  PeriphClkInitStruct.OspiClockSelection = RCC_OSPICLKSOURCE_PLL2;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* MPU Configuration */

void MPU_Config(void)
{

  /* Disables the MPU */
  HAL_MPU_Disable();
  /* Enables the MPU */
  HAL_MPU_Enable(MPU_HFNMI_PRIVDEF);
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
