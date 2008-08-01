/*
 *
 * modified from original freebios code
 * by Steve M. Gehlbach <steve@kesa.com>
 *
 */

#include <arch/io.h>
#include <string.h>
#include <pc80/vga.h>
#include <console/console.h>

/* The video buffer, should be replaced by symbol in ldscript.ld */
static char *vidmem;

int vga_line, vga_col;

int vga_inited = 0; // it will be changed in pci_rom.c

static int vga_console_inited = 0;

#define VIDBUFFER 0xB8000;

static void memsetw(void *s, int c, unsigned int n)
{
	int i;
	 u16 *ss = (u16 *) s;

	for (i = 0; i < n; i++) {
		ss[i] = ( u16 ) c;
	}
}

static void vga_init(void)
{
	// these are globals
	vga_line = 0;
	vga_col = 0;
	vidmem = (char *) VIDBUFFER;
	
	// mainboard or chip specific init routines
	// also loads font
	vga_hardware_fixup();
	
	// set attributes, char for entire screen
	// font should be previously loaded in 
	// device specific code (vga_hardware_fixup)
	 memsetw(vidmem, VGA_ATTR_CLR_WHT, 2*1024); //
}

static void vga_scroll(void)
{
	int i;

	memcpy(vidmem, vidmem + COLS * 2, (LINES - 1) * COLS * 2);
	for (i = (LINES - 1) * COLS * 2; i < LINES * COLS * 2; i += 2)
		vidmem[i] = ' ';
}

static void vga_tx_byte(unsigned char byte)
{
	if (!vga_inited) {
		return;
	}
 
	if(!vga_console_inited) {
		vga_init();
		vga_console_inited = 1;
	}

	if (byte == '\n') {
		vga_line++;
		vga_col = 0;

	} else if (byte == '\r') {
		vga_col = 0;

	} else if (byte == '\b') {
		vga_col--;

	} else if (byte == '\t') {
		vga_col += 4;

	} else if (byte == '\a') {
		//beep
//		beep(500);
		;
	} else {
		vidmem[((vga_col + (vga_line *COLS)) * 2)] = byte;
		vidmem[((vga_col + (vga_line *COLS)) * 2) +1] = VGA_ATTR_CLR_WHT;
		vga_col++;
	}
	if (vga_col < 0) {
		vga_col = 0;
	}
	if (vga_col >= COLS) {
		vga_line++;
		vga_col = 0;
	}
	if (vga_line >= LINES) {
		vga_scroll();
		vga_line--;
	}
	// move the cursor
	write_crtc((vga_col + (vga_line *COLS)) >> 8, CRTC_CURSOR_HI);
	write_crtc((vga_col + (vga_line *COLS)) & 0x0ff, CRTC_CURSOR_LO);
}

static const struct console_driver vga_console __console ={
	.init    = 0,
	.tx_byte = vga_tx_byte,
	.rx_byte = 0,
	.tst_byte = 0,
};
