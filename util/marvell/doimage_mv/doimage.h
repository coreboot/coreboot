/*******************************************************************************
Copyright (C) Marvell International Ltd. and its affiliates

Marvell GPL License Option

If you received this File from Marvell, you may opt to use, redistribute and/or
modify this File in accordance with the terms and conditions of the General
Public License Version 2, June 1991 (the "GPL License"), a copy of which is
available along with the File in the license.txt file or by writing to the Free
Software Foundation, Inc.

THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE IMPLIED
WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY
DISCLAIMED.  The GPL License provides additional details about this warranty
disclaimer.

*******************************************************************************/
#ifndef _INC_DOIMAGE_H
#define _INC_DOIMAGE_H

#include <sys/types.h>
#include <commonlib/bsd/helpers.h>

/* use the same version as in "bootrom.inc" file */
#define VERSION_NUMBER "2.20"
#define PRODUCT_SUPPORT "Marvell Armada-3xx series"

#define RSA_KEY_SIZE 2048
#define RSA_EXPONENT 65537

#define T_OPTION_MASK 0x1       /* image type */
#define D_OPTION_MASK 0x2       /* image destination */
#define E_OPTION_MASK 0x4       /* image execution address */
#define S_OPTION_MASK 0x8       /* starting sector */
#define R_OPTION_MASK 0x10      /* DRAM file */
#define C_OPTION_MASK 0x20      /* headers definition file */
#define P_OPTION_MASK 0x40      /* NAND Page size */
#define M_OPTION_MASK 0x80      /* TWSI serial init file */
#define W_OPTION_MASK 0x100     /* HEX file width */
#define H_OPTION_MASK 0x200     /* Header mode */
#define X_OPTION_MASK 0x400     /* Pre padding */
#define Y_OPTION_MASK 0x800     /* Post padding */
#define J_OPTION_MASK 0x1000    /* JTAG Enabled */
#define B_OPTION_MASK 0x2000    /* Box ID */
#define Z_OPTION_MASK 0x4000    /* secure boot mode - KAK private key */
#define F_OPTION_MASK 0x8000    /* Flash ID */
#define A_OPTION_MASK 0x10000   /* secure boot mode - CSK private key */
#define G_OPTION_MASK 0x20000   /* binary file */
#define K_OPTION_MASK 0x40000   /* secure boot mode - CSK private key index */
#define L_OPTION_MASK 0x80000   /* NAND block size (in 64K chunks) */
#define N_OPTION_MASK 0x100000  /* NAND cell technology MLC/SLC */
#define p_OPTION_MASK 0x200000  /* Print enable */
#define b_OPTION_MASK 0x400000  /* Baudrate */
#define u_OPTION_MASK 0x800000  /* debug serial port number */
#define m_OPTION_MASK 0x1000000 /* debug serial port MPP configuration */

#ifndef O_BINARY /* should be defined on __WIN32__ */
#define O_BINARY 0
#endif

typedef enum {
	IMG_SATA,
	IMG_UART,
	IMG_FLASH,
	IMG_MMC,
	IMG_BOOTROM,
	IMG_NAND,
	IMG_HEX,
	IMG_BIN,
	IMG_PEX,
	IMG_I2C

} IMG_TYPE;

#define REGULAR_IMAGE(opt)                                                     \
	(((opt)->image_type != IMG_BOOTROM) &&                                 \
	 ((opt)->image_type != IMG_HEX) && ((opt)->image_type != IMG_BIN))

typedef struct {
	IMG_TYPE img_type;    /* image type */
	char *img_name;       /* image name string */
	unsigned int img_opt; /* mandatory options for this image type */

} IMG_MAP;

typedef enum {
	IMG_FILE_INDX = 1,
	HDR_FILE_INDX = 2

} FILE_IDX;

typedef enum {
	HDR_IMG_ONE_FILE = 1,  /* Create one file with header and image */
	HDR_IMG_TWO_FILES = 2, /* Create separate header and image files */
	HDR_ONLY = 3,	  /* Create only header */
	IMG_ONLY = 4,	  /* Create only image */

} HEADER_MODE;

typedef struct {
	IMG_TYPE image_type;
	char *fname_dram;	 /* DRAM init file for "register" header */
	char *fname_twsi;	 /* TWSI serial init file */
	char *fname_bin;	  /* binary code file for "binary" header */
	char *fname_prkey;	/* KAK RSA Private key file */
	char *fname_prkeyCsk;     /* CSK RSA Private key file */
	char *fname_list;	 /* headers definition file */
	u32 flags;		  /* user-defined flags */
	u32 req_flags;		  /* mandatory flags */
	u32 image_source;	 /* starting sector */
	u32 image_dest;		  /* image destination  */
	u32 image_exec;		  /* image execution  */
	unsigned int hex_width;   /* HEX file width */
	unsigned int header_mode; /* Header file mode */
	int csk_index;
	int pre_padding;
	int post_padding;
	int prepadding_size;
	int postpadding_size;
	unsigned int bytesToAlign;
	unsigned int nandPageSize;
	unsigned int nandBlkSize;
	char nandCellTech;
	u32 boxId;
	u32 flashId;
	u32 jtagDelay;
	char *image_buf;  /* image buffer for image pre-load */
	u32 image_sz;     /* total size of pre-loaded image buffer including
			     paddings */
	u32 img_gap;      /* gap between header and image start point */
	u32 baudRate;     /* debug print port baudrate */
	u32 debugPortNum; /* debug print port number */
	u32 debugPortMpp; /* debug print port MPP configuration */
	union {
		char *fname_arr[5];
		struct {
			char *in;
			char *out;
			char *hdr_out;
			char *romc;
			char *romd;
		} fname;
	};
} USER_OPTIONS;

/* Function declaration */
void print_usage(void);

/* 32 bit checksum */
MV_U32 checksum32(void *start, MV_U32 len, MV_U32 csum);
MV_U8 checksum8(void *start, MV_U32 len, MV_U8 csum);
MV_U32 crc32(MV_U32 crc, volatile MV_U32 *buf, MV_U32 len);

#endif /* _INC_DOIMAGE_H */
