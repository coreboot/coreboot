/*
 *
 * modified from original freebios code
 * by Steve M. Gehlbach <steve@kesa.com>
 *
 */

/*
 * TODO:
 * * make vga_console_init take FB location, columns, lines and starting
 *   column/line.
 * * track a word offset, and not columns/lines. The offset is needed more
 *   often than columns/lines and the latter two can be calculated easily.
 * * then implement real vga scrolling, instead of memcpying stuff around.
 *
 * -- libv.
 */

#include <arch/io.h>
#include <string.h>
#include <pc80/vga_io.h>
#include <pc80/vga.h>
#include <console/console.h>

/* The video buffer, should be replaced by symbol in ldscript.ld */
static char *vidmem;
static int total_lines, total_columns;
static int current_line, current_column;
static int vga_console_inited = 0;

/*
 *
 */
void vga_console_init(void)
{
	vidmem = (char *) VGA_FB;
	total_columns = VGA_COLUMNS;
	total_lines = VGA_LINES;
	current_column = 0;
	current_line = 0;

	vga_console_inited = 1;
}

static void vga_scroll(void)
{
	int i;

	memcpy(vidmem, vidmem + total_columns * 2, (total_lines - 1) * total_columns * 2);
	for (i = (total_lines - 1) * total_columns * 2; i < total_lines * total_columns * 2; i += 2)
		vidmem[i] = ' ';
}

static void
vga_tx_byte(unsigned char byte)
{
	if (!vga_console_inited)
	    return;

	switch (byte) {
	case '\n':
		current_line++;
		current_column = 0;
		break;
	case '\r':
		current_column = 0;
		break;
	case '\b':
		current_column--;
		break;
	case '\t':
		current_column += 4;
		break;
	case '\a': /* beep */
		break;
	default:
		vidmem[((current_column + (current_line * total_columns)) * 2)] = byte;
		vidmem[((current_column + (current_line * total_columns)) * 2) +1] = 0x07;
		current_column++;
		break;
	}

	if (current_column < 0)
		current_column = 0;
	if (current_column >= total_columns) {
		current_line++;
		current_column = 0;
	}
	if (current_line >= total_lines) {
		vga_scroll();
		current_line--;
	}

	/* move the cursor */
	vga_cr_write(0x0E, (current_column + (current_line * total_columns)) >> 8);
	vga_cr_write(0x0F, (current_column + (current_line * total_columns)) & 0x0ff);
}

static const struct console_driver vga_console __console ={
	.init    = 0,
	.tx_byte = vga_tx_byte,
	.rx_byte = 0,
	.tst_byte = 0,
};
