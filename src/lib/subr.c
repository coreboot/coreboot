/*
 * Bootstrap code for the INTEL 
 * $Id$
 *
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif

#include <arch/io.h>

#include <printk.h>
#include <pci.h>
#include <subr.h>
#include <string.h>

#ifdef SERIAL_CONSOLE
#include <serial_subr.h>
#endif

#ifdef VIDEO_BIOS_WORKS

# error the video display code has not been tested

// kludgy but this is only used here ...
static char *vidmem;		/* The video buffer, should be replaced by symbol in ldscript.ld */
static int video_line, video_col;

#define LINES  	25		/* Number of lines and   */
#define COLS   	80		/*   columns on display  */
#define VIDBUFFER 0x20000;

static void video_init(void)
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

static void video_tx_byte(unsigned char byte)
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
#endif /* VIDEO_BIOS_WORKS */

// initialize the display
void displayinit(void)
{
#ifdef VIDEO_BIOS_WORKS
	video_init();
#endif
#ifdef SERIAL_CONSOLE
	ttys0_init();
#endif
}

void display_tx_byte(unsigned char byte)
{
#ifdef VIDEO_BIOS_WORKS
	video_tx_byte(byte);
#endif
#ifdef SERIAL_CONSOLE
	ttys0_tx_byte(byte);
#endif
#ifdef SROM_CONSOLE
	srom_tx_byte(byte);
#endif
}

void display(char *string)
{
	while(*string) {
		if (*string == '\n') {
			display_tx_byte('\r');
		}
		display_tx_byte(*string);
		string++;
	}
}

void error(char errmsg[])
{
	display(errmsg);
	while (1);		/* Halt */
}

/*
 *    Write POST information
 */
void post_code(uint8_t value)
{
	unsigned long hi, lo;
	outb(value, 0x80);
#ifdef SERIAL_POST
	rdtsc(lo, hi);
	printk(KERN_INFO "POST: 0x%02x, TSC Lo: %d, Hi: %d\n",
	       value, lo, hi);
#endif
}

