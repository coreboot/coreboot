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

#ifndef _INC_BOOTSTRAP__DEF_H
#define _INC_BOOTSTRAP__DEF_H
#include "bootstrap_os.h"

#ifndef MV_ASMLANGUAGE

#define MAIN_HDR_VERSION 1

#define MAIN_HDR_NAND_SLC 0
#define MAIN_HDR_NAND_MLC 1

typedef struct BHR_t {
	/*	type        name                byte order */
	MV_U8 blockID;		/*   0   */
	MV_U8 flags;		/*   1   */
	MV_U16 nandPageSize;    /*  2-3  */
	MV_U32 blockSize;       /*  4-7  */
	MV_U8 version;		/*   8   */
	MV_U8 hdrSizeMsb;       /*   9   */
	MV_U16 hdrSizeLsb;      /* 10-11 */
	MV_U32 sourceAddr;      /* 12-15 */
	MV_U32 destinationAddr; /* 16-19 */
	MV_U32 executionAddr;   /* 20-23 */
	MV_U8 options;		/*   24  */
	MV_U8 nandBlockSize;    /*   25  */
	MV_U8 nandTechnology;   /*   26  */
	MV_U8 rsvd4;		/*   27  */
	MV_U16 rsvd2;		/* 28-29 */
	MV_U8 ext;		/*   30  */
	MV_U8 checkSum;		/*   31  */

} BHR_t, *pBHR_t;

#define MAIN_HDR_GET_LEN(pHdr)                                                 \
	(((MV_U32)((pHdr)->hdrSizeMsb) << 16) | ((MV_U32)((pHdr)->hdrSizeLsb)))

#define EXT_HDR_TYP_SECURITY 0x01
#define EXT_HDR_TYP_BINARY 0x02
#define EXT_HDR_TYP_REGISTER 0x03

typedef struct headExtBHR_t {/* Common extension header head */
			     //  type        name        byte order
	MV_U8 type;
	MV_U8 lenMsb;
	MV_U16 lenLsb;

} headExtBHR_t;

#define EXT_HDR_SET_LEN(pHead, len)                                            \
	do {                                                                   \
		(pHead)->lenMsb = ((len)&0x00FF0000) >> 16;                    \
		(pHead)->lenLsb = (len)&0x0000FFFF;                            \
	} while (0)

#define EXT_HDR_GET_LEN(pHead)                                                 \
	(((MV_U32)((pHead)->lenMsb) << 16) | ((pHead)->lenLsb))

typedef struct tailExtBHR_t {/* Common extension header tail */
			     // type        name        byte order
	MV_U8 nextHdr;
	MV_U8 delay;
	MV_U16 rsvd2;

} tailExtBHR_t;

typedef struct publicKey_t {/* public key*/
	MV_U8 Key[524];
} publicKey_t;

#define RSA_MAX_KEY_LEN_BYTES 256

typedef struct secExtBHR_t {
	headExtBHR_t head;
	MV_U8 encrypt;
	MV_U8 rsrvd0;
	MV_U16 rsrvd1;
	publicKey_t pubKey;
	MV_U8 jtagEn;
	MV_U8 rsrvd2;
	MV_U16 rsrvd3;
	MV_U32 boxId;
	MV_U32 flashId;
	MV_U8 hdrSign[256];
	MV_U8 imgSign[256];
	publicKey_t cskArray[16];
	MV_U8 cskBlockSign[256];
	tailExtBHR_t tail;

} secExtBHR_t, *pSecExtBHR_T;

/***********************/
/* SECURE PARAMS       */
/***********************/
#define CSK_BLOCK_OFFSET 0x420
#define PUB_KEY_SIZE 524
#define CSK_KEY_NUM 16
#define CSK_BLOCK_SIGN_OFFSET (CSK_BLOCK_OFFSET + (PUB_KEY_SIZE * CSK_KEY_NUM))
/* 16 keys + 256 bytes long signature */
#define CSK_BLOCK_SIZE ((PUB_KEY_SIZE * CSK_KEY_NUM) + 0x100)

#define BOOTROM_SIZE (64 * 1024)
#define EXT_HDR_BASE_SIZE (sizeof(headExtBHR_t) + sizeof(tailExtBHR_t))
/* MAX size of entire headers block */
#define MAX_HEADER_SIZE (192 * 1024)
#define MAX_TWSI_HDR_SIZE                                                      \
	(60 * 1024) /* MAX eeprom is 64K & leave 4K for image and header */

/* Boot Type - block ID */
#define IBR_HDR_I2C_ID 0x4D
#define IBR_HDR_SPI_ID 0x5A
#define IBR_HDR_NAND_ID 0x8B
#define IBR_HDR_SATA_ID 0x78
#define IBR_HDR_PEX_ID 0x9C
#define IBR_HDR_MMC_ID 0xAE
#define IBR_HDR_UART_ID 0x69
#define IBR_DEF_ATTRIB 0x00

/* ROM flags */
#define BHR_FLAG_PRINT_EN 0x01
#define BHR_FLAG_RESERVED1 0x02
#define BHR_FLAG_RESERVED2 0x04
#define BHR_FLAG_RESERVED3 0x08
#define BHR_FLAG_RESERVED4 0x10
#define BHR_FLAG_RESERVED5 0x20
#define BHR_FLAG_RESERVED6 0x40
#define BHR_FLAG_RESERVED7 0x80

/* ROM options */
#define BHR_OPT_BAUDRATE_OFFS 0x0
#define BHR_OPT_BAUDRATE_MASK (0x7 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_DEFAULT (0x0 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_2400 (0x1 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_4800 (0x2 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_9600 (0x3 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_19200 (0x4 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_38400 (0x5 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_57600 (0x6 << BHR_OPT_BAUDRATE_OFFS)
#define BHR_OPT_BAUD_115200 (0x7 << BHR_OPT_BAUDRATE_OFFS)

#define BHR_OPT_UART_PORT_OFFS 0x3
#define BHR_OPT_UART_PORT_MASK (0x3 << BHR_OPT_UART_PORT_OFFS)

#define BHR_OPT_UART_MPPS_OFFS 0x5
#define BHR_OPT_UART_MPPS_MASK (0x7 << BHR_OPT_UART_MPPS_OFFS)

typedef struct _mvCpuArmClk {
	MV_U32 cpuClk; /* CPU clock MHz */
	MV_U32 ddrClk; /* DDR clock MHz */
	MV_U32 l2cClk; /* L2 cache clock MHz */
} MV_CPU_ARM_CLK;

#endif /* MV_ASMLANGUAGE */
#endif /* _INC_BOOTSTRAP_H */
