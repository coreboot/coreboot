/*
 *  arch/arm/asmlib.h
 *
 *  Adapted from Linux arch/arm/include/assembler.h
 *
 *  Copyright (C) 1996-2000 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This file contains arm architecture specific defines
 *  for the different processors.
 *
 *  Do not include any C declarations in this file - it is included by
 *  assembler source.
 */

/*
 * WARNING: This file is *only* meant for memcpy.S and friends which were copied
 * from Linux and require some weird macros. It does unspeakable things like
 * redefining "push", so do *not* try to turn it into a general assembly macro
 * file, and keep it out of global include directories.
 */

#ifndef __ARM_ASMLIB_H__
#define __ARM_ASMLIB_H__

/*
 * Endian independent macros for shifting bytes within registers.
 */
#ifndef __ARMEB__
#define pull		lsr
#define push		lsl
#define get_byte_0	lsl #0
#define get_byte_1	lsr #8
#define get_byte_2	lsr #16
#define get_byte_3	lsr #24
#define put_byte_0	lsl #0
#define put_byte_1	lsl #8
#define put_byte_2	lsl #16
#define put_byte_3	lsl #24
#else
#define pull		lsl
#define push		lsr
#define get_byte_0	lsr #24
#define get_byte_1	lsr #16
#define get_byte_2	lsr #8
#define get_byte_3      lsl #0
#define put_byte_0	lsl #24
#define put_byte_1	lsl #16
#define put_byte_2	lsl #8
#define put_byte_3      lsl #0
#endif

/*
 * Data preload for architectures that support it
 */
#if __COREBOOT_ARM_ARCH__ >= 5
#define PLD(code...)	code
#else
#define PLD(code...)
#endif

/*
 * This can be used to enable code to cacheline align the destination
 * pointer when bulk writing to memory. Linux doesn't enable this except
 * for the "Feroceon" processor, so we better just leave it out.
 */
#define CALGN(code...)

#endif	/* __ARM_ASMLIB_H */
