/*
 *  linux/include/asm-arm/byteorder.h
 *
 * ARM Endian-ness.  In little endian mode, the data bus is connected such
 * that byte accesses appear as:
 *  0 = d0...d7, 1 = d8...d15, 2 = d16...d23, 3 = d24...d31
 * and word accesses (data or instruction) appear as:
 *  d0...d31
 *
 * When in big endian mode, byte accesses appear as:
 *  0 = d24...d31, 1 = d16...d23, 2 = d8...d15, 3 = d0...d7
 * and word accesses (data or instruction) appear as:
 *  d0...d31
 */
#ifndef __ASM_ARM_BYTEORDER_H
#define __ASM_ARM_BYTEORDER_H


#include <arch/types.h>

#if !defined(__STRICT_ANSI__)
#  define __BYTEORDER_HAS_U64__
#  define __SWAB_64_THRU_32__
#endif

#ifdef __ARMEB__
#include <linux/byteorder/big_endian.h>
#else
#include <linux/byteorder/little_endian.h>
#endif

#endif
