/*
 * Procedures for drawing on the screen early on in the boot process.
 *
 * Benjamin Herrenschmidt <benh@kernel.crashing.org>
 *
 *   move to coreboot by LYH yhlu@tyan.com
 */

#if 0

#include <delay.h>
#include <stdlib.h>
#include <string.h>
#include <arch/io.h>

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>

#endif

#include <arch/io.h>
#include <string.h>
#include <console/console.h>


#include <arch/byteorder.h>

#include <console/btext.h>

//#define NO_SCROLL

#ifndef NO_SCROLL
static void scrollscreen(void);
#endif

static void draw_byte(unsigned char c, u32 locX, u32 locY);
#if 0
static void draw_byte_32(unsigned char *bits, u32 *base, u32 rb);
static void draw_byte_16(unsigned char *bits, u32 *base, u32 rb);
#endif
static void draw_byte_8(unsigned char *bits, u32 *base, u32 rb);

static u32 g_loc_X;
static u32 g_loc_Y;
static u32 g_max_loc_X;
static u32 g_max_loc_Y;

#define CHAR_256 0

#if CHAR_256==1
#define cmapsz	(16*256)
#else
#define cmapsz        (16*96)
#endif

extern unsigned char vga_font[cmapsz];

u32 boot_text_mapped;

boot_infos_t disp_bi;

#define BTEXT		
#define BTDATA	


/* This function will enable the early boot text when doing OF booting. This
 * way, xmon output should work too
 */
void
btext_setup_display(u32 width, u32 height, u32 depth, u32 pitch,
		    unsigned long address)
{
	boot_infos_t* bi = &disp_bi;

	g_loc_X = 0;
	g_loc_Y = 0;
	g_max_loc_X = width / 8;
	g_max_loc_Y = height / 16;
//	bi->logicalDisplayBase = (unsigned char *)address;
	bi->dispDeviceBase = (unsigned char *)address;
	bi->dispDeviceRowBytes = pitch;
	bi->dispDeviceDepth = depth;
	bi->dispDeviceRect[0] = bi->dispDeviceRect[1] = 0;
	bi->dispDeviceRect[2] = width;
	bi->dispDeviceRect[3] = height;
	boot_text_mapped = 0;
}

/* Here's a small text engine to use during early boot
 * or for debugging purposes
 *
 * todo:
 *
 *  - build some kind of vgacon with it to enable early printk
 *  - move to a separate file
 *  - add a few video driver hooks to keep in sync with display
 *    changes.
 */

void 
map_boot_text(void)
{
#if 0
	unsigned long base, offset, size;
	boot_infos_t *bi = &disp_bi;

	if (bi->dispDeviceBase == 0)
		return;
	base = ((unsigned long) bi->dispDeviceBase) & 0xFFFFF000UL;
	offset = ((unsigned long) bi->dispDeviceBase) - base;
	size = bi->dispDeviceRowBytes * bi->dispDeviceRect[3] + offset 
		+ bi->dispDeviceRect[0];
	bi->logicalDisplayBase = ioremap(base,0x800000 ); 
	if (bi->logicalDisplayBase == 0)
		return;
//	bi->logicalDisplayBase += offset;
#endif
	boot_text_mapped = 1;
}

/* Calc the base address of a given point (x,y) */
static unsigned char * BTEXT
calc_base(boot_infos_t *bi, u32 x, u32 y)
{
	unsigned char *base;
#if 0
	base = bi->logicalDisplayBase;
	if (base == 0)
#endif
		base = bi->dispDeviceBase;
	base += (x + bi->dispDeviceRect[0]) * (bi->dispDeviceDepth >> 3);
	base += (y + bi->dispDeviceRect[1]) * bi->dispDeviceRowBytes;
	return base;
}

/* Adjust the display to a new resolution */
void
btext_update_display(unsigned long phys, u32 width, u32 height,
		     u32 depth, u32 pitch)
{
	boot_infos_t *bi = &disp_bi;
#if 0
	if (bi->dispDeviceBase == 0)
		return;
	/* check it's the same frame buffer (within 256MB) */
	if ((phys ^ (unsigned long)bi->dispDeviceBase) & 0xf0000000)
		return;
#endif

	bi->dispDeviceBase = (u8 *) phys;
	bi->dispDeviceRect[0] = 0;
	bi->dispDeviceRect[1] = 0;
	bi->dispDeviceRect[2] = width;
	bi->dispDeviceRect[3] = height;
	bi->dispDeviceDepth = depth;
	bi->dispDeviceRowBytes = pitch;
	if (boot_text_mapped) {
#if 0
		iounmap(bi->logicalDisplayBase);
#endif
		boot_text_mapped = 0;
	}
	map_boot_text();
	g_loc_X = 0;
	g_loc_Y = 0;
	g_max_loc_X = width / 8;
	g_max_loc_Y = height / 16;
}

void BTEXT btext_clearscreen(void)
{
	boot_infos_t* bi	= &disp_bi;
	u32 *base	= (u32 *)calc_base(bi, 0, 0);
	u32 width 	= ((bi->dispDeviceRect[2] - bi->dispDeviceRect[0]) *
					(bi->dispDeviceDepth >> 3)) >> 2;
	u32 i,j;

	for (i=0; i<(bi->dispDeviceRect[3] - bi->dispDeviceRect[1]); i++)
	{
		u32 *ptr = base;
		for(j=width; j; --j)
			*(ptr++) = 0;
		base += (bi->dispDeviceRowBytes >> 2);
	}
}

#if 0
__inline__ void dcbst(const void* addr)
{
	__asm__ __volatile__ ("dcbst 0,%0" :: "r" (addr));
}

void BTEXT btext_flushscreen(void)
{
	boot_infos_t* bi	= &disp_bi;
	u32  *base	= (unsigned long *)calc_base(bi, 0, 0);
	u32 width 	= ((bi->dispDeviceRect[2] - bi->dispDeviceRect[0]) *
					(bi->dispDeviceDepth >> 3)) >> 2;
	u32 i,j;

	for (i=0; i<(bi->dispDeviceRect[3] - bi->dispDeviceRect[1]); i++)
	{
		u32 *ptr = base;
		for(j=width; j>0; j-=8) {
			dcbst(ptr);
			ptr += 8;
		}
		base += (bi->dispDeviceRowBytes >> 2);
	}
}
#endif


#ifndef NO_SCROLL
static BTEXT void
scrollscreen(void)
{
	boot_infos_t* bi		= &disp_bi;
	u32 *src		= (u32 *)calc_base(bi,0,16);
	u32 *dst		= (u32 *)calc_base(bi,0,0);
	u32 width		= ((bi->dispDeviceRect[2] - bi->dispDeviceRect[0]) *
						(bi->dispDeviceDepth >> 3)) >> 2;
	u32 i,j;

	for (i=0; i<(bi->dispDeviceRect[3] - bi->dispDeviceRect[1] - 16); i++)
	{
		u32 *src_ptr = src;
		u32 *dst_ptr = dst;
		for(j=width; j; --j)
			*(dst_ptr++) = *(src_ptr++);
		src += (bi->dispDeviceRowBytes >> 2);
		dst += (bi->dispDeviceRowBytes >> 2);
	}
	for (i=0; i<16; i++)
	{
		u32 *dst_ptr = dst;
		for(j=width; j; --j)
			*(dst_ptr++) = 0;
		dst += (bi->dispDeviceRowBytes >> 2);
	}
}
#endif /* ndef NO_SCROLL */

void BTEXT btext_drawchar(char c)
{
	u32 cline = 0;

	if (!boot_text_mapped)
		return;

	switch (c) {
	case '\b':
		if (g_loc_X > 0)
			--g_loc_X;
		break;
	case '\t':
		g_loc_X = (g_loc_X & -8) + 8;
		break;
	case '\r':
		g_loc_X = 0;
		break;
	case '\n':
		g_loc_X = 0;
		g_loc_Y++;
		cline = 1;
		break;
	default:
		draw_byte(c, g_loc_X++, g_loc_Y);
	}
	if (g_loc_X >= g_max_loc_X) {
		g_loc_X = 0;
		g_loc_Y++;
		cline = 1;
	}
#ifndef NO_SCROLL
	while (g_loc_Y >= g_max_loc_Y) {
		scrollscreen();
		g_loc_Y--;
	}
#else
	/* wrap around from bottom to top of screen so we don't
	   waste time scrolling each line.  -- paulus. */
	if (g_loc_Y >= g_max_loc_Y)
		g_loc_Y = 0;
	if (cline) {
		for (x = 0; x < g_max_loc_X; ++x)
			draw_byte(' ', x, g_loc_Y);
	}
#endif
}
#if 0
void BTEXT
btext_drawstring(const char *c)
{
	if (!boot_text_mapped)
		return;
	while (*c)
		btext_drawchar(*c++);
}

void BTEXT
btext_drawhex(u32 v)
{
	static char hex_table[] = "0123456789abcdef";

	if (!boot_text_mapped)
		return;
	btext_drawchar(hex_table[(v >> 28) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >> 24) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >> 20) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >> 16) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >> 12) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >>  8) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >>  4) & 0x0000000FUL]);
	btext_drawchar(hex_table[(v >>  0) & 0x0000000FUL]);
	btext_drawchar(' ');
}
#endif
static void BTEXT
draw_byte(unsigned char c, u32 locX, u32 locY)
{
	boot_infos_t* bi	= &disp_bi;
	unsigned char *base	= calc_base(bi, locX << 3, locY << 4);
#if CHAR_256==1
        unsigned char *font     = &vga_font[((u32)c) * 16];
#else
	unsigned char *font	= &vga_font[((u32)c-0x20) * 16]; // skip the first 0x20
#endif
	u32 rb			= bi->dispDeviceRowBytes;

	switch(bi->dispDeviceDepth) {
#if 0
	case 24:
	case 32:
		draw_byte_32(font, (u32 *)base, rb);
		break;
	case 15:
	case 16:
		draw_byte_16(font, (u32 *)base, rb);
		break;
#endif
	case 8:
		draw_byte_8(font, (u32 *)base, rb);
		break;
	}
}
static u32 expand_bits_8[16] BTDATA = {
#if defined(__BIG_ENDIAN)
    0x00000000,0x000000ff,0x0000ff00,0x0000ffff,
    0x00ff0000,0x00ff00ff,0x00ffff00,0x00ffffff,
    0xff000000,0xff0000ff,0xff00ff00,0xff00ffff,
    0xffff0000,0xffff00ff,0xffffff00,0xffffffff
#elif defined(__LITTLE_ENDIAN)
    0x00000000,0xff000000,0x00ff0000,0xffff0000,
    0x0000ff00,0xff00ff00,0x00ffff00,0xffffff00,
    0x000000ff,0xff0000ff,0x00ff00ff,0xffff00ff,
    0x0000ffff,0xff00ffff,0x00ffffff,0xffffffff
#else
#error FIXME: No endianness??
#endif                      
};
#if 0
static const u32 expand_bits_16[4] BTDATA = {
#if defined(__BIG_ENDIAN)
    0x00000000, 0x0000ffff, 0xffff0000, 0xffffffff
#elif defined(__LITTLE_ENDIAN)
    0x00000000, 0xffff0000, 0x0000ffff, 0xffffffff
#else
#error FIXME: No endianness??
#endif
};
#endif
#if 0
static void BTEXT
draw_byte_32(unsigned char *font, u32 *base, u32 rb)
{
	u32 l, bits;
	u32 fg = 0xFFFFFFFF;
	u32 bg = 0x00000000;

	for (l = 0; l < 16; ++l)
	{
		bits = *font++;
		base[0] = (-(bits >> 7) & fg) ^ bg;
		base[1] = (-((bits >> 6) & 1) & fg) ^ bg;
		base[2] = (-((bits >> 5) & 1) & fg) ^ bg;
		base[3] = (-((bits >> 4) & 1) & fg) ^ bg;
		base[4] = (-((bits >> 3) & 1) & fg) ^ bg;
		base[5] = (-((bits >> 2) & 1) & fg) ^ bg;
		base[6] = (-((bits >> 1) & 1) & fg) ^ bg;
		base[7] = (-(bits & 1) & fg) ^ bg;
		base = (u32 *) ((char *)base + rb);
	}
}

static void BTEXT
draw_byte_16(unsigned char *font, u32 *base, u32 rb)
{
	u32 l, bits;
	u32 fg = 0xFFFFFFFF;
	u32 bg = 0x00000000;
	u32 *eb = expand_bits_16;

	for (l = 0; l < 16; ++l)
	{
		bits = *font++;
		base[0] = (eb[bits >> 6] & fg) ^ bg;
		base[1] = (eb[(bits >> 4) & 3] & fg) ^ bg;
		base[2] = (eb[(bits >> 2) & 3] & fg) ^ bg;
		base[3] = (eb[bits & 3] & fg) ^ bg;
		base = (u32 *) ((char *)base + rb);
	}
}
#endif
static void BTEXT
draw_byte_8(unsigned char *font, u32 *base, u32 rb)
{
	u32 l, bits;
	u32 fg = 0x0F0F0F0F;
	u32 bg = 0x00000000;
	u32 *eb = expand_bits_8;

	for (l = 0; l < 16; ++l)
	{
		bits = *font++;
		base[0] = (eb[bits >> 4] & fg) ^ bg;
		base[1] = (eb[bits & 0xf] & fg) ^ bg;
		base = (u32 *) ((char *)base + rb);
	}
}

void btext_init(void)
{
	btext_setup_display(640, 480, 8, 640,0xfc000000);
// Not realy init
// It will be init in xlinit.c because We only can access fb after the device resource is allocated and enabled.

}

void btext_tx_byte(unsigned char data)
{
        btext_drawchar(data);
}

static const struct console_driver btext_console __console = {
        .init    = btext_init,
        .tx_byte = btext_tx_byte,
        .rx_byte = 0,
        .tst_byte = 0,
};

