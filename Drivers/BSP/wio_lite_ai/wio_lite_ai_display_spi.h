#ifndef WIO_LITE_AI_DISPLAY_SPI_H
#define WIO_LITE_AI_DISPLAY_SPI_H

/* Includes ------------------------------------------------------------------*/
#include "lcd.h"

/* Functions prototype -------------------------------------------------------*/
int32_t BSP_DISPLAY_SPI_Init(void);
void BSP_DISPLAY_SPI_DrawImage(const char* Image_Array);

#endif 