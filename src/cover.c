/* clang-format off */

#include "stm32f7xx_hal.h"
#include "stm32f7xx_hal_jpeg.h"
#include "stm32f7xx_hal_dma2d.h"
#include "stm32f769i_discovery_lcd.h"
#include "jpeg_utils.h"
#include "ff.h"
#include "helper_functions.h"

#include <stdbool.h>

#define LCD_FRAME_BUFFER        0xC0000000
#define JPEG_OUTPUT_DATA_BUFFER 0xC0200000

/*
** keeping track of processed bytes
*/
uint32_t MCU_TotalNb = 0;
uint32_t num_bytes_decoded = 0;

/*
** needed for processing jpeg
*/
FIL *jpeg_file;
unsigned int jpeg_num_bytes_read = 0;
unsigned int jpeg_file_offset = 0;
uint8_t jpeg_buffer[10000];
int jpeg_complete = 0;

/*
** global variables for HAL
*/
JPEG_HandleTypeDef jpeg_handle;
JPEG_ConfTypeDef jpeg_info;
DMA2D_HandleTypeDef DMA2D_Handle;

/*
** Helper function for copying data with DMA2D
*/
static bool DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, JPEG_ConfTypeDef *jpeg_info);

/*
** Initializes everything needed for displaying the album cover
** Returns 'true' if everything intialized correctly
*/
bool Cover_Init(void) {
    // Initialize JPEG
    jpeg_handle.Instance = JPEG;
    HAL_StatusTypeDef res = HAL_JPEG_Init(&jpeg_handle);
    if (res != HAL_OK) return false;
    JPEG_InitColorTables();

    DMA2D_Handle.Init.Mode                  = DMA2D_M2M_PFC;
    DMA2D_Handle.Init.ColorMode             = DMA2D_OUTPUT_ARGB8888;
    DMA2D_Handle.Init.AlphaInverted         = DMA2D_REGULAR_ALPHA;
    DMA2D_Handle.Init.RedBlueSwap           = DMA2D_RB_REGULAR;
    DMA2D_Handle.XferCpltCallback           = NULL;
    DMA2D_Handle.LayerCfg[1].AlphaMode      = DMA2D_REPLACE_ALPHA;
    DMA2D_Handle.LayerCfg[1].InputAlpha     = 0xFF;
    DMA2D_Handle.LayerCfg[1].InputColorMode = DMA2D_INPUT_ARGB8888;
    DMA2D_Handle.LayerCfg[1].RedBlueSwap    = DMA2D_RB_REGULAR;
    DMA2D_Handle.LayerCfg[1].AlphaInverted  = DMA2D_REGULAR_ALPHA;
    DMA2D_Handle.Instance                   = DMA2D;

    return true;
}

/*
** Displays the jpeg image in 'file' in the center of the LCD screen
** Returns 'true' if everything initializes correctly
*/
bool Cover_Display(FIL *file) {
    num_bytes_decoded = 0;
    jpeg_num_bytes_read = 0;
    jpeg_file_offset = 0;
    jpeg_complete = 0;
    jpeg_file = file;

    f_read(jpeg_file, jpeg_buffer, sizeof(jpeg_buffer), &jpeg_num_bytes_read);
    jpeg_file_offset = jpeg_num_bytes_read;

    // Decode the passed file without DMA
    HAL_StatusTypeDef status = HAL_JPEG_Decode(&jpeg_handle,
                                               jpeg_buffer,
                                               jpeg_num_bytes_read,
                                               (uint8_t*)JPEG_OUTPUT_DATA_BUFFER,
                                               10000,
                                               HAL_MAX_DELAY);
    if (status != HAL_OK) return false;
    while (!jpeg_complete);

    // Get the info
    HAL_JPEG_GetInfo(&jpeg_handle, &jpeg_info);

    // Do the color conversion on decoded data
    uint8_t *raw_output = colorConversion((uint8_t *)JPEG_OUTPUT_DATA_BUFFER, num_bytes_decoded);

    uint32_t width_offset = 0;
    if (jpeg_info.ChromaSubsampling == JPEG_420_SUBSAMPLING) {
        if ((jpeg_info.ImageWidth % 16) != 0)
            width_offset = 16 - (jpeg_info.ImageWidth % 16);
    }

    if (jpeg_info.ChromaSubsampling == JPEG_422_SUBSAMPLING) {
        if ((jpeg_info.ImageWidth % 16) != 0)
            width_offset = 16 - (jpeg_info.ImageWidth % 16);
    }

    if (jpeg_info.ChromaSubsampling == JPEG_444_SUBSAMPLING) {
        if ((jpeg_info.ImageWidth % 8) != 0)
            width_offset = 8 - (jpeg_info.ImageWidth % 8);
    }

    DMA2D_Handle.Init.OutputOffset  = BSP_LCD_GetXSize() - jpeg_info.ImageWidth;
    DMA2D_Handle.LayerCfg[1].InputOffset = width_offset;

    /* DMA2D Initialization */
    if (HAL_DMA2D_DeInit(&DMA2D_Handle) != HAL_OK) return false;
    if (HAL_DMA2D_Init(&DMA2D_Handle) != HAL_OK) return false;

    /* DMA2D Config Layer */
    if (HAL_DMA2D_ConfigLayer(&DMA2D_Handle, 1) != HAL_OK) return false;

    // Copy decoded data to the LCD
    uint32_t xPos = (BSP_LCD_GetXSize() - jpeg_info.ImageWidth)/2;
    uint32_t yPos = (BSP_LCD_GetYSize() - jpeg_info.ImageHeight)/2 - 100;
    if (!DMA2D_CopyBuffer((uint32_t *)raw_output, (uint32_t *)LCD_FRAME_BUFFER, xPos , yPos, &jpeg_info)) {
        return false;
    }

    return true;
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* CALLBACKS                                                                  */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*
 * Callback called whenever the JPEG needs more data
 */
void HAL_JPEG_GetDataCallback(JPEG_HandleTypeDef *hjpeg, uint32_t NbDecodedData) {
    if (NbDecodedData != jpeg_num_bytes_read) {
        jpeg_file_offset = jpeg_file_offset - jpeg_num_bytes_read + NbDecodedData;
        f_lseek(jpeg_file, jpeg_file_offset);
    }

    f_read(jpeg_file, jpeg_buffer, sizeof(jpeg_buffer), &jpeg_num_bytes_read);
    jpeg_file_offset += jpeg_num_bytes_read;
    HAL_JPEG_ConfigInputBuffer(hjpeg, jpeg_buffer, jpeg_num_bytes_read);
}

/*
 * Callback called whenever the JPEG has filled the output buffer
 */
void HAL_JPEG_DataReadyCallback(JPEG_HandleTypeDef *hjpeg, uint8_t *pDataOut, uint32_t OutDataLength) {
    num_bytes_decoded += OutDataLength;
    HAL_JPEG_ConfigOutputBuffer(hjpeg, (uint8_t*)(pDataOut+OutDataLength), 10000);
}

/*
 * Callback called whenever the JPEG has completed
 */
void HAL_JPEG_DecodeCpltCallback(JPEG_HandleTypeDef *hjpeg) { jpeg_complete = 1; }

// Provided code for callback
// Called when the jpeg header has been parsed
// Adjust the width to be a multiple of 8 or 16 (depending on image configuration) (from STM examples)
// Get the correct color conversion function to use to convert to RGB
void HAL_JPEG_InfoReadyCallback(JPEG_HandleTypeDef *hjpeg, JPEG_ConfTypeDef *pInfo) {
    // Have to add padding for DMA2D
    if (pInfo->ChromaSubsampling == JPEG_420_SUBSAMPLING) {
        if ((pInfo->ImageWidth % 16) != 0)
            pInfo->ImageWidth += (16 - (pInfo->ImageWidth % 16));

        if ((pInfo->ImageHeight % 16) != 0)
            pInfo->ImageHeight += (16 - (pInfo->ImageHeight % 16));
    }

    if (pInfo->ChromaSubsampling == JPEG_422_SUBSAMPLING) {
        if ((pInfo->ImageWidth % 16) != 0)
            pInfo->ImageWidth += (16 - (pInfo->ImageWidth % 16));

        if ((pInfo->ImageHeight % 8) != 0)
            pInfo->ImageHeight += (8 - (pInfo->ImageHeight % 8));
    }

    if (pInfo->ChromaSubsampling == JPEG_444_SUBSAMPLING) {
        if ((pInfo->ImageWidth % 8) != 0)
            pInfo->ImageWidth += (8 - (pInfo->ImageWidth % 8));

        if ((pInfo->ImageHeight % 8) != 0)
            pInfo->ImageHeight += (8 - (pInfo->ImageHeight % 8));
    }

    if (JPEG_GetDecodeColorConvertFunc(pInfo, &pConvert_Function, &MCU_TotalNb) != HAL_OK) {
        while(1);
    }
}

/*
** Callback for intializing everything else for JPEG
*/
void HAL_JPEG_MspInit(JPEG_HandleTypeDef *hjpeg) {
    __HAL_RCC_JPEG_CLK_ENABLE();

    HAL_NVIC_SetPriority(JPEG_IRQn, 0x06, 0x0F);
    HAL_NVIC_EnableIRQ(JPEG_IRQn);
}

/*
** Helper function for copying data with DMA2D
*/
bool DMA2D_CopyBuffer(uint32_t *pSrc, uint32_t *pDst, uint16_t x, uint16_t y, JPEG_ConfTypeDef *jpeg_info) {
    uint32_t destination = (uint32_t)pDst + (y * BSP_LCD_GetXSize() + x) * 4;
    uint32_t source      = (uint32_t)pSrc;

    /* DMA2D Start */
    if (HAL_DMA2D_Start(&DMA2D_Handle, source, destination, jpeg_info->ImageWidth, jpeg_info->ImageHeight) != HAL_OK) {
        return false;
    }

    /* DMA2D Poll for Transfer */
    if (HAL_DMA2D_PollForTransfer(&DMA2D_Handle, HAL_MAX_DELAY) != HAL_OK) return false;

    return true;
}
