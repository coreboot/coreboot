#ifndef _LINUX_FB_H
#define _LINUX_FB_H

#define u32 uint32_t
#define u16 uint16_t
#define u8	uint8_t
#define s16	short

/* Definitions of frame buffers						*/

#define FB_MAJOR		29
#define FB_MAX			32	/* sufficient for now */

/* ioctls
   0x46 is 'F'								*/
#define FBIOGET_VSCREENINFO	0x4600
#define FBIOPUT_VSCREENINFO	0x4601
#define FBIOGET_FSCREENINFO	0x4602
#define FBIOGETCMAP		0x4604
#define FBIOPUTCMAP		0x4605
#define FBIOPAN_DISPLAY		0x4606
/* 0x4607-0x460B are defined below */
/* #define FBIOGET_MONITORSPEC	0x460C */
/* #define FBIOPUT_MONITORSPEC	0x460D */
/* #define FBIOSWITCH_MONIBIT	0x460E */
#define FBIOGET_CON2FBMAP	0x460F
#define FBIOPUT_CON2FBMAP	0x4610
#define FBIOBLANK		0x4611		/* arg: 0 or vesa level + 1 */
#define FBIOGET_VBLANK		_IOR('F', 0x12, struct fb_vblank)
#define FBIO_ALLOC		0x4613
#define FBIO_FREE		0x4614
#define FBIOGET_GLYPH		0x4615
#define FBIOGET_HWCINFO		0x4616
#define FBIOPUT_MODEINFO	0x4617
#define FBIOGET_DISPINFO	0x4618


#define FB_TYPE_PACKED_PIXELS		0	/* Packed Pixels	*/
#define FB_TYPE_PLANES			1	/* Non interleaved planes */
#define FB_TYPE_INTERLEAVED_PLANES	2	/* Interleaved planes	*/
#define FB_TYPE_TEXT			3	/* Text/attributes	*/
#define FB_TYPE_VGA_PLANES		4	/* EGA/VGA planes	*/

#define FB_AUX_TEXT_MDA		0	/* Monochrome text */
#define FB_AUX_TEXT_CGA		1	/* CGA/EGA/VGA Color text */
#define FB_AUX_TEXT_S3_MMIO	2	/* S3 MMIO fasttext */
#define FB_AUX_TEXT_MGA_STEP16	3	/* MGA Millenium I: text, attr, 14 reserved bytes */
#define FB_AUX_TEXT_MGA_STEP8	4	/* other MGAs:	    text, attr,  6 reserved bytes */

#define FB_AUX_VGA_PLANES_VGA4		0	/* 16 color planes (EGA/VGA) */
#define FB_AUX_VGA_PLANES_CFB4		1	/* CFB4 in planes (VGA) */
#define FB_AUX_VGA_PLANES_CFB8		2	/* CFB8 in planes (VGA) */

#define FB_VISUAL_MONO01		0	/* Monochr. 1=Black 0=White */
#define FB_VISUAL_MONO10		1	/* Monochr. 1=White 0=Black */
#define FB_VISUAL_TRUECOLOR		2	/* True color	*/
#define FB_VISUAL_PSEUDOCOLOR		3	/* Pseudo color (like atari) */
#define FB_VISUAL_DIRECTCOLOR		4	/* Direct color */
#define FB_VISUAL_STATIC_PSEUDOCOLOR	5	/* Pseudo color readonly */

#define FB_ACCEL_NONE		0	/* no hardware accelerator	*/
#define FB_ACCEL_ATARIBLITT	1	/* Atari Blitter		*/
#define FB_ACCEL_AMIGABLITT	2	/* Amiga Blitter		      */
#define FB_ACCEL_S3_TRIO64	3	/* Cybervision64 (S3 Trio64)    */
#define FB_ACCEL_NCR_77C32BLT	4	/* RetinaZ3 (NCR 77C32BLT)	*/
#define FB_ACCEL_S3_VIRGE	5	/* Cybervision64/3D (S3 ViRGE)	*/
#define FB_ACCEL_ATI_MACH64GX	6	/* ATI Mach 64GX family		*/
#define FB_ACCEL_DEC_TGA	7	/* DEC 21030 TGA		*/
#define FB_ACCEL_ATI_MACH64CT	8	/* ATI Mach 64CT family		*/
#define FB_ACCEL_ATI_MACH64VT	9	/* ATI Mach 64CT family VT class */
#define FB_ACCEL_ATI_MACH64GT	10	/* ATI Mach 64CT family GT class */
#define FB_ACCEL_SUN_CREATOR	11	/* Sun Creator/Creator3D	*/
#define FB_ACCEL_SUN_CGSIX	12	/* Sun cg6			*/
#define FB_ACCEL_SUN_LEO	13	/* Sun leo/zx			*/
#define FB_ACCEL_IMS_TWINTURBO	14	/* IMS Twin Turbo		*/
#define FB_ACCEL_3DLABS_PERMEDIA2 15	/* 3Dlabs Permedia 2		*/
#define FB_ACCEL_MATROX_MGA2064W 16	/* Matrox MGA2064W (Millenium)	*/
#define FB_ACCEL_MATROX_MGA1064SG 17	/* Matrox MGA1064SG (Mystique)	*/
#define FB_ACCEL_MATROX_MGA2164W 18	/* Matrox MGA2164W (Millenium II) */
#define FB_ACCEL_MATROX_MGA2164W_AGP 19	/* Matrox MGA2164W (Millenium II) */
#define FB_ACCEL_MATROX_MGAG100	20	/* Matrox G100 (Productiva G100) */
#define FB_ACCEL_MATROX_MGAG200	21	/* Matrox G200 (Myst, Mill, ...) */
#define FB_ACCEL_SUN_CG14	22	/* Sun cgfourteen		 */
#define FB_ACCEL_SUN_BWTWO	23	/* Sun bwtwo			*/
#define FB_ACCEL_SUN_CGTHREE	24	/* Sun cgthree			*/
#define FB_ACCEL_SUN_TCX	25	/* Sun tcx			*/
#define FB_ACCEL_MATROX_MGAG400	26	/* Matrox G400			*/
#define FB_ACCEL_NV3		27	/* nVidia RIVA 128		 */
#define FB_ACCEL_NV4		28	/* nVidia RIVA TNT		*/
#define FB_ACCEL_NV5		29	/* nVidia RIVA TNT2		*/
#define FB_ACCEL_CT_6555x	30	/* C&T 6555x			*/
#define FB_ACCEL_3DFX_BANSHEE	31	/* 3Dfx Banshee			*/
#define FB_ACCEL_ATI_RAGE128	32	/* ATI Rage128 family		*/
#define FB_ACCEL_IGS_CYBER2000	33	/* CyberPro 2000		*/
#define FB_ACCEL_IGS_CYBER2010	34	/* CyberPro 2010		*/
#define FB_ACCEL_IGS_CYBER5000	35	/* CyberPro 5000		*/
#define FB_ACCEL_SIS_GLAMOUR	36	/* SiS 300/630/540		   */
#define FB_ACCEL_3DLABS_PERMEDIA3 37	/* 3Dlabs Permedia 3		*/
#define FB_ACCEL_ATI_RADEON	38	/* ATI Radeon family		*/
#define FB_ACCEL_SIS_GLAMOUR_2  40	/* SiS 315, 650, 740		*/
#define FB_ACCEL_SIS_XABRE	41	/* SiS 330 ("Xabre")		*/

#define FB_ACCEL_NEOMAGIC_NM2070 90	/* NeoMagic NM2070		    */
#define FB_ACCEL_NEOMAGIC_NM2090 91	/* NeoMagic NM2090		    */
#define FB_ACCEL_NEOMAGIC_NM2093 92	/* NeoMagic NM2093		    */
#define FB_ACCEL_NEOMAGIC_NM2097 93	/* NeoMagic NM2097		    */
#define FB_ACCEL_NEOMAGIC_NM2160 94	/* NeoMagic NM2160		    */
#define FB_ACCEL_NEOMAGIC_NM2200 95	/* NeoMagic NM2200		    */
#define FB_ACCEL_NEOMAGIC_NM2230 96	/* NeoMagic NM2230		    */
#define FB_ACCEL_NEOMAGIC_NM2360 97	/* NeoMagic NM2360		    */
#define FB_ACCEL_NEOMAGIC_NM2380 98	/* NeoMagic NM2380		    */

#if 0

struct fb_fix_screeninfo {
	char id[16];			/* identification string eg "TT Builtin" */
	unsigned long smem_start;	/* Start of frame buffer mem */
					/* (physical address) */
	u32 smem_len;			/* Length of frame buffer mem */
	u32 type;			/* see FB_TYPE_*		*/
	u32 type_aux;			/* Interleave for interleaved Planes */
	u32 visual;			/* see FB_VISUAL_*		*/
	u16 xpanstep;			/* zero if no hardware panning  */
	u16 ypanstep;			/* zero if no hardware panning  */
	u16 ywrapstep;		/* zero if no hardware ywrap	 */
	u32 line_length;		/* length of a line in bytes	   */
	unsigned long mmio_start;	/* Start of Memory Mapped I/O   */
					/* (physical address) */
	u32 mmio_len;			/* Length of Memory Mapped I/O  */
	u32 accel;			/* Type of acceleration available */
	u16 reserved[3];		/* Reserved for future compatibility */
};

#endif

/* Interpretation of offset for color fields: All offsets are from the right,
 * inside a "pixel" value, which is exactly 'bits_per_pixel' wide (means: you
 * can use the offset as right argument to <<). A pixel afterwards is a bit
 * stream and is written to video memory as that unmodified. This implies
 * big-endian byte order if bits_per_pixel is greater than 8.
 */
struct fb_bitfield {
	u32 offset;			/* beginning of bitfield	*/
	u32 length;			/* length of bitfield		*/
	u32 msb_right;		/* != 0 : Most significant bit is */
					/* right */
};

#define FB_NONSTD_HAM		1	/* Hold-And-Modify (HAM)	 */

#define FB_ACTIVATE_NOW		0	/* set values immediately (or vbl)*/
#define FB_ACTIVATE_NXTOPEN	1	/* activate on next open	*/
#define FB_ACTIVATE_TEST	2	/* don't set, round up impossible */
#define FB_ACTIVATE_MASK       15
					/* values			*/
#define FB_ACTIVATE_VBL	       16	/* activate values on next vbl  */
#define FB_CHANGE_CMAP_VBL     32	/* change colormap on vbl	*/
#define FB_ACTIVATE_ALL	       64	/* change all VCs on this fb	*/

#define FB_ACCELF_TEXT		1	/* text mode acceleration */

#define FB_SYNC_HOR_HIGH_ACT	1	/* horizontal sync high active	*/
#define FB_SYNC_VERT_HIGH_ACT	2	/* vertical sync high active	*/
#define FB_SYNC_EXT		4	/* external sync		*/
#define FB_SYNC_COMP_HIGH_ACT	8	/* composite sync high active	*/
#define FB_SYNC_BROADCAST	16	/* broadcast video timings	     */
					/* vtotal = 144d/288n/576i => PAL  */
					/* vtotal = 121d/242n/484i => NTSC */
#define FB_SYNC_ON_GREEN	32	/* sync on green */

#define FB_VMODE_NONINTERLACED  0	/* non interlaced */
#define FB_VMODE_INTERLACED	1	/* interlaced	*/
#define FB_VMODE_DOUBLE		2	/* double scan */
#define FB_VMODE_MASK		255

#define FB_VMODE_YWRAP		256	/* ywrap instead of panning	    */
#define FB_VMODE_SMOOTH_XPAN	512	/* smooth xpan possible (internally used) */
#define FB_VMODE_CONUPDATE	512	/* don't update x/yoffset	*/

struct fb_var_screeninfo {
	u32 xres;			/* visible resolution		*/
	u32 yres;
	u32 xres_virtual;		/* virtual resolution		*/
	u32 yres_virtual;
	u32 xoffset;			/* offset from virtual to visible */
	u32 yoffset;			/* resolution			*/

	u32 bits_per_pixel;		/* guess what			*/
	u32 grayscale;		/* != 0 Graylevels instead of colors */

	struct fb_bitfield red;		/* bitfield in fb mem if true color, */
	struct fb_bitfield green;	/* else only length is significant */
	struct fb_bitfield blue;
	struct fb_bitfield transp;	/* transparency			*/

	u32 nonstd;			/* != 0 Non standard pixel format */

	u32 activate;			/* see FB_ACTIVATE_*		*/

	u32 height;			/* height of picture in mm    */
	u32 width;			/* width of picture in mm	    */

	u32 accel_flags;		/* acceleration flags (hints)	*/

	/* Timing: All values in pixclocks, except pixclock (of course) */
	u32 pixclock;			/* pixel clock in ps (pico seconds) */
	u32 left_margin;		/* time from sync to picture	*/
	u32 right_margin;		/* time from picture to sync	*/
	u32 upper_margin;		/* time from sync to picture	*/
	u32 lower_margin;
	u32 hsync_len;		/* length of horizontal sync	*/
	u32 vsync_len;		/* length of vertical sync	*/
	u32 sync;			/* see FB_SYNC_*		*/
	u32 vmode;			/* see FB_VMODE_*		*/
	u32 reserved[6];		/* Reserved for future compatibility */
};

struct fb_cmap {
	u32 start;			/* First entry	*/
	u32 len;			/* Number of entries */
	u16 *red;			/* Red values	*/
	u16 *green;
	u16 *blue;
	u16 *transp;			/* transparency, can be NULL */
};

struct fb_con2fbmap {
	u32 console;
	u32 framebuffer;
};

/* VESA Blanking Levels */
#define VESA_NO_BLANKING	0
#define VESA_VSYNC_SUSPEND	1
#define VESA_HSYNC_SUSPEND	2
#define VESA_POWERDOWN		3

struct fb_monspecs {
	u32 hfmin;			/* hfreq lower limit (Hz) */
	u32 hfmax; 			/* hfreq upper limit (Hz) */
	u16 vfmin;			/* vfreq lower limit (Hz) */
	u16 vfmax;			/* vfreq upper limit (Hz) */
	unsigned dpms : 1;		/* supports DPMS */
};

#define FB_VBLANK_VBLANKING	0x001	/* currently in a vertical blank */
#define FB_VBLANK_HBLANKING	0x002	/* currently in a horizontal blank */
#define FB_VBLANK_HAVE_VBLANK	0x004	/* vertical blanks can be detected */
#define FB_VBLANK_HAVE_HBLANK	0x008	/* horizontal blanks can be detected */
#define FB_VBLANK_HAVE_COUNT	0x010	/* global retrace counter is available */
#define FB_VBLANK_HAVE_VCOUNT	0x020	/* the vcount field is valid */
#define FB_VBLANK_HAVE_HCOUNT	0x040	/* the hcount field is valid */
#define FB_VBLANK_VSYNCING	0x080	/* currently in a vsync */
#define FB_VBLANK_HAVE_VSYNC	0x100	/* verical syncs can be detected */

struct fb_vblank {
	u32 flags;			/* FB_VBLANK flags */
	u32 count;			/* counter of retraces since boot */
	u32 vcount;			/* current scanline position */
	u32 hcount;			/* current scandot position */
	u32 reserved[4];		/* reserved for future compatibility */
};


#if 1

#define FBCMD_GET_CURRENTPAR	0xDEAD0005
#define FBCMD_SET_CURRENTPAR	0xDEAD8005

#endif



   /*
    *	 Hardware Cursor
    */

#define FBIOGET_FCURSORINFO	0x4607
#define FBIOGET_VCURSORINFO	0x4608
#define FBIOPUT_VCURSORINFO	0x4609
#define FBIOGET_CURSORSTATE	0x460A
#define FBIOPUT_CURSORSTATE	0x460B


struct fb_fix_cursorinfo {
	u16 crsr_width;		/* width and height of the cursor in */
	u16 crsr_height;		/* pixels (zero if no cursor)	*/
	u16 crsr_xsize;		/* cursor size in display pixels */
	u16 crsr_ysize;
	u16 crsr_color1;		/* colormap entry for cursor color1 */
	u16 crsr_color2;		/* colormap entry for cursor color2 */
};

struct fb_var_cursorinfo {
	u16 width;
	u16 height;
	u16 xspot;
	u16 yspot;
	u8 data[1];			/* field with [height][width]	    */
};

struct fb_cursorstate {
	s16 xoffset;
	s16 yoffset;
	u16 mode;
};


struct fb_info {
   char modename[40];			/* default video mode */
//   kdev_t node;
   int flags;
   int open;				/* Has this been open already ? */
   struct fb_var_screeninfo var;	/* Current var */
#if 0
   struct fb_fix_screeninfo fix;	/* Current fix */
#endif
   struct fb_monspecs monspecs;		/* Current Monitor specs */
   struct fb_cmap cmap;			/* Current cmap */
//   struct fb_ops *fbops;
   char *screen_base;			/* Virtual address */
   struct display *disp;		/* initial display variable */
//   struct vc_data *display_fg;	  /* Console visible on this display */
   char fontname[40];			/* default font name */
#if 0
   devfs_handle_t devfs_handle;		/* Devfs handle for new name	     */
   devfs_handle_t devfs_lhandle;	/* Devfs handle for compat. symlink  */
   int (*changevar)(int);		/* tell console var has changed */
   int (*switch_con)(int, struct fb_info*);
					/* tell fb to switch consoles */
   int (*updatevar)(int, struct fb_info*);
					/* tell fb to update the vars */
   void (*blank)(int, struct fb_info*); /* tell fb to (un)blank the screen */
					/* arg = 0: unblank */
					/* arg > 0: VESA level (arg-1) */
#endif
   void *pseudo_palette;		/* Fake palette of 16 colors and
					   the cursor's color for non
					   palette mode */
   /* From here on everything is device dependent */
   void *par;
};

#endif /* _LINUX_FB_H */
