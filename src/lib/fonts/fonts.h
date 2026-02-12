/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __FONTS_H__
#define __FONTS_H__

#include <types.h>

/* Printable ASCII character range [32-126] */
#define FONT_START_CHAR  32   /* ASCII Space */
#define FONT_END_CHAR    126  /* ASCII ~ */
#define FONT_NUM_CHARS  (FONT_END_CHAR - FONT_START_CHAR + 1)

extern const uint8_t font_table[FONT_NUM_CHARS][CONFIG_FONT_WIDTH * CONFIG_FONT_HEIGHT];
extern const uint8_t font_widths[FONT_NUM_CHARS];

#endif // __FONTS_H__
