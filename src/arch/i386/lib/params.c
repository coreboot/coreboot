#include <string.h>

/*
This software and ancillary information (herein called SOFTWARE )
called LinuxBIOS          is made available under the terms described
here.  The SOFTWARE has been approved for release with associated
LA-CC Number 00-34   .  Unless otherwise indicated, this SOFTWARE has
been authored by an employee or employees of the University of
California, operator of the Los Alamos National Laboratory under
Contract No. W-7405-ENG-36 with the U.S. Department of Energy.  The
U.S. Government has rights to use, reproduce, and distribute this
SOFTWARE.  The public may copy, distribute, prepare derivative works
and publicly display this SOFTWARE without charge, provided that this
Notice and any statement of authorship are reproduced on all copies.
Neither the Government nor the University makes any warranty, express
or implied, or assumes any liability or responsibility for the use of
this SOFTWARE.  If SOFTWARE is modified to produce derivative works,
such modified SOFTWARE should be clearly marked, so as not to confuse
it with the version available from LANL.
 */
/* Copyright 2000, Ron Minnich, Advanced Computing Lab, LANL
 * rminnich@lanl.gov
 */

#ifndef lint
static char rcsid[] = "$Id$";
#endif

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

#define E820MAP	0x2d0		/* our map */
#define E820MAX	32		/* number of entries in E820MAP */
#define E820NR	0x1e8		/* # entries in E820MAP */

/* sigh, this is not a struct or include file thing. */
#ifdef i386
/* from linux 2.2.13 */
/*
 * This is set up by the setup-routine at boot-time
 */
#define PARAM   ((unsigned char *)empty_zero_page)
#define SCREEN_INFO (*(struct screen_info *) (PARAM+0))
#define EXT_MEM_K (*(unsigned short *) (PARAM+2))
#define ALT_MEM_K (*(unsigned long *) (PARAM+0x1e0))
#define E820_MAP_NR (*(char*) (PARAM+E820NR))
#define E820_MAP    ((unsigned long *) (PARAM+E820MAP))
#define APM_BIOS_INFO (*(struct apm_bios_info *) (PARAM+0x40))
#define DRIVE_INFO (*(struct drive_info_struct *) (PARAM+0x80))
#define SYS_DESC_TABLE (*(struct sys_desc_table_struct*)(PARAM+0xa0))
#define MOUNT_ROOT_RDONLY (*(unsigned short *) (PARAM+0x1F2))
#define RAMDISK_FLAGS (*(unsigned short *) (PARAM+0x1F8))
#define ORIG_ROOT_DEV (*(unsigned short *) (PARAM+0x1FC))
#define AUX_DEVICE_INFO (*(unsigned char *) (PARAM+0x1FF))
#define LOADER_TYPE (*(unsigned char *) (PARAM+0x210))
#define KERNEL_START (*(unsigned long *) (PARAM+0x214))
#define INITRD_START (*(unsigned long *) (PARAM+0x218))
#define INITRD_SIZE (*(unsigned long *) (PARAM+0x21c))
#define COMMAND_LINE ((char *) (PARAM+2048))
#define COMMAND_LINE_SIZE 256

#endif
/* this is support for passing parameters to the linux kernel. 
 * you can pass memory size, command lines, and so on. 
 */

void
init_params(unsigned char *empty_zero_page)
{
	memset(PARAM, 0, 4096 /*XXXXXXXXXXXXXXXXX */ );
}

void
set_memory_size(unsigned char *empty_zero_page, unsigned long ext_memory_size,
		unsigned long alt_memory_size)
{
	/* for now, we do not have an 820 map (who cares, 8x0 chipsets are dead! */
	E820_MAP_NR = 0;
	EXT_MEM_K = ext_memory_size;
	ALT_MEM_K = alt_memory_size;
}

void
set_root_rdonly(unsigned char *empty_zero_page)
{
	MOUNT_ROOT_RDONLY = 1;
}

/* see the lilo doc */
void
set_command_line(unsigned char *empty_zero_page, unsigned char *cmdline)
{
	int i;
	/* no strncpy ...
	   strncpy(COMMAND_LINE, cmdline, COMMAND_LINE_SIZE);
	 */
	for (i = 0; (i < COMMAND_LINE_SIZE) && (cmdline[i]); i++)
		COMMAND_LINE[i] = cmdline[i];
	*(unsigned short *) 0x90020 = 0xa33f;
	*(unsigned short *) 0x90022 = 2048;
}

unsigned char *
get_empty_zero_page()
{
	/* gosh, I thought we had to do get at empty_zero_page, and we don't. 
	 * silly me. We just have to get to 0x90000
	 */
	/* we'll leave this hook in for now. We may need it someday */
//	unsigned long l = *(unsigned long *) 0xffffc;

	return (unsigned char *) 0x90000;
}

/* I am pretty sure we only need to set rows and cols for now. 
 * All the rest is BIOS stuff. If it gets worse we'll have to make this a 
 * screen_info * as the param
 */
void
set_display(unsigned char *empty_zero_page, int rows, int cols)
{
	struct screen_info *sc = &SCREEN_INFO;
	sc->orig_video_cols = cols;
	sc->orig_video_lines = rows;
}

void
set_initrd(unsigned char *empty_zero_page, unsigned long start, unsigned long size)
{
  INITRD_START = start;
  INITRD_SIZE  = size;
  LOADER_TYPE  = 1;
}
