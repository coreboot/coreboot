/*
 *  linux/drivers/video/fbcon.h -- Low level frame buffer based console driver
 *
 *	Copyright (C) 1997 Geert Uytterhoeven
 *
 *  This file is subject to the terms and conditions of the GNU General Public
 *  License.  See the file COPYING in the main directory of this archive
 *  for more details.
 */

#ifndef _VIDEO_FBCON_H
#define _VIDEO_FBCON_H

struct display {
    /* Filled in by the frame buffer device */
    struct fb_var_screeninfo var;   /* variable infos. yoffset and vmode */
                                    /* are updated by fbcon.c */
    struct fb_cmap cmap;            /* colormap */
    char *screen_base;              /* pointer to top of virtual screen */
                                    /* (virtual address) */
    int visual;
    int type;                       /* see FB_TYPE_* */
    int type_aux;                   /* Interleave for interleaved Planes */
    u16 ypanstep;               /* zero if no hardware ypan */
    u16 ywrapstep;              /* zero if no hardware ywrap */
    u32 line_length;             /* length of a line in bytes */
    u16 can_soft_blank;         /* zero if no hardware blanking */
    u16 inverse;                /* != 0 text black on white as default */

    /* Filled in by the low-level console driver */

    struct vc_data *conp;           /* pointer to console data */
    int vrows;                      /* number of virtual rows */
    unsigned short cursor_x;        /* current cursor position */
    unsigned short cursor_y;
    int fgcol;                      /* text colors */
    int bgcol;
    u32 next_line;               /* offset to one line below */
    u32 next_plane;              /* offset to next plane */
    u8 *fontdata;               /* Font associated to this display */
    unsigned short _fontheightlog;
    unsigned short _fontwidthlog;
    unsigned short _fontheight;
    unsigned short _fontwidth;
    int userfont;                   /* != 0 if fontdata kmalloc()ed */
    u16 scrollmode;             /* Scroll Method */
    short yscroll;                  /* Hardware scrolling */
    unsigned char fgshift, bgshift;
    unsigned short charmask;        /* 0xff or 0x1ff */
};


#define fontheight(p) ((p)->_fontheight)
#define fontheightlog(p) ((p)->_fontheightlog)

#ifdef FBCON_FONTWIDTH8_ONLY

/* fontwidth w is supported by dispsw */
#define FONTWIDTH(w)	(1 << ((8) - 1))
/* fontwidths w1-w2 inclusive are supported by dispsw */
#define FONTWIDTHRANGE(w1,w2)	FONTWIDTH(8)

#define fontwidth(p) (8)
#define fontwidthlog(p) (0)

#else

/* fontwidth w is supported by dispsw */
#define FONTWIDTH(w)	(1 << ((w) - 1))
/* fontwidths w1-w2 inclusive are supported by dispsw */
#define FONTWIDTHRANGE(w1,w2)	(FONTWIDTH(w2+1) - FONTWIDTH(w1))

#define fontwidth(p) ((p)->_fontwidth)
#define fontwidthlog(p) ((p)->_fontwidthlog)

#endif

    /*
     *  Attribute Decoding
     */

/* Color */
#define attr_fgcol(p,s)    \
	(((s) >> ((p)->fgshift)) & 0x0f)
#define attr_bgcol(p,s)    \
	(((s) >> ((p)->bgshift)) & 0x0f)
#define	attr_bgcol_ec(p,conp) \
	((conp) ? (((conp)->vc_video_erase_char >> ((p)->bgshift)) & 0x0f) : 0)

/* Monochrome */
#define attr_bold(p,s) \
	((s) & 0x200)
#define attr_reverse(p,s) \
	(((s) & 0x800) ^ ((p)->inverse ? 0x800 : 0))
#define attr_underline(p,s) \
	((s) & 0x400)
#define attr_blink(p,s) \
	((s) & 0x8000)

    /*
     *  Scroll Method
     */

/* Internal flags */
#define __SCROLL_YPAN		0x001
#define __SCROLL_YWRAP		0x002
#define __SCROLL_YMOVE		0x003
#define __SCROLL_YREDRAW	0x004
#define __SCROLL_YMASK		0x00f
#define __SCROLL_YFIXED		0x010
#define __SCROLL_YNOMOVE	0x020
#define __SCROLL_YPANREDRAW	0x040
#define __SCROLL_YNOPARTIAL	0x080

/* Only these should be used by the drivers */
/* Which one should you use? If you have a fast card and slow bus,
   then probably just 0 to indicate fbcon should choose between
   YWRAP/YPAN+MOVE/YMOVE. On the other side, if you have a fast bus
   and even better if your card can do fonting (1->8/32bit painting),
   you should consider either SCROLL_YREDRAW (if your card is
   able to do neither YPAN/YWRAP), or SCROLL_YNOMOVE.
   The best is to test it with some real life scrolling (usually, not
   all lines on the screen are filled completely with non-space characters,
   and REDRAW performs much better on such lines, so don't cat a file
   with every line covering all screen columns, it would not be the right
   benchmark).
 */
#define SCROLL_YREDRAW		(__SCROLL_YFIXED|__SCROLL_YREDRAW)
#define SCROLL_YNOMOVE		(__SCROLL_YNOMOVE|__SCROLL_YPANREDRAW)

/* SCROLL_YNOPARTIAL, used in combination with the above, is for video
   cards which can not handle using panning to scroll a portion of the
   screen without excessive flicker.  Panning will only be used for
   whole screens.
 */
/* Namespace consistency */
#define SCROLL_YNOPARTIAL	__SCROLL_YNOPARTIAL

#endif /* _VIDEO_FBCON_H */
