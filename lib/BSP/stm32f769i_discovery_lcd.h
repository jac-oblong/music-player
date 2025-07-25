/**
 ******************************************************************************
 * @file    stm32f769i_discovery_lcd.h
 * @author  MCD Application Team
 * @brief   This file contains the common defines and functions prototypes for
 *          the stm32469i_discovery_lcd.c driver.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2016 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F769I_DISCOVERY_LCD_H
#define __STM32F769I_DISCOVERY_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Include LCD component Driver */

/* Include OTM8009A LCD Driver IC driver code */
#include "otm8009a.h"
/* Include NT35510 LCD Driver IC driver code */
#include "nt35510.h"
/* Include ADV7533 HDMI Driver IC driver code */
#include "adv7533.h"

/* Include SDRAM Driver */
#include "stm32f769i_discovery.h"
#include "stm32f769i_discovery_sdram.h"

#include "fonts.h"

#include <string.h> /* use of memset() */

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STM32F769I_DISCOVERY
 * @{
 */

/** @addtogroup STM32F769I_DISCOVERY_LCD
 * @{
 */

/** @defgroup STM32F769I_DISCOVERY_LCD_Exported_Constants STM32F769I DISCOVERY
 * LCD Exported Constants
 * @{
 */
#define BSP_LCD_DMA2D_IRQHandler DMA2D_IRQHandler
#define BSP_LCD_DSI_IRQHandler DSI_IRQHandler
#define BSP_LCD_LTDC_IRQHandler LTDC_IRQHandler
#define BSP_LCD_LTDC_ER_IRQHandler LTDC_ER_IRQHandler

#define LCD_LayerCfgTypeDef LTDC_LayerCfgTypeDef
/**
 * @brief  LCD FB_StartAddress
 */
#define LCD_FB_START_ADDRESS ((uint32_t)0xC0000000)

/** @brief Maximum number of LTDC layers
 */
#define LTDC_MAX_LAYER_NUMBER ((uint32_t)2)

/** @brief LTDC Background layer index
 */
#define LTDC_ACTIVE_LAYER_BACKGROUND ((uint32_t)0)

/** @brief LTDC Foreground layer index
 */
#define LTDC_ACTIVE_LAYER_FOREGROUND ((uint32_t)1)

/** @brief Number of LTDC layers
 */
#define LTDC_NB_OF_LAYERS ((uint32_t)2)

/** @brief LTDC Default used layer index
 */
#define LTDC_DEFAULT_ACTIVE_LAYER LTDC_ACTIVE_LAYER_FOREGROUND

/**
 * @brief  LCD status structure definition
 */
#define LCD_OK 0x00
#define LCD_ERROR 0x01
#define LCD_TIMEOUT 0x02

/**
 * @brief  LCD Display OTM8009A DSI Virtual Channel  ID
 */
#define LCD_OTM8009A_ID ((uint32_t)0)

/**
 * @brief  HDMI ADV7533 DSI Virtual Channel  ID
 */
#define HDMI_ADV7533_ID ((uint32_t)0)

/**
 * @brief  HDMI Foramt
 */
#define HDMI_FORMAT_720_480                                                    \
  ((uint8_t)0x00) /*720_480 format choice of HDMI display */
#define HDMI_FORMAT_720_576                                                    \
  ((uint8_t)0x01) /*720_576 format choice of HDMI display*/

/**
 * @brief  LCD color definitions values
 * in ARGB8888 format.
 */

/** @brief Blue value in ARGB8888 format
 */
#define LCD_COLOR_BLUE ((uint32_t)0xFF0000FF)

/** @brief Green value in ARGB8888 format
 */
#define LCD_COLOR_GREEN ((uint32_t)0xFF00FF00)

/** @brief Red value in ARGB8888 format
 */
#define LCD_COLOR_RED ((uint32_t)0xFFFF0000)

/** @brief Cyan value in ARGB8888 format
 */
#define LCD_COLOR_CYAN ((uint32_t)0xFF00FFFF)

/** @brief Magenta value in ARGB8888 format
 */
#define LCD_COLOR_MAGENTA ((uint32_t)0xFFFF00FF)

/** @brief Yellow value in ARGB8888 format
 */
#define LCD_COLOR_YELLOW ((uint32_t)0xFFFFFF00)

/** @brief Light Blue value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTBLUE ((uint32_t)0xFF8080FF)

/** @brief Light Green value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTGREEN ((uint32_t)0xFF80FF80)

/** @brief Light Red value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTRED ((uint32_t)0xFFFF8080)

/** @brief Light Cyan value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTCYAN ((uint32_t)0xFF80FFFF)

/** @brief Light Magenta value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTMAGENTA ((uint32_t)0xFFFF80FF)

/** @brief Light Yellow value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTYELLOW ((uint32_t)0xFFFFFF80)

/** @brief Dark Blue value in ARGB8888 format
 */
#define LCD_COLOR_DARKBLUE ((uint32_t)0xFF000080)

/** @brief Light Dark Green value in ARGB8888 format
 */
#define LCD_COLOR_DARKGREEN ((uint32_t)0xFF008000)

/** @brief Light Dark Red value in ARGB8888 format
 */
#define LCD_COLOR_DARKRED ((uint32_t)0xFF800000)

/** @brief Dark Cyan value in ARGB8888 format
 */
#define LCD_COLOR_DARKCYAN ((uint32_t)0xFF008080)

/** @brief Dark Magenta value in ARGB8888 format
 */
#define LCD_COLOR_DARKMAGENTA ((uint32_t)0xFF800080)

/** @brief Dark Yellow value in ARGB8888 format
 */
#define LCD_COLOR_DARKYELLOW ((uint32_t)0xFF808000)

/** @brief White value in ARGB8888 format
 */
#define LCD_COLOR_WHITE ((uint32_t)0xFFFFFFFF)

/** @brief Light Gray value in ARGB8888 format
 */
#define LCD_COLOR_LIGHTGRAY ((uint32_t)0xFFD3D3D3)

/** @brief Gray value in ARGB8888 format
 */
#define LCD_COLOR_GRAY ((uint32_t)0xFF808080)

/** @brief Dark Gray value in ARGB8888 format
 */
#define LCD_COLOR_DARKGRAY ((uint32_t)0xFF404040)

/** @brief Black value in ARGB8888 format
 */
#define LCD_COLOR_BLACK ((uint32_t)0xFF000000)

/** @brief Brown value in ARGB8888 format
 */
#define LCD_COLOR_BROWN ((uint32_t)0xFFA52A2A)

/** @brief Orange value in ARGB8888 format
 */
#define LCD_COLOR_ORANGE ((uint32_t)0xFFFFA500)

/** @brief Transparent value in ARGB8888 format
 */
#define LCD_COLOR_TRANSPARENT ((uint32_t)0xFF000000)

/**
 * @brief LCD default font
 */
#define LCD_DEFAULT_FONT Font24

/**
 *  @brief  Possible values of
 *  pixel data format (ie color coding) transmitted on DSI Data lane in DSI
 * packets
 */

#define LCD_DSI_PIXEL_DATA_FMT_RBG888                                          \
  DSI_RGB888 /*!< DSI packet pixel format chosen is RGB888 : 24 bpp */
#define LCD_DSI_PIXEL_DATA_FMT_RBG565                                          \
  DSI_RGB565 /*!< DSI packet pixel format chosen is RGB565 : 16 bpp */

/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_LCD_Exported_Types STM32F769I DISCOVERY LCD
 * Exported Types
 * @{
 */

/**
 * @brief  LCD Drawing main properties
 */
typedef struct {
  uint32_t TextColor; /*!< Specifies the color of text */
  uint32_t BackColor; /*!< Specifies the background color below the text */
  sFONT *pFont;       /*!< Specifies the font used for the text */

} LCD_DrawPropTypeDef;

/**
 * @brief  LCD Drawing point (pixel) geometric definition
 */
typedef struct {
  int16_t X; /*!< geometric X position of drawing */
  int16_t Y; /*!< geometric Y position of drawing */

} Point;

/**
 * @brief  Pointer on LCD Drawing point (pixel) geometric definition
 */
typedef Point *pPoint;

/**
 * @brief  LCD drawing Line alignment mode definitions
 */
typedef enum {
  CENTER_MODE = 0x01, /*!< Center mode */
  RIGHT_MODE = 0x02,  /*!< Right mode  */
  LEFT_MODE = 0x03    /*!< Left mode   */

} Text_AlignModeTypdef;

/**
 *  @brief LCD_OrientationTypeDef
 *  Possible values of Display Orientation
 */
typedef enum {
  LCD_ORIENTATION_PORTRAIT =
      0x00, /*!< Portrait orientation choice of LCD screen  */
  LCD_ORIENTATION_LANDSCAPE =
      0x01, /*!< Landscape orientation choice of LCD screen */
  LCD_ORIENTATION_INVALID =
      0x02 /*!< Invalid orientation choice of LCD screen   */
} LCD_OrientationTypeDef;

/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_LCD_Exported_Macro STM32F769I DISCOVERY LCD
 * Exported Macro
 * @{
 */

/** @addtogroup STM32F769I_DISCOVERY_LCD_Exported_Functions
 * @{
 */
uint8_t BSP_LCD_Init(void);
uint8_t BSP_LCD_InitEx(LCD_OrientationTypeDef orientation);
uint8_t BSP_LCD_HDMIInitEx(uint8_t format);

void BSP_LCD_MspDeInit(void);
void BSP_LCD_MspInit(void);
void BSP_LCD_Reset(void);

uint32_t BSP_LCD_GetXSize(void);
uint32_t BSP_LCD_GetYSize(void);
void BSP_LCD_SetXSize(uint32_t imageWidthPixels);
void BSP_LCD_SetYSize(uint32_t imageHeightPixels);

void BSP_LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t FB_Address);
void BSP_LCD_SetTransparency(uint32_t LayerIndex, uint8_t Transparency);
void BSP_LCD_SetLayerAddress(uint32_t LayerIndex, uint32_t Address);
void BSP_LCD_SetColorKeying(uint32_t LayerIndex, uint32_t RGBValue);
void BSP_LCD_ResetColorKeying(uint32_t LayerIndex);
void BSP_LCD_SetLayerWindow(uint16_t LayerIndex, uint16_t Xpos, uint16_t Ypos,
                            uint16_t Width, uint16_t Height);

void BSP_LCD_SelectLayer(uint32_t LayerIndex);
void BSP_LCD_SetLayerVisible(uint32_t LayerIndex, FunctionalState State);

void BSP_LCD_SetTextColor(uint32_t Color);
uint32_t BSP_LCD_GetTextColor(void);
void BSP_LCD_SetBackColor(uint32_t Color);
uint32_t BSP_LCD_GetBackColor(void);
void BSP_LCD_SetFont(sFONT *fonts);
sFONT *BSP_LCD_GetFont(void);

uint32_t BSP_LCD_ReadPixel(uint16_t Xpos, uint16_t Ypos);
void BSP_LCD_DrawPixel(uint16_t Xpos, uint16_t Ypos, uint32_t pixel);
void BSP_LCD_Clear(uint32_t Color);
void BSP_LCD_ClearStringLine(uint32_t Line);
void BSP_LCD_DisplayStringAtLine(uint16_t Line, uint8_t *ptr);
void BSP_LCD_DisplayStringAt(uint16_t Xpos, uint16_t Ypos, uint8_t *Text,
                             Text_AlignModeTypdef Mode);
void BSP_LCD_DisplayChar(uint16_t Xpos, uint16_t Ypos, uint8_t Ascii);

void BSP_LCD_DrawHLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void BSP_LCD_DrawVLine(uint16_t Xpos, uint16_t Ypos, uint16_t Length);
void BSP_LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
void BSP_LCD_DrawRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
                      uint16_t Height);
void BSP_LCD_DrawCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_DrawPolygon(pPoint Points, uint16_t PointCount);
void BSP_LCD_DrawEllipse(int Xpos, int Ypos, int XRadius, int YRadius);
void BSP_LCD_DrawBitmap(uint32_t Xpos, uint32_t Ypos, uint8_t *pbmp);

void BSP_LCD_FillRect(uint16_t Xpos, uint16_t Ypos, uint16_t Width,
                      uint16_t Height);
void BSP_LCD_FillCircle(uint16_t Xpos, uint16_t Ypos, uint16_t Radius);
void BSP_LCD_FillPolygon(pPoint Points, uint16_t PointCount);
void BSP_LCD_FillEllipse(int Xpos, int Ypos, int XRadius, int YRadius);

void BSP_LCD_DisplayOn(void);
void BSP_LCD_DisplayOff(void);
void BSP_LCD_SetBrightness(uint8_t BrightnessValue);

/**
 * @}
 */

/** @defgroup STM32F769I_DISCOVERY_LCD_Exported_Variables STM32F769I DISCOVERY
 * LCD Exported Variables
 * @{
 */

/* @brief DMA2D handle variable */
extern DMA2D_HandleTypeDef hdma2d_discovery;

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F769I_DISCOVERY_LCD_H */
