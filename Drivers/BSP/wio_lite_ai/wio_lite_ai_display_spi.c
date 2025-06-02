/* Includes ------------------------------------------------------------------*/
#include "ILI9341_STM32_Driver.h"

/* Functions definition ------------------------------------------------------*/
int32_t BSP_DISPLAY_SPI_Init(void)
{
  return ILI9341_Init(SCREEN_HORIZONTAL_2);
}

void BSP_DISPLAY_SPI_DrawImage(const char* Image_Array)
{
  ILI9341_Draw_Image(Image_Array, SCREEN_HORIZONTAL_2);
}