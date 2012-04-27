#ifndef _VGA_H
#define _VGA_H

/*
 * Basic palette.
 */
struct palette {
        unsigned char red;
        unsigned char green;
        unsigned char blue;
};

extern const struct palette default_vga_palette[0x100];

extern const unsigned char vga_font_8x16[256][16];

#endif /* _VGA_H */
