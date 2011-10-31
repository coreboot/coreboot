/* Public Domain Curses */
/* This file is BSD licensed, Copyright 2011 secunet AG */

#include "lppdc.h"
#include <libpayload.h>

/* ACS definitions originally by jshumate@wrdis01.robins.af.mil -- these
   match code page 437 and compatible pages (CP850, CP852, etc.) */

#ifdef CHTYPE_LONG

# define A(x) ((chtype)x | A_ALTCHARSET)

chtype acs_map[128] =
{
    A(0), A(1), A(2), A(3), A(4), A(5), A(6), A(7), A(8), A(9), A(10),
    A(11), A(12), A(13), A(14), A(15), A(16), A(17), A(18), A(19),
    A(20), A(21), A(22), A(23), A(24), A(25), A(26), A(27), A(28),
    A(29), A(30), A(31), ' ', '!', '"', '#', '$', '%', '&', '\'', '(',
    ')', '*',

    A(0x1a), A(0x1b), A(0x18), A(0x19),

    '/',

    0xdb,

    '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=',
    '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',

    A(0x04), 0xb1,

    'b', 'c', 'd', 'e',

    0xf8, 0xf1, 0xb0, A(0x0f), 0xd9, 0xbf, 0xda, 0xc0, 0xc5, 0x2d, 0x2d,
    0xc4, 0x2d, 0x5f, 0xc3, 0xb4, 0xc1, 0xc2, 0xb3, 0xf3, 0xf2, 0xe3,
    0xd8, 0x9c, 0xf9,

    A(127)
};

# undef A

#endif

/* See terminfo(5). */
chtype fallback_acs_map[128] =
	{
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	'>',	'<',	'^',	'v',	' ',
	'#',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	' ',	' ',	' ',	' ',	' ',	' ',	' ',	' ',
	'+',	':',	' ',	' ',	' ',	' ',	'\\',   '#',
	'#',	'#',	'+',	'+',	'+',	'+',	'+',	'~',
	'-',	'-',	'-',	'_',	'+',	'+',	'+',	'+',
	'|',	'<',	'>',	'*',	'!',	'f',	'o',	' ',
	};

#ifdef CONFIG_SERIAL_CONSOLE
#ifdef CONFIG_SERIAL_ACS_FALLBACK
chtype serial_acs_map[128];
#else
/* See acsc of vt100. */
chtype serial_acs_map[128] =
	{
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	'`',	'a',	0,	0,	0,	0,	'f',	'g',
	0,	0,	'j',	'k',	'l',	'm',	'n',	'o',
	'p',	'q',	'r',	's',	't',	'u',	'v',	'w',
	'x',	'y',	'z',	'{',	'|',	'}',	'~',	0,
	};
#endif
#endif

#ifdef CONFIG_VIDEO_CONSOLE
/* See acsc of linux. */
chtype console_acs_map[128] =
	{
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	'\020', '\021', '\030', '\031',	0,
	'\333',	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,
	'\004',	'\261',	0,	0,	0,	0,	'\370',	'\361',
	'\260',	'\316',	'\331',	'\277',	'\332',	'\300',	'\305',	'~',
	'\304',	'\304',	'\304',	'_',	'\303', '\264', '\301',	'\302',
	'\263',	'\363',	'\362',	'\343',	'\330',	'\234',	'\376',	0,
	};
#endif

/* position hardware cursor at (y, x) */

void PDC_gotoyx(int row, int col)
{
    PDC_LOG(("PDC_gotoyx() - called: row %d col %d\n", row, col));

#ifdef CONFIG_SERIAL_CONSOLE
    serial_set_cursor(col, row);
#endif
#ifdef CONFIG_VIDEO_CONSOLE
    video_console_set_cursor(col, row);
#endif
}

/* update the given physical line to look like the corresponding line in
   curscr */

void PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    int j, ch, attr;

    PDC_LOG(("PDC_transform_line() - called: line %d, len %d, curses_flags %d\n", lineno, len, curses_flags));

#ifdef CONFIG_SERIAL_CONSOLE
    int serial_is_bold = 0;
    int serial_is_reverse = 0;
    int serial_is_altcharset = 0;
    int serial_cur_pair = 0;
    int need_altcharset;

    if (curses_flags & F_ENABLE_SERIAL) {
        serial_end_bold();
        serial_end_altcharset();
        serial_set_cursor(lineno, x);
    }
#endif

    for (j = 0; j < len; j++)
    {
	ch = srcp[j];
	attr = ch;
#ifdef CONFIG_SERIAL_CONSOLE
	if (curses_flags & F_ENABLE_SERIAL) {
		if (attr & A_BOLD) {
			if (!serial_is_bold) {
				serial_start_bold();
				serial_is_bold = 1;
			}
		} else {
			if (serial_is_bold) {
				serial_end_bold();
				serial_is_bold = 0;
				/* work around serial.c
				 * shortcoming:
				 */
				serial_is_reverse = 0;
				serial_cur_pair = 0;
			}
		}

		if (attr & A_REVERSE) {
			if (!serial_is_reverse) {
				serial_start_reverse();
				serial_is_reverse = 1;
			}
		} else {
			if (serial_is_reverse) {
				serial_end_reverse();
				serial_is_reverse = 0;
				/* work around serial.c
				 * shortcoming:
				 */
				serial_is_bold = 0;
				serial_cur_pair = 0;
			}
		}

		need_altcharset = 0;
		if (attr & A_ALTCHARSET) {
			if (serial_acs_map[ch & 0x7f]) {
				ch = serial_acs_map[ch & 0x7f];
				need_altcharset = 1;
			} else
				ch = fallback_acs_map[ch & 0x7f];
		}
		if (need_altcharset && !serial_is_altcharset) {
			serial_start_altcharset();
			serial_is_altcharset = 1;
		}
		if (!need_altcharset && serial_is_altcharset) {
			serial_end_altcharset();
			serial_is_altcharset = 0;
		}

		if (serial_cur_pair != PAIR_NUMBER(attr)) {
			short int fg, bg;
			pair_content(PAIR_NUMBER(attr),
				     &fg, &bg);
			serial_set_color(fg, bg);
			serial_cur_pair = PAIR_NUMBER(attr);
		}

		serial_putchar(ch & 0xff);

	}
#endif
#ifdef CONFIG_VIDEO_CONSOLE
	unsigned char c = pdc_atrtab[srcp[j] >> PDC_ATTR_SHIFT];

	if (curses_flags & F_ENABLE_CONSOLE) {
		if (attr & A_ALTCHARSET) {
			if (console_acs_map[ch & 0x7f])
				ch = console_acs_map[ch & 0x7f];
			else
				ch = fallback_acs_map[ch & 0x7f];
		}

		/*
		 * FIXME: Somewhere along the line, the
		 * character value is getting sign-extented.
		 * For now grab just the 8 bit character,
		 * but this will break wide characters!
		 */
		video_console_putc(lineno, x + j, (c << 8) | ( ch & 0xff));
	}
#endif
    }
}

