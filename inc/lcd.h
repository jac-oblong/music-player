/* clang-format off */

#pragma once

#include <stdbool.h>

typedef enum {
    TS_INPUT_NONE,
    TS_INPUT_PAUSE_PLAY,
    TS_INPUT_SKIP,
    TS_INPUT_VOL_UP,
    TS_INPUT_VOL_DOWN
} TS_Input;

/*
** Initializes everything needed for LCD and TS
** Returns 'true' if everything intialized correctly
*/
bool LCD_Init(void);

/*
** Displays the song title
** Returns 'true' if everything initializes correctly
*/
bool LCD_SongTitle(char *title);

/*
** Displays the song artist
** Returns 'true' if everything initializes correctly
*/
bool LCD_SongArtist(char *artist);

/*
** Updates the displayed volume
*/
void LCD_DrawVol(void);

/*
** Displays the play symbol
*/
void LCD_DrawPlay(void);

/*
** Displays the pause symbol
*/
void LCD_DrawPause(void);

/*
** Returns enum value of user input
*/
TS_Input LCD_GetUserInput(void);
