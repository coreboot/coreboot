#include <video_subr.h>
# error the video display code has not been tested

// kludgy but this is only used here ...
static char *vidmem;		/* The video buffer, should be replaced by symbol in ldscript.ld */
static int video_line, video_col;

#define LINES  	25		/* Number of lines and   */
#define COLS   	80		/*   columns on display  */
#define VIDBUFFER 0x20000;

void video_init(void)
{
	video_line = 0;
	video_col = 0;
	vidmem = (char *) VIDBUFFER;
	memset(vidmem, 0, 64*1024);
}
static void video_scroll(void)
{
	int i;

	memcpy(vidmem, vidmem + COLS * 2, (LINES - 1) * COLS * 2);
	for (i = (LINES - 1) * COLS * 2; i < LINES * COLS * 2; i += 2)
		vidmem[i] = ' ';
}

void video_tx_byte(unsigned char byte)
{
	if (byte == '\n') {
		video_line++;
	}
	else if (byte == '\r') {
		video_col = 0;
	}
	else {
		videmem[((video_col + (video_line *COLS)) * 2)] = byte;
		videmem[((video_col + (video_line *COLS)) * 2) +1] = 0x07;
		video_col++;
	}
	if (video_col >= COLS) {
		video_line++;
		video_col = 0;
	}
	if (video_line >= LINES) {
		video_scroll();
		video_line--;
	}
}
