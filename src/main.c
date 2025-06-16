/* clang-format off */

#include "init.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "cover.h"
#include "lcd.h"
#include "music.h"
#include "sd_diskio.h"
#include <stdio.h>
#include <string.h>

FATFS sdFatFs;

static void play_song(FILINFO *file_info);
static void display_title_and_artist(FIL *meta);
static uint32_t str_end_on_nl(char *str);
static void str_add_dots(char *str, int len);

int main(void){
	Sys_Init();

	printf("\033[2J\033[;H");
	printf("\033c");
	fflush(stdout);

	// Initialize peripherals
	Music_Init();
	Cover_Init();
	LCD_Init();

    // Link FATFS Driver
    FATFS_LinkDriver(&SD_Driver, "0:/");

    // Mount the disk
    f_mount(&sdFatFs, "0:/", 0);

	// Go through files in root directory
    FRESULT res;
    DIR dir;
    FILINFO file_info;
    res = f_opendir(&dir, "/");
	while (1) {
		res = f_readdir(&dir, &file_info);
		// at end of files, close and reopen directory to get back to beginning
		if (res != FR_OK || file_info.fname[0] == 0) {
			f_closedir(&dir);
			f_opendir(&dir, "/");
			continue;
		}
		// file is not a directory, ignore
		if ((file_info.fattrib & AM_DIR) == 0) continue;

		// file is a directory, play the song in the directory
		play_song(&file_info);
	}
}

void play_song(FILINFO *file_info) {
	char path[strlen(file_info->fname) + 15];
	FIL cover, meta, song;

	// create path for album cover
	strcpy(path, file_info->fname);
	strcat(path, "/cover.jpg");
	// process album cover
	if (f_open(&cover, path, FA_READ) != FR_OK) return;
	Cover_Display(&cover);
	f_close(&cover);

	// display song title and artist
	strcpy(path, file_info->fname);
	strcat(path, "/meta.txt");
	if (f_open(&meta, path, FA_READ) != FR_OK) return;
	display_title_and_artist(&meta);
	f_close(&meta);

	// create path for song
	strcpy(path, file_info->fname);
	strcat(path, "/song.raw");
	// process song
	if (f_open(&song, path, FA_READ) != FR_OK) return;
	Music_Start(&song);

	bool skip = false;
	while (Music_Process() && !skip) {
		switch (LCD_GetUserInput()) {
			case TS_INPUT_NONE: break;
			case TS_INPUT_PAUSE_PLAY:
				Music_PauseResume();
				if (Music_IsPaused()) LCD_DrawPlay();
				else LCD_DrawPause();
				break;
			case TS_INPUT_SKIP: skip = true; break;
			case TS_INPUT_VOL_UP:
				Music_IncreaseVolume();
				LCD_DrawVol();
				break;
			case TS_INPUT_VOL_DOWN:
				Music_DecreaseVolume();
				LCD_DrawVol();
				break;
		}
	}

	if (!Music_IsPaused()) Music_PauseResume();

	f_close(&song);
}

void display_title_and_artist(FIL *meta) {
	char buffer[25] = {0};
	unsigned bytes_read = 0;

	// read in chunk and find first new line (for title)
	f_read(meta, buffer, sizeof(buffer), &bytes_read);
	uint32_t x = str_end_on_nl(buffer);
	str_add_dots(buffer, 20);
	LCD_SongTitle(buffer);

	// seek back number of bytes not used in title
	f_lseek(meta, bytes_read - x);
	// read until new line
	char c = 0;
	while (c != '\n') f_read(meta, &c, sizeof(c), &bytes_read);

	// reset buffer to all 0
	memset(buffer, 0, sizeof(buffer));

	// read in rest, limit song artist to 20 characters
	f_read(meta, buffer, sizeof(buffer), &bytes_read);
	str_end_on_nl(buffer);
	str_add_dots(buffer, 20);
	LCD_SongArtist(buffer);

	// read until end of file
	while (!f_eof(meta)) f_read(meta, &c, sizeof(c), &bytes_read);
	memset(buffer, 0, sizeof(buffer));
}

uint32_t str_end_on_nl(char *str) {
	char *end = strchr(str, '\n');
	if (end == NULL) return 0;

	uint32_t x = strlen(end);
	memset(end, 0, x);
	return x;
}

void str_add_dots(char *str, int len) {
	if (len < 3) return;

	uint32_t slen = strlen(str);
	if (slen < len) return;

	char *end = str + len;
	*end = '\0';
	end -= 3;
	memcpy(end, "...", 3);
}
