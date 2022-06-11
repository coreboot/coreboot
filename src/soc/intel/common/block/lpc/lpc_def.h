/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_COMMON_BLOCK_LPC_DEF_H_
#define _SOC_COMMON_BLOCK_LPC_DEF_H_

#define LPC_SERIRQ_CTL			0x64
#define  LPC_SCNT_EN			(1 << 7)
#define  LPC_SCNT_MODE			(1 << 6)
#define LPC_IO_DECODE			0x80
#define  LPC_IOD_FDD_RANGE		(0 << 12)
#define  LPC_IOD_FDD_RANGE_MASK		(1 << 12)
#define  LPC_IOD_LPT_RANGE		(0 << 8)
#define  LPC_IOD_LPT_RANGE_MASK		(3 << 8)
#define  LPC_IOD_COMA_RANGE_MASK	(7 << 0)
#define  LPC_IOD_COMB_RANGE_MASK	(7 << 4)
#define  LPC_IOD_COMA_RANGE		(0 << 0) /* 0x3F8 - 0x3FF COMA */
#define  LPC_IOD_COMB_RANGE		(1 << 4) /* 0x2F8 - 0x2FF COMB */
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
#define  LPC_BC_WPD			(1 << 0) /* WPD */
#define  LPC_BC_EISS			(1 << 5) /* EISS */
#define LPC_PCCTL			0xE0 /* PCI Clock Control */
#define  LPC_PCCTL_CLKRUN_EN		(1 << 0)

#endif /* _SOC_COMMON_BLOCK_LPC_DEF_H_ */
