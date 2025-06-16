/* clang-format off */

#pragma once

#include "ff.h"

#include <stdbool.h>

/*
** Initializes everything needed for displaying the album cover
** Returns 'true' if everything intialized correctly
*/
bool Cover_Init(void);

/*
** Displays the jpeg image in 'file' in the center of the LCD screen
** Returns 'true' if everything initializes correctly
*/
bool Cover_Display(FIL *file);
