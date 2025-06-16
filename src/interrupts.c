/* clang-format off */

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* This file contains all the interrupts                                      */
/*                                                                            */
/*----------------------------------------------------------------------------*/

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_jpeg.h"
#include "stm32f7xx_hal_sai.h"

/*
** Interrupts for audio
*/
extern SAI_HandleTypeDef haudio_out_sai;
void DMA2_Stream1_IRQHandler(void) {
    HAL_DMA_IRQHandler(haudio_out_sai.hdmatx);
}

/*
** Interrupts for jpeg
*/
extern JPEG_HandleTypeDef jpeg_handle;
void JPEG_IRQHandler(void) {
    HAL_JPEG_IRQHandler(&jpeg_handle);
}
