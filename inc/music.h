/* clang-format off */

#pragma once

#include "ff.h"
#include <stdbool.h>
#include <stdint.h>

/*
** Initialize the audio output using the BSP
** Returns 'true' if initialization happens successfully
*/
bool Music_Init(void);

/*
** Start playing the music data found in 'file'
** Returns 'true' if music starts successfully
*/
bool Music_Start(FIL *file);

/*
** Stops the music that is currently being played
*/
void Music_Stop(void);

/*
** Pauses/Resumes the music that is currently being played
*/
void Music_PauseResume(void);

/*
** Returns 'true' if the music is currently paused
*/
bool Music_IsPaused(void);

/*
** Increases the volume by 5%
*/
void Music_IncreaseVolume(void);

/*
** Decreases the volume by 5%
*/
void Music_DecreaseVolume(void);

/*
** Returns the current volume
*/
uint32_t Music_GetVolume(void);

/*
** Does all processing necessary to keep music playing
** Returns 'true' if should play the next song
*/
bool Music_Process(void);
