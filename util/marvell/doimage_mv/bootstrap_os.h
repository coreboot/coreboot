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
#ifndef _INC_BOOTSTRAP_OS_H
#define _INC_BOOTSTRAP_OS_H

/* BE/ LE swap for Asm */
#if defined(MV_CPU_LE)

#define htoll(x) x
#define HTOLL(sr, tr)

#elif defined(MV_CPU_BE)

#define htoll(x)                                                               \
	((((x)&0x00ff) << 24) | (((x)&0xff00) << 8) | (((x) >> 8) & 0xff00) |  \
	 (((x) >> 24) & 0x00ff))

#define HTOLL(sr, temp) do {	/*sr   = A  ,B  ,C  ,D    */        \
	eor temp, sr, sr, ROR #16; /*temp = A^C,B^D,C^A,D^B  */     \
	bic temp, temp, #0xFF0000; /*temp = A^C,0  ,C^A,D^B  */      \
	mov sr, sr, ROR #8;	/*sr   = D  ,A  ,B  ,C    */         \
	eor sr, sr, temp, LSR #8   /*sr   = D  ,C  ,B  ,A    */	    \
	} while (0)

#endif

#define MV_REG_READ_ASM(toReg, tmpReg, regOffs)	do {                   \
	ldr tmpReg, = (INTER_REGS_BASE + regOffs);                     \
	ldr toReg, [tmpReg];                                           \
	HTOLL(toReg, tmpReg)						\
	} while (0)

#define MV_REG_WRITE_ASM(fromReg, tmpReg, regOffs)                             \
	HTOLL(fromReg, tmpReg);                                                \
	ldr tmpReg, = (INTER_REGS_BASE + regOffs);                             \
	str fromReg, [tmpReg]

#define MV_DV_REG_READ_ASM(toReg, tmpReg, regOffs)                             \
	ldr tmpReg, = (CFG_DFL_MV_REGS + regOffs);                             \
	ldr toReg, [tmpReg];                                                   \
	HTOLL(toReg, tmpReg)

#define MV_DV_REG_WRITE_ASM(fromReg, tmpReg, regOffs)                          \
	HTOLL(fromReg, tmpReg);                                                \
	ldr tmpReg, = (CFG_DFL_MV_REGS + regOffs);                             \
	str fromReg, [tmpReg]

/* Defines */

/* The following is a list of Marvell status    */
#define MV_ERROR (-1)
#define MV_OK (0x00)		  /* Operation succeeded                   */
#define MV_FAIL (0x01)		  /* Operation failed                      */
#define MV_BAD_VALUE (0x02)       /* Illegal value (general)               */
#define MV_OUT_OF_RANGE (0x03)    /* The value is out of range             */
#define MV_BAD_PARAM (0x04)       /* Illegal parameter in function called  */
#define MV_BAD_PTR (0x05)	 /* Illegal pointer value                 */
#define MV_BAD_SIZE (0x06)	/* Illegal size                          */
#define MV_BAD_STATE (0x07)       /* Illegal state of state machine        */
#define MV_SET_ERROR (0x08)       /* Set operation failed                  */
#define MV_GET_ERROR (0x09)       /* Get operation failed                  */
#define MV_CREATE_ERROR (0x0A)    /* Fail while creating an item           */
#define MV_NOT_FOUND (0x0B)       /* Item not found                        */
#define MV_NO_MORE (0x0C)	 /* No more items found                   */
#define MV_NO_SUCH (0x0D)	 /* No such item                          */
#define MV_TIMEOUT (0x0E)	 /* Time Out                              */
#define MV_NO_CHANGE (0x0F)       /* Parameter(s) is already in this value */
#define MV_NOT_SUPPORTED (0x10)   /* This request is not support           */
#define MV_NOT_IMPLEMENTED (0x11) /* Request supported but not implemented */
#define MV_NOT_INITIALIZED (0x12) /* The item is not initialized           */
#define MV_NO_RESOURCE (0x13)     /* Resource not available (memory ...)   */
#define MV_FULL (0x14)		  /* Item is full (Queue or table etc...)  */
#define MV_EMPTY (0x15)		  /* Item is empty (Queue or table etc...) */
#define MV_INIT_ERROR (0x16)      /* Error occurred while INIT process      */
#define MV_HW_ERROR (0x17)	/* Hardware error                        */
#define MV_TX_ERROR (0x18)	/* Transmit operation not succeeded      */
#define MV_RX_ERROR (0x19)	/* Receive operation not succeeded       */
#define MV_NOT_READY (0x1A)       /* The other side is not ready yet       */
#define MV_ALREADY_EXIST (0x1B)   /* Tried to create existing item         */
#define MV_OUT_OF_CPU_MEM (0x1C)  /* Cpu memory allocation failed.         */
#define MV_NOT_STARTED (0x1D)     /* Not started yet         */
#define MV_BUSY (0x1E)		  /* Item is busy.                         */
#define MV_TERMINATE (0x1F)       /* Item terminates it's work.            */
#define MV_NOT_ALIGNED (0x20)     /* Wrong alignment                       */
#define MV_NOT_ALLOWED (0x21)     /* Operation NOT allowed                 */
#define MV_WRITE_PROTECT (0x22)   /* Write protected                       */

#define MV_INVALID (int)(-1)

#define MV_FALSE 0
#define MV_TRUE (!(MV_FALSE))

#ifndef NULL
#define NULL ((void *)0)
#endif

#ifndef MV_ASMLANGUAGE
/* typedefs */

typedef char MV_8;
typedef unsigned char MV_U8;

typedef int MV_32;
typedef unsigned int MV_U32;

typedef short MV_16;
typedef unsigned short MV_U16;
/*
#ifdef MV_PPC64
typedef long		MV_64;
typedef unsigned long	MV_U64;
#else
typedef long long		MV_64;
typedef unsigned long long	MV_U64;
#endif
*/
typedef long MV_LONG;		/* 32/64 */
typedef unsigned long MV_ULONG; /* 32/64 */

typedef int MV_STATUS;
typedef int MV_BOOL;
/*typedef void    MV_VOID;*/
#define MV_VOID void
typedef float MV_FLOAT;

typedef int (*MV_FUNCPTR)(void);       /* ptr to function returning int   */
typedef void (*MV_VOIDFUNCPTR)(void);  /* ptr to function returning void  */
typedef double (*MV_DBLFUNCPTR)(void); /* ptr to function returning double*/
typedef float (*MV_FLTFUNCPTR)(void);  /* ptr to function returning float */

typedef MV_U32 MV_KHZ;
typedef MV_U32 MV_MHZ;
typedef MV_U32 MV_HZ;
#if defined(_HOST_COMPILER)
#define __MV_PACKED
#else
#define __MV_PACKED /*__packed*/
#endif

#endif /* MV_ASMLANGUAGE */

/* Bit field definitions */
#define NO_BIT 0x00000000
#define BIT0 0x00000001
#define BIT1 0x00000002
#define BIT2 0x00000004
#define BIT3 0x00000008
#define BIT4 0x00000010
#define BIT5 0x00000020
#define BIT6 0x00000040
#define BIT7 0x00000080
#define BIT8 0x00000100
#define BIT9 0x00000200
#define BIT10 0x00000400
#define BIT11 0x00000800
#define BIT12 0x00001000
#define BIT13 0x00002000
#define BIT14 0x00004000
#define BIT15 0x00008000
#define BIT16 0x00010000
#define BIT17 0x00020000
#define BIT18 0x00040000
#define BIT19 0x00080000
#define BIT20 0x00100000
#define BIT21 0x00200000
#define BIT22 0x00400000
#define BIT23 0x00800000
#define BIT24 0x01000000
#define BIT25 0x02000000
#define BIT26 0x04000000
#define BIT27 0x08000000
#define BIT28 0x10000000
#define BIT29 0x20000000
#define BIT30 0x40000000
#define BIT31 0x80000000

/* includes */
#define _1K 0x00000400
#define _4K 0x00001000
#define _8K 0x00002000
#define _16K 0x00004000
#define _32K 0x00008000
#define _64K 0x00010000
#define _128K 0x00020000
#define _256K 0x00040000
#define _512K 0x00080000
/* Sizes */
#define _1M 0x00100000
#define _2M 0x00200000
#define _4M 0x00400000
#define _8M 0x00800000
#define _16M 0x01000000
#define _32M 0x02000000
#define _64M 0x04000000
#define _128M 0x08000000
#define _256M 0x10000000
#define _512M 0x20000000
#define _1G 0x40000000
#define _2G 0x80000000
/* Speed */
#define _25MHZ 25000000
#define _125MHZ 125000000
#define _133MHZ 133333333
#define _150MHZ 150000000
#define _166MHZ 166666667
#define _200MHZ 200000000
#define _250MHZ 250000000

/* Swap tool */

/* 16bit nibble swap. For example 0x1234 -> 0x2143                          */
#define MV_NIBBLE_SWAP_16BIT(X)                                                \
	(((X & 0xf) << 4) | ((X & 0xf0) >> 4) | ((X & 0xf00) << 4) |           \
	 ((X & 0xf000) >> 4))

/* 32bit nibble swap. For example 0x12345678 -> 0x21436587                  */
#define MV_NIBBLE_SWAP_32BIT(X)                                                \
	(((X & 0xf) << 4) | ((X & 0xf0) >> 4) | ((X & 0xf00) << 4) |           \
	 ((X & 0xf000) >> 4) | ((X & 0xf0000) << 4) | ((X & 0xf00000) >> 4) |  \
	 ((X & 0xf000000) << 4) | ((X & 0xf0000000) >> 4))

/* 16bit byte swap. For example 0x1122 -> 0x2211                            */
#define MV_BYTE_SWAP_16BIT(X) ((((X)&0xff) << 8) | (((X)&0xff00) >> 8))

/* 32bit byte swap. For example 0x11223344 -> 0x44332211                    */
#define MV_BYTE_SWAP_32BIT(X)                                                  \
	((((X)&0xff) << 24) | (((X)&0xff00) << 8) | (((X)&0xff0000) >> 8) |    \
	 (((X)&0xff000000) >> 24))

/* 64bit byte swap. For example 0x11223344.55667788 -> 0x88776655.44332211  */
#define MV_BYTE_SWAP_64BIT(X)                                                  \
	((l64)((((X)&0xffULL) << 56) | (((X)&0xff00ULL) << 40) |               \
	       (((X)&0xff0000ULL) << 24) | (((X)&0xff000000ULL) << 8) |        \
	       (((X)&0xff00000000ULL) >> 8) |                                  \
	       (((X)&0xff0000000000ULL) >> 24) |                               \
	       (((X)&0xff000000000000ULL) >> 40) |                             \
	       (((X)&0xff00000000000000ULL) >> 56)))

/* Endianness macros.                                                        */
#if defined(MV_CPU_LE)
#define MV_16BIT_LE(X) (X)
#define MV_32BIT_LE(X) (X)
#define MV_64BIT_LE(X) (X)
#define MV_16BIT_BE(X) MV_BYTE_SWAP_16BIT(X)
#define MV_32BIT_BE(X) MV_BYTE_SWAP_32BIT(X)
#define MV_64BIT_BE(X) MV_BYTE_SWAP_64BIT(X)
#elif defined(MV_CPU_BE)
#define MV_16BIT_LE(X) MV_BYTE_SWAP_16BIT(X)
#define MV_32BIT_LE(X) MV_BYTE_SWAP_32BIT(X)
#define MV_64BIT_LE(X) MV_BYTE_SWAP_64BIT(X)
#define MV_16BIT_BE(X) (X)
#define MV_32BIT_BE(X) (X)
#define MV_64BIT_BE(X) (X)
#else
#error "CPU endianness isn't defined!\n"
#endif

#ifndef MV_ASMLANGUAGE
/* Get the min between 'a' or 'b'                                           */
#define MV_MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Marvell controller register read/write macros */
#define CPU_PHY_MEM(x) ((MV_U32) x)
#define CPU_MEMIO_CACHED_ADDR(x) ((void *) x)
#define CPU_MEMIO_UNCACHED_ADDR(x) ((void *) x)

/* CPU architecture dependent 32, 16, 8 bit read/write IO addresses */
#define MV_MEMIO32_WRITE(addr, data)                                           \
	((*((volatile unsigned int *)(addr))) = ((unsigned int)(data)))

#define MV_MEMIO32_READ(addr) ((*((volatile unsigned int *)(addr))))

#define MV_MEMIO16_WRITE(addr, data)                                           \
	((*((volatile unsigned short *)(addr))) = ((unsigned short)(data)))

#define MV_MEMIO16_READ(addr) ((*((volatile unsigned short *)(addr))))

#define MV_MEMIO8_WRITE(addr, data)                                            \
	((*((volatile unsigned char *)(addr))) = ((unsigned char)(data)))

#define MV_MEMIO8_READ(addr) ((*((volatile unsigned char *)(addr))))

/* No Fast Swap implementation (in assembler) for ARM */
#define MV_32BIT_LE_FAST(val) MV_32BIT_LE(val)
#define MV_16BIT_LE_FAST(val) MV_16BIT_LE(val)
#define MV_32BIT_BE_FAST(val) MV_32BIT_BE(val)
#define MV_16BIT_BE_FAST(val) MV_16BIT_BE(val)

/* 32 and 16 bit read/write in big/little endian mode */

/* 16bit write in little endian mode */
#define MV_MEMIO_LE16_WRITE(addr, data)                                        \
	MV_MEMIO16_WRITE(addr, MV_16BIT_LE_FAST(data))

/* 16bit read in little endian mode */
static inline MV_U16 MV_MEMIO_LE16_READ(void *addr)
{
	MV_U16 data;
	MV_U16 *addr1 = (MV_U16 *)addr;

	data = (MV_U16)MV_MEMIO16_READ(addr1);

	return (MV_U16)MV_16BIT_LE_FAST(data);
}

/* 32bit write in little endian mode */
#define MV_MEMIO_LE32_WRITE(addr, data)                                        \
	MV_MEMIO32_WRITE(addr, MV_32BIT_LE_FAST(data))

/* 32bit read in little endian mode */
static inline MV_U32 MV_MEMIO_LE32_READ(void *addr)
{
	MV_U32 data;
	MV_U32 *addr1 = (MV_U32 *)addr;

	data = (MV_U32)MV_MEMIO32_READ(addr1);

	return (MV_U32)MV_32BIT_LE_FAST(data);
}

/* Flash APIs */
#define MV_FL_8_READ MV_MEMIO8_READ
#define MV_FL_16_READ MV_MEMIO_LE16_READ
#define MV_FL_32_READ MV_MEMIO_LE32_READ
#define MV_FL_8_DATA_READ MV_MEMIO8_READ
#define MV_FL_16_DATA_READ MV_MEMIO16_READ
#define MV_FL_32_DATA_READ MV_MEMIO32_READ
#define MV_FL_8_WRITE MV_MEMIO8_WRITE
#define MV_FL_16_WRITE MV_MEMIO_LE16_WRITE
#define MV_FL_32_WRITE MV_MEMIO_LE32_WRITE
#define MV_FL_8_DATA_WRITE MV_MEMIO8_WRITE
#define MV_FL_16_DATA_WRITE MV_MEMIO16_WRITE
#define MV_FL_32_DATA_WRITE MV_MEMIO32_WRITE

/* CPU cache information */
#define CPU_I_CACHE_LINE_SIZE 32 /* 2do: replace 32 with linux core macro */
#define CPU_D_CACHE_LINE_SIZE 32 /* 2do: replace 32 with linux core macro */

#define MV_REG_VALUE(offset) (MV_MEMIO32_READ((INTER_REGS_BASE | (offset))))

#define MV_REG_READ(offset) (MV_MEMIO_LE32_READ(INTER_REGS_BASE | (offset)))

#define MV_REG_WRITE(offset, val)                                              \
	MV_MEMIO_LE32_WRITE((INTER_REGS_BASE | (offset)), (val))

#define MV_REG_BYTE_READ(offset) (MV_MEMIO8_READ((INTER_REGS_BASE | (offset))))

#define MV_REG_BYTE_WRITE(offset, val)                                         \
	MV_MEMIO8_WRITE((INTER_REGS_BASE | (offset)), (val))

#define MV_REG_SHORT_READ(offset)                                              \
	(MV_MEMIO_LE16_READ(INTER_REGS_BASE | (offset)))

#define MV_REG_BIT_SET(offset, bitMask)                                        \
	(MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)),                        \
			  (MV_MEMIO32_READ((INTER_REGS_BASE | (offset))) |     \
			   MV_32BIT_LE_FAST((bitMask)))))

#define MV_REG_BIT_RESET(offset, bitMask)                                      \
	(MV_MEMIO32_WRITE((INTER_REGS_BASE | (offset)),                        \
			  (MV_MEMIO32_READ((INTER_REGS_BASE | (offset))) &     \
			   MV_32BIT_LE_FAST(~(bitMask)))))

#define mvOsUDelay uDelay
#define mvOsMDelay(msec) uDelay(msec * 1000)
#define mvOsDelay(sec) mvOsMDelay(sec * 1000)

#define u32 MV_U32
#define u16 MV_U16
#define u8 MV_U8
#define s16 short
#define s32 long
#define s8 char

#define UINT32 MV_U32
#define UINT16 MV_U16
#define UINT8 MV_U8

typedef UINT32 * PUINT32;
typedef UINT16 * PUINT16;
typedef UINT8 * PUINT8;

#define TRUE MV_TRUE
#define FALSE MV_FALSE

#endif /* #ifndef MV_ASMLANGUAGE */

#endif /* _INC_BOOTSTRAP_OS_H */
