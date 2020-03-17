/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_COMMON_BLOCK_LPC_DEF_H_
#define _SOC_COMMON_BLOCK_LPC_DEF_H_

#define LPC_SERIRQ_CTL			0x64
#define  LPC_SCNT_EN			(1 << 7)
#define  LPC_SCNT_MODE			(1 << 6)
#define LPC_IO_DECODE			0x80
#define  LPC_IOD_COMA_RANGE             (0 << 0) /* 0x3F8 - 0x3FF COMA*/
#define  LPC_IOD_COMB_RANGE             (1 << 4) /* 0x2F8 - 0x2FF COMB*/
/* Use IO_<peripheral>_<IO port> style macros defined in lpc_lib.h
 * to enable decoding of I/O locations for a peripheral. */
#define LPC_IO_ENABLES			0x82
#define LPC_GENERIC_IO_RANGE(n)		((((n) & 0x3) * 4) + 0x84)
#define  LPC_LGIR_AMASK_MASK		(0xfc << 16)
#define  LPC_LGIR_ADDR_MASK		0xfffc
#define  LPC_LGIR_EN			(1 << 0)
#define LPC_LGIR_MAX_WINDOW_SIZE	256
#define LPC_GENERIC_MEM_RANGE		0x98
#define  LPC_LGMR_ADDR_MASK		0xffff0000
#define  LPC_LGMR_EN			(1 << 0)
#define LPC_LGMR_WINDOW_SIZE		(64 * KiB)
#define LPC_BIOS_CNTL			0xdc
#define  LPC_BC_BILD			(1 << 7) /* BILD */
#define  LPC_BC_LE			(1 << 1) /* LE */
#define  LPC_BC_EISS			(1 << 5) /* EISS */
#define LPC_PCCTL			0xE0 /* PCI Clock Control */
#define  LPC_PCCTL_CLKRUN_EN		(1 << 0)

#endif /* _SOC_COMMON_BLOCK_LPC_DEF_H_ */
