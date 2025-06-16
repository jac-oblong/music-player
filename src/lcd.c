/* clang-format off */

#include "lcd.h"

#include "music.h"
#include "stm32f769i_discovery_lcd.h"
#include "stm32f769i_discovery_ts.h"
#include "stm32f7xx_hal.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SQRT_3 1.73

#define LCD_FRAME_BUFFER 0xC0000000
#define LCD_FG           0xFFCDD6F4
#define LCD_BG           0xFF1E1E2E

// leeway we are giving to touch-screen UI elements
#define UI_TS_LEEWAY 10

// width of screen
#define UI_X        472
// height of screen
#define UI_Y        800

// y position
#define UI_VOL_Y    580
// radius
#define UI_VOL_R    30
// gap between edge of screen and circle
#define UI_VOL_OGAP 50
// gap between circle and +/-
#define UI_VOL_IGAP 15
// x position of volume down
#define UI_VOL_DN_X (UI_VOL_OGAP + UI_VOL_R)
// x position of volume up
#define UI_VOL_UP_X (UI_X - (UI_VOL_OGAP + UI_VOL_R))
// length of lines that make up +/-
#define UI_VOL_LINE ((UI_VOL_R - UI_VOL_IGAP)*2)

// y position of pause/play
#define UI_PAUSE_PLAY_Y 690
// x position of pause/play
#define UI_PAUSE_PLAY_X (UI_X/2)
// scale factor of pause/play
#define UI_PAUSE_PLAY_S 80

// y position of next
#define UI_NEXT_Y 690
// x position of next
#define UI_NEXT_X (UI_X/2 + UI_X/4)
// scale factor of next
#define UI_NEXT_S 80


TS_StateTypeDef TS_State;

static void LCD_DrawVolUp(void);
static void LCD_DrawVolDown(void);
static void LCD_DrawNext(void);

/*
** Initializes everything needed for LCD and TS
** Returns 'true' if everything intialized correctly
*/
bool LCD_Init(void) {
    BSP_LCD_InitEx(LCD_ORIENTATION_PORTRAIT);
    BSP_LCD_LayerDefaultInit(0, LCD_FRAME_BUFFER);
    BSP_LCD_SelectLayer(0);
    BSP_LCD_Clear(LCD_BG);
    BSP_LCD_SetBackColor(LCD_BG);
    BSP_LCD_SetTextColor(LCD_FG);

    LCD_DrawNext();
    LCD_DrawPause();
    LCD_DrawVolUp();
    LCD_DrawVolDown();
    LCD_DrawVol();

    BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());

    return true;
}

/*
** Displays the song title
** Returns 'true' if everything initializes correctly
*/
bool LCD_SongTitle(char *title) {
    BSP_LCD_SetTextColor(LCD_BG);
    BSP_LCD_FillRect(0, 0, 480, 100);
    BSP_LCD_SetTextColor(LCD_FG);

    BSP_LCD_DisplayStringAt(0,30, (uint8_t *)title, CENTER_MODE);
    return true;
}

/*
** Displays the song artist
** Returns 'true' if everything initializes correctly
*/
bool LCD_SongArtist(char *artist) {
    BSP_LCD_DisplayStringAt(0,60, (uint8_t *)artist, CENTER_MODE);
    return true;
}

/*
** Returns enum value of user input
*/
TS_Input LCD_GetUserInput(void) {
    // this function is so weird because of "phantom" presses that happen after
    // not pressing the TS for some period of time (usually 20-30 seconds)
    // the way it works, is by waiting until the press is released and then
    // ensuring that it lasted >= X ms (via HAL SysTick)

    static TS_Input pressed_input = TS_INPUT_NONE;
    static uint32_t pressed_time = 0;

    uint16_t x1, y1;
    BSP_TS_GetState(&TS_State);
    if (pressed_input == TS_INPUT_NONE) {
        if (!TS_State.touchDetected) return TS_INPUT_NONE;

        pressed_time = HAL_GetTick();
        x1 = TS_State.touchX[0];
        y1 = TS_State.touchY[0];

        //Next Song
        if (y1 < UI_NEXT_Y + (UI_NEXT_S/2) + UI_TS_LEEWAY &&
            y1 > UI_NEXT_Y - (UI_NEXT_S/2) + UI_TS_LEEWAY &&
            x1 < UI_NEXT_X + (UI_NEXT_S/2) + UI_TS_LEEWAY &&
            x1 > UI_NEXT_X - (UI_NEXT_S/2) + UI_TS_LEEWAY){
            pressed_input = TS_INPUT_SKIP;
        }
        //volume up
        if ((y1 < UI_VOL_Y + UI_VOL_R + UI_TS_LEEWAY) &&
            (y1 > UI_VOL_Y - UI_VOL_R + UI_TS_LEEWAY) &&
            (x1 < UI_VOL_UP_X + UI_VOL_R + UI_TS_LEEWAY) &&
            (x1 > UI_VOL_UP_X - UI_VOL_R + UI_TS_LEEWAY)) {
            pressed_input = TS_INPUT_VOL_UP;
        }
        //volume down
        if ((y1 < UI_VOL_Y + UI_VOL_R + UI_TS_LEEWAY) &&
            (y1 > UI_VOL_Y - UI_VOL_R + UI_TS_LEEWAY) &&
            (x1 < UI_VOL_DN_X + UI_VOL_R + UI_TS_LEEWAY) &&
            (x1 > UI_VOL_DN_X - UI_VOL_R + UI_TS_LEEWAY)) {
            pressed_input = TS_INPUT_VOL_DOWN;
        }

        //Pause and Play button
        if (y1 < UI_PAUSE_PLAY_Y + (UI_PAUSE_PLAY_S/2) + UI_TS_LEEWAY &&
            y1 > UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2) + UI_TS_LEEWAY &&
            x1 < UI_PAUSE_PLAY_X + (UI_PAUSE_PLAY_S/2) + UI_TS_LEEWAY &&
            x1 > UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2) + UI_TS_LEEWAY) {
            pressed_input = TS_INPUT_PAUSE_PLAY;
        }

        return TS_INPUT_NONE;

    } else {
        // touch has been released, if enough time has passed (the touch lasted
        // long enough) return the corresponding value, otherwise, ignore the
        // touch
        if (TS_State.touchDetected) return TS_INPUT_NONE;
        TS_Input ret_val = pressed_input;
        pressed_input = TS_INPUT_NONE;
        if (HAL_GetTick() - pressed_time < 25) return TS_INPUT_NONE;
        return ret_val;
    }
}

void LCD_DrawVolDown(void) {
    BSP_LCD_SetTextColor(LCD_FG);
    BSP_LCD_DrawCircle(UI_VOL_DN_X, UI_VOL_Y, UI_VOL_R);
    BSP_LCD_DrawHLine(UI_VOL_DN_X - UI_VOL_R + UI_VOL_IGAP, UI_VOL_Y, UI_VOL_LINE);
}

void LCD_DrawVolUp(void) {
    BSP_LCD_SetTextColor(LCD_FG);
    BSP_LCD_DrawCircle(UI_VOL_UP_X, UI_VOL_Y, UI_VOL_R);
    BSP_LCD_DrawHLine(UI_VOL_UP_X - UI_VOL_R + UI_VOL_IGAP, UI_VOL_Y, UI_VOL_LINE);
    BSP_LCD_DrawVLine(UI_VOL_UP_X, UI_VOL_Y - UI_VOL_R + UI_VOL_IGAP, UI_VOL_LINE);
}

void LCD_DrawVol(void) {
    BSP_LCD_SetTextColor(LCD_FG);
    char buf[10] = {0};
    snprintf(buf, sizeof(buf), "VOL: %3ld", Music_GetVolume());
    BSP_LCD_DisplayStringAt(0, 570, (uint8_t *)buf, CENTER_MODE);
}

void LCD_DrawPlay(void) {
    // equilateral triangle
    const Point points[] = {
        {.X = UI_PAUSE_PLAY_X - (uint16_t)((UI_PAUSE_PLAY_S*SQRT_3)/4),
         .Y = UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X + (uint16_t)((UI_PAUSE_PLAY_S*SQRT_3)/4),
         .Y = UI_PAUSE_PLAY_Y},
        {.X = UI_PAUSE_PLAY_X - (uint16_t)((UI_PAUSE_PLAY_S*SQRT_3)/4),
         .Y = UI_PAUSE_PLAY_Y + (UI_PAUSE_PLAY_S/2)},
    };

    BSP_LCD_SetTextColor(LCD_BG);
    BSP_LCD_FillRect(UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2) - 1,
                     UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2) - 1,
                     UI_PAUSE_PLAY_S + 2, UI_PAUSE_PLAY_S + 2);
    BSP_LCD_SetTextColor(LCD_FG);
    BSP_LCD_FillPolygon((pPoint)points, sizeof(points) / sizeof(points[0]));
}

void LCD_DrawPause(void) {
    // two rectangles next to each other
    const Point points1[] = {
        {.X = UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2),
         .Y = UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2) + (UI_PAUSE_PLAY_S/3),
         .Y = UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2) + (UI_PAUSE_PLAY_S/3),
         .Y = UI_PAUSE_PLAY_Y + (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2),
         .Y = UI_PAUSE_PLAY_Y + (UI_PAUSE_PLAY_S/2)},
    };
    const Point points2[] = {
        {.X = UI_PAUSE_PLAY_X + (UI_PAUSE_PLAY_S/2) - (UI_PAUSE_PLAY_S/3),
         .Y = UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X + (UI_PAUSE_PLAY_S/2),
         .Y = UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X + (UI_PAUSE_PLAY_S/2),
         .Y = UI_PAUSE_PLAY_Y + (UI_PAUSE_PLAY_S/2)},
        {.X = UI_PAUSE_PLAY_X + (UI_PAUSE_PLAY_S/2) - (UI_PAUSE_PLAY_S/3),
         .Y = UI_PAUSE_PLAY_Y + (UI_PAUSE_PLAY_S/2)},
    };

    BSP_LCD_SetTextColor(LCD_BG);
    BSP_LCD_FillRect(UI_PAUSE_PLAY_X - (UI_PAUSE_PLAY_S/2) - 1,
                     UI_PAUSE_PLAY_Y - (UI_PAUSE_PLAY_S/2) - 1,
                     UI_PAUSE_PLAY_S + 2, UI_PAUSE_PLAY_S + 2);
    BSP_LCD_SetTextColor(LCD_FG);
    BSP_LCD_FillPolygon((pPoint)points1, sizeof(points1) / sizeof(points1[0]));
    BSP_LCD_FillPolygon((pPoint)points2, sizeof(points2) / sizeof(points2[0]));
}

void LCD_DrawNext(void) {
    // equilateral triangle and rectangle overlapping
    const Point points1[] = {
        {.X = UI_NEXT_X - (uint16_t)((UI_NEXT_S*SQRT_3)/4),
         .Y = UI_NEXT_Y - (UI_NEXT_S/2)},
        {.X = UI_NEXT_X + (uint16_t)((UI_NEXT_S*SQRT_3)/4),
         .Y = UI_NEXT_Y},
        {.X = UI_NEXT_X - (uint16_t)((UI_NEXT_S*SQRT_3)/4),
         .Y = UI_NEXT_Y + (UI_NEXT_S/2)},
    };
    const Point points2[] = {
        {.X = UI_NEXT_X + (UI_NEXT_S/2) - (UI_NEXT_S/4),
         .Y = UI_NEXT_Y - (UI_NEXT_S/2)},
        {.X = UI_NEXT_X + (UI_NEXT_S/2),
         .Y = UI_NEXT_Y - (UI_NEXT_S/2)},
        {.X = UI_NEXT_X + (UI_NEXT_S/2),
         .Y = UI_NEXT_Y + (UI_NEXT_S/2)},
        {.X = UI_NEXT_X + (UI_NEXT_S/2) - (UI_NEXT_S/4),
         .Y = UI_NEXT_Y + (UI_NEXT_S/2)},
    };

    BSP_LCD_FillPolygon((pPoint)points1, sizeof(points1) / sizeof(points1[0]));
    BSP_LCD_FillPolygon((pPoint)points2, sizeof(points2) / sizeof(points2[0]));
}
