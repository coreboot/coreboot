/* small console implementation */
/*
 * Copyright (C) 2003 Stefan Reinauer
 *
 * See the file "COPYING" for further information about
 * the copyright and warranty status of this work.
 */

#include <string.h>

#include <config.h>
#include <types.h>
#include <io.h>
#ifdef DEBUG_CONSOLE

/* ******************************************************************
 *                       serial console functions
 * ****************************************************************** */

#ifdef DEBUG_CONSOLE_SERIAL

#define RBR(x)  x==2?0x2f8:0x3f8
#define THR(x)  x==2?0x2f8:0x3f8
#define IER(x)  x==2?0x2f9:0x3f9
#define IIR(x)  x==2?0x2fa:0x3fa
#define LCR(x)  x==2?0x2fb:0x3fb
#define MCR(x)  x==2?0x2fc:0x3fc
#define LSR(x)  x==2?0x2fd:0x3fd
#define MSR(x)  x==2?0x2fe:0x3fe
#define SCR(x)  x==2?0x2ff:0x3ff
#define DLL(x)  x==2?0x2f8:0x3f8
#define DLM(x)  x==2?0x2f9:0x3f9

static int uart_charav(int port)
{
	if (!port)
		return -1;
	return ((inb(LSR(port)) & 1) != 0);
}

static char uart_getchar(int port)
{
	if (!port)
		return -1;
	while (!uart_charav(port));
	return ((char) inb(RBR(port)) & 0177);
}

static void uart_putchar(int port, unsigned char c)
{
	if (!port)
		return;
	if (c == '\n')
		uart_putchar(port, '\r');
	while (!(inb(LSR(port)) & 0x20));
	outb(THR(port), c);
}

static void uart_init_line(int port, unsigned long baud)
{
	int i, baudconst;

	if (!port)
		return;

	switch (baud) {
	case 115200:
		baudconst = 1;
		break;
	case 57600:
		baudconst = 2;
		break;
	case 38400:
		baudconst = 3;
		break;
	case 19200:
		baudconst = 6;
		break;
	case 9600:
	default:
		baudconst = 12;
		break;
	}

	outb(LCR(port), 0x87);
	outb(DLM(port), 0);
	outb(DLL(port), baudconst);
	outb(LCR(port), 0x07);
	outb(MCR(port), 0x0f);

	for (i = 10; i > 0; i--) {
		if (inb(LSR(port)) == (unsigned int) 0)
			break;
		inb(RBR(port));
	}
}

int uart_init(int port, unsigned long speed)
{
	if (port)
		uart_init_line(port, speed);
	return -1;
}

static void serial_putchar(int c)
{
	uart_putchar(SERIAL_PORT, (unsigned char) (c & 0xff));
}

static void serial_cls(void)
{
	serial_putchar(27);
	serial_putchar('[');
	serial_putchar('H');
	serial_putchar(27);
	serial_putchar('[');
	serial_putchar('J');
}

#endif

/* ******************************************************************
 *          simple polling video/keyboard console functions
 * ****************************************************************** */

#ifdef DEBUG_CONSOLE_VGA

/* raw vga text mode */
#define COLUMNS			80	/* The number of columns.  */
#define LINES			25	/* The number of lines.  */
#define ATTRIBUTE		7	/* The attribute of an character.  */

#define VGA_BASE		0xB8000	/* The video memory address.  */

/* VGA Index and Data Registers */
#define VGA_REG_INDEX    0x03D4	    /* VGA index register */
#define VGA_REG_DATA     0x03D5     /* VGA data register */

#define VGA_IDX_CURMSL   0x09       /* cursor maximum scan line */
#define VGA_IDX_CURSTART 0x0A       /* cursor start */
#define VGA_IDX_CUREND   0x0B       /* cursor end */
#define VGA_IDX_CURLO    0x0F       /* cursor position (low 8 bits)*/
#define VGA_IDX_CURHI    0x0E       /* cursor position (high 8 bits) */

/* Save the X and Y position.  */
static int xpos, ypos;
/* Point to the video memory.  */
static volatile unsigned char *const video = (unsigned char *) VGA_BASE;

static void video_initcursor(void)
{
	u8 val;
	outb(VGA_REG_INDEX, VGA_IDX_CURMSL);
	val = inb(VGA_REG_DATA) & 0x1f; /* maximum scan line -1 */
	
	outb(VGA_REG_INDEX, VGA_IDX_CURSTART);
	outb(VGA_REG_DATA, 0);

	outb(VGA_REG_INDEX, VGA_IDX_CUREND);
	outb(VGA_REG_DATA, val);
}



static void video_poscursor(unsigned int x, unsigned int y)
{
	unsigned short pos;

	/* Calculate new cursor position as a function of x and y */
	pos = (y*COLUMNS) + x;

	/* Output the new position to VGA card */
	outb(VGA_REG_INDEX, VGA_IDX_CURLO);   /* output low 8 bits */
	outb(VGA_REG_DATA,  (u8) (pos));
	outb(VGA_REG_INDEX, VGA_IDX_CURHI);   /* output high 8 bits */
	outb(VGA_REG_DATA,  (u8) (pos >> 8));

};


static void video_newline(void)
{
	xpos = 0;

	if (ypos < LINES-1) {
		ypos++;
	} else {
		int i;
		memmove((void *)video, (void *)(video+2*COLUMNS), (LINES-1)*COLUMNS*2);

		for(i=( (LINES-1) *2*COLUMNS ); i<2*COLUMNS*LINES;) {
			video[i++]=0;
			video[i++]=ATTRIBUTE;
		}
	}

}

/* Put the character C on the screen.  */
static void video_putchar(int c)
{
	if (c == '\n' || c == '\r') {
		video_newline();
		return;
	}

	if (xpos >= COLUMNS)
		video_newline();

	*(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

	xpos++;

	video_poscursor(xpos, ypos);
}

static void video_cls(void)
{	
	int i;
	for(i=0; i<2*COLUMNS*LINES;) {
		video[i++]=0;
		video[i++]=ATTRIBUTE;
	}


	xpos = 0; ypos = 0;

	video_initcursor();
	video_poscursor(xpos,ypos);
}

/*
 *  keyboard driver
 */

static char normal[] = {
	0x0, 0x1b, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',
	'=', '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o',
	'p', '[', ']', 0xa, 0x0, 'a', 's', 'd', 'f', 'g', 'h', 'j',
	'k', 'l', ';', 0x27, 0x60, 0x0, 0x5c, 'z', 'x', 'c', 'v', 'b',
	'n', 'm', ',', '.', '/', 0x0, '*', 0x0, ' ', 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, '0', 0x7f
};

static char shifted[] = {
	0x0, 0x1b, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',
	'+', '\b', '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O',
	'P', '{', '}', 0xa, 0x0, 'A', 'S', 'D', 'F', 'G', 'H', 'J',
	'K', 'L', ':', 0x22, '~', 0x0, '|', 'Z', 'X', 'C', 'V', 'B',
	'N', 'M', '<', '>', '?', 0x0, '*', 0x0, ' ', 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, '7', '8',
	'9', 0x0, '4', '5', '6', 0x0, '1', '2', '3', '0', 0x7f
};

static int key_ext;
static int key_lshift = 0, key_rshift = 0, key_caps = 0;

static char last_key;

static void keyboard_cmd(unsigned char cmd, unsigned char val)
{
	outb(0x60, cmd);
	/* wait until keyboard controller accepts cmds: */
	while (inb(0x64) & 2);
	outb(0x60, val);
	while (inb(0x64) & 2);
}

static char keyboard_poll(void)
{
	unsigned int c;
	if (inb(0x64) & 1) {
		c = inb(0x60);
		switch (c) {
		case 0xe0:
			key_ext = 1;
			return 0;
		case 0x2a:
			key_lshift = 1;
			return 0;
		case 0x36:
			key_rshift = 1;
			return 0;
		case 0xaa:
			key_lshift = 0;
			return 0;
		case 0xb6:
			key_rshift = 0;
			return 0;
		case 0x3a:
			if (key_caps) {
				key_caps = 0;
				keyboard_cmd(0xed, 0);
			} else {
				key_caps = 1;
				keyboard_cmd(0xed, 4);	/* set caps led */
			}
			return 0;
		}

		if (key_ext) {
			void printk(const char *format, ...);
			printk("extended keycode: %x\n", c);

			key_ext = 0;
			return 0;
		}

		if (c & 0x80)	/* unhandled key release */
			return 0;

		if (key_lshift || key_rshift)
			return key_caps ? normal[c] : shifted[c];
		else
			return key_caps ? shifted[c] : normal[c];
	}
	return 0;
}

static int keyboard_dataready(void)
{
	if (last_key)
		return 1;

	last_key = keyboard_poll();

	return (last_key != 0);
}

static unsigned char keyboard_readdata(void)
{
	char tmp;
	while (!keyboard_dataready());
	tmp = last_key;
	last_key = 0;
	return tmp;
}
#endif


/* ******************************************************************
 *      common functions, implementing simple concurrent console
 * ****************************************************************** */

int putchar(int c)
{
#ifdef DEBUG_CONSOLE_SERIAL
	serial_putchar(c);
#endif
#ifdef DEBUG_CONSOLE_VGA
	video_putchar(c);
#endif
	return c;
}

int availchar(void)
{
#ifdef DEBUG_CONSOLE_SERIAL
	if (uart_charav(SERIAL_PORT))
		return 1;
#endif
#ifdef DEBUG_CONSOLE_VGA
	if (keyboard_dataready())
		return 1;
#endif
	return 0;
}

int getchar(void)
{
#ifdef DEBUG_CONSOLE_SERIAL
	if (uart_charav(SERIAL_PORT))
		return (uart_getchar(SERIAL_PORT));
#endif
#ifdef DEBUG_CONSOLE_VGA
	if (keyboard_dataready())
		return (keyboard_readdata());
#endif
	return 0;
}

void cls(void)
{
#ifdef DEBUG_CONSOLE_SERIAL
	serial_cls();
#endif
#ifdef DEBUG_CONSOLE_VGA
	video_cls();
#endif
}


#endif				// DEBUG_CONSOLE
