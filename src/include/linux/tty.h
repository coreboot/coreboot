#ifndef _LINUX_TTY_H
#define _LINUX_TTY_H

/*
 * 'tty.h' defines some structures used by tty_io.c and some defines.
 */

/*
 * These constants are also useful for user-level apps (e.g., VC
 * resizing).
 */
#define MIN_NR_CONSOLES 1       /* must be at least 1 */
#define MAX_NR_CONSOLES	63	/* serial lines start at 64 */
#define MAX_NR_USER_CONSOLES 63	/* must be root to allocate above this */
		/* Note: the ioctl VT_GETSTATE does not work for
		   consoles 16 and higher (since it returns a short) */

/*
 * These are set up by the setup-routine at boot-time:
 */

struct screen_info {
	unsigned char  orig_x;			/* 0x00 */
	unsigned char  orig_y;			/* 0x01 */
	unsigned short dontuse1;		/* 0x02 -- EXT_MEM_K sits here */
	unsigned short orig_video_page;		/* 0x04 */
	unsigned char  orig_video_mode;		/* 0x06 */
	unsigned char  orig_video_cols;		/* 0x07 */
	unsigned short unused2;			/* 0x08 */
	unsigned short orig_video_ega_bx;	/* 0x0a */
	unsigned short unused3;			/* 0x0c */
	unsigned char  orig_video_lines;	/* 0x0e */
	unsigned char  orig_video_isVGA;	/* 0x0f */
	unsigned short orig_video_points;	/* 0x10 */

	/* VESA graphic mode -- linear frame buffer */
	unsigned short lfb_width;		/* 0x12 */
	unsigned short lfb_height;		/* 0x14 */
	unsigned short lfb_depth;		/* 0x16 */
	unsigned long  lfb_base;		/* 0x18 */
	unsigned long  lfb_size;		/* 0x1c */
	unsigned short dontuse2, dontuse3;	/* 0x20 -- CL_MAGIC and CL_OFFSET here */
	unsigned short lfb_linelength;		/* 0x24 */
	unsigned char  red_size;		/* 0x26 */
	unsigned char  red_pos;			/* 0x27 */
	unsigned char  green_size;		/* 0x28 */
	unsigned char  green_pos;		/* 0x29 */
	unsigned char  blue_size;		/* 0x2a */
	unsigned char  blue_pos;		/* 0x2b */
	unsigned char  rsvd_size;		/* 0x2c */
	unsigned char  rsvd_pos;		/* 0x2d */
	unsigned short vesapm_seg;		/* 0x2e */
	unsigned short vesapm_off;		/* 0x30 */
	unsigned short pages;			/* 0x32 */
						/* 0x34 -- 0x3f reserved for future expansion */
};

#define ORIG_X			(screen_info.orig_x)
#define ORIG_Y			(screen_info.orig_y)
#define ORIG_VIDEO_MODE		(screen_info.orig_video_mode)
#define ORIG_VIDEO_COLS 	(screen_info.orig_video_cols)
#define ORIG_VIDEO_EGA_BX	(screen_info.orig_video_ega_bx)
#define ORIG_VIDEO_LINES	(screen_info.orig_video_lines)
#define ORIG_VIDEO_ISVGA	(screen_info.orig_video_isVGA)
#define ORIG_VIDEO_POINTS       (screen_info.orig_video_points)

#define VIDEO_TYPE_MDA		0x10	/* Monochrome Text Display	*/
#define VIDEO_TYPE_CGA		0x11	/* CGA Display 			*/
#define VIDEO_TYPE_EGAM		0x20	/* EGA/VGA in Monochrome Mode	*/
#define VIDEO_TYPE_EGAC		0x21	/* EGA in Color Mode		*/
#define VIDEO_TYPE_VGAC		0x22	/* VGA+ in Color Mode		*/
#define VIDEO_TYPE_VLFB		0x23	/* VESA VGA in graphic mode    	*/

#define VIDEO_TYPE_TGAC		0x40	/* DEC TGA */

#define VIDEO_TYPE_SUN          0x50    /* Sun frame buffer. */
#define VIDEO_TYPE_SUNPCI       0x51    /* Sun PCI based frame buffer. */

#define VIDEO_TYPE_PMAC		0x60	/* PowerMacintosh frame buffer. */

#define VIDEO_TYPE_SGI          0x70    /* Various SGI graphics hardware */
#define VIDEO_TYPE_MIPS_G364	0x71    /* MIPS Magnum 4000 G364 video  */

#endif
