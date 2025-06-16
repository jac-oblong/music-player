/* clang-format off */

#include "music.h"

#include "stm32f769i_discovery_audio.h"
#include "ff.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MUSIC_BUFFER_SIZE 2048

// playback volume
static volatile uint32_t music_volume = 20;
// state of the music player
static enum { MUSIC_IDLE, MUSIC_INIT, MUSIC_PLAY, MUSIC_DONE } music_state = MUSIC_IDLE;
// state of pause
static enum { PLAY_RESUMED, PLAY_PAUSED } play_state = PLAY_RESUMED;
// buffer for holding data
static struct {
    uint8_t data[MUSIC_BUFFER_SIZE];
    volatile enum { BUFFER_FULL, BUFFER_FIRST, BUFFER_SECOND } state;
    volatile enum { LAST_NONE, LAST_HALF, LAST_FULL } done;
    FIL *file;
} music_buffer;

/*
** Initialize the audio output using the BSP
** Returns 'true' if initialization happens successfully
*/
bool Music_Init(void) {
    if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, music_volume, AUDIO_FREQUENCY_44K) != AUDIO_OK) {
        return false;
    }

    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);

    music_state = MUSIC_INIT;
    return true;
}

/*
** Start playing the music data found in 'file'
** Returns 'true' if music starts successfully
*/
bool Music_Start(FIL *file) {
    if (music_state == MUSIC_IDLE) {
        return false;
    }

    music_buffer.state = BUFFER_FULL;
    music_buffer.file = file;
    music_buffer.done = LAST_NONE;
    unsigned int bytes_read = 0;
    f_read(music_buffer.file, music_buffer.data, MUSIC_BUFFER_SIZE, &bytes_read);

    if (bytes_read > 0) {
        if (Music_IsPaused()) Music_PauseResume();
        BSP_AUDIO_OUT_Play((uint16_t*)music_buffer.data, MUSIC_BUFFER_SIZE);
        music_state = MUSIC_PLAY;
        play_state = PLAY_RESUMED;
        return true;
    }

    return false;
}

/*
** Stops the music that is currently being played
*/
void Music_Stop(void) {
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    music_state = MUSIC_INIT;
}

/*
** Pauses/Resumes the music that is currently being played
*/
void Music_PauseResume(void) {
    if (play_state == PLAY_RESUMED) {
        BSP_AUDIO_OUT_Pause();
        play_state = PLAY_PAUSED;
    } else {
        BSP_AUDIO_OUT_Resume();
        play_state = PLAY_RESUMED;
    }
}

/*
** Returns 'true' if the music is currently paused
*/
bool Music_IsPaused(void) {
    return play_state == PLAY_PAUSED;
}

/*
** Increases the volume by 5%
*/
void Music_IncreaseVolume(void) {
    if (music_volume <= 95) music_volume += 5;
    else music_volume = 100;
    BSP_AUDIO_OUT_SetVolume(music_volume);
}

/*
** Decreases the volume by 5%
*/
void Music_DecreaseVolume(void) {
    if (music_volume >= 10) music_volume -= 5;
    else music_volume = 5;
    BSP_AUDIO_OUT_SetVolume(music_volume);
}

/*
** Returns the current volume
*/
uint32_t Music_GetVolume(void) {
    return music_volume;
}

/*
** Does all processing necessary to keep music playing
** Returns 'true' if should continue to play the song
*/
bool Music_Process(void) {
    void *buf = NULL;
    unsigned int bytes_read = 0;
    uint16_t last;

    switch (music_state) {
    // currently playing music, read more data if needed
    case MUSIC_PLAY:
        switch (music_buffer.state) {
        // nothing to do, buffer is full
        case BUFFER_FULL: break;
        // the first half of the buffer has been emptied, read some more
        case BUFFER_FIRST:
            buf = &music_buffer.data[0];
            last = LAST_HALF;
            break;
        // the second half of the buffer has been emptied, read some more
        case BUFFER_SECOND:
            buf = &music_buffer.data[MUSIC_BUFFER_SIZE/2];
            last = LAST_FULL;
            break;
        }

        // nothing to read
        if (buf == NULL) break;

        // read data, if not all data was read, check for end of file
        f_read(music_buffer.file, buf, MUSIC_BUFFER_SIZE/2, &bytes_read);
        if (f_eof(music_buffer.file) && music_buffer.done == LAST_NONE) music_buffer.done = last;
        memset(buf+bytes_read, 0, MUSIC_BUFFER_SIZE/2 - bytes_read);
        music_buffer.state = BUFFER_FULL;

        break;

    // done with the song, let know should move to next song
    case MUSIC_DONE:
        return false;

    // nothing to do
    case MUSIC_INIT:
    case MUSIC_IDLE:
        break;
    }

    return true;
}

/*----------------------------------------------------------------------------*/
/*                                                                            */
/* BSP CALLBACKS                                                              */
/*                                                                            */
/*----------------------------------------------------------------------------*/

/*
** Tells Music_Process() to copy more data if applicable, if this is the last
** callback, set the music state to done
*/
void BSP_AUDIO_OUT_TransferComplete_CallBack(void) {
  if (music_state == MUSIC_PLAY) {
      music_buffer.state = BUFFER_SECOND;
      if (music_buffer.done == LAST_FULL) music_state = MUSIC_DONE;
  }
}

/*
** Tells Music_Process() to copy more data if applicable, if this is the last
** callback, set the music state to done
*/
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
  if (music_state == MUSIC_PLAY) {
      music_buffer.state = BUFFER_FIRST;
      if (music_buffer.done == LAST_HALF) music_state = MUSIC_DONE;
  }
}
