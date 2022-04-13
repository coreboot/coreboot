/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_PCR_GPMR_H
#define SOC_INTEL_COMMON_BLOCK_PCR_GPMR_H

#define GPMR_LPCLGIR1		0x2730
#define GPMR_DMICTL		0x2234
#define  GPMR_DMICTL_SRLOCK	(1 << 31)
#define GPMR_LPCGMR		0x2740
#define GPMR_GCS		0x274c
#define  GPMR_GCS_BILD		(1 << 0)
#define GPMR_LPCIOD		0x2770
#define GPMR_LPCIOE		0x2774
#define GPMR_TCOBASE		0x2778
#define  GPMR_TCOEN		(1 << 1)

#define MAX_GPMR_REGS	3

#define GPMR_OFFSET(x)		(0x277c + (x) * 8)
#define  GPMR_LIMIT_MASK	0xffff0000
#define  GPMR_BASE_SHIFT	16
#define  GPMR_BASE_MASK		0xffff

#define GPMR_DID_OFFSET(x)	(0x2780 + (x) * 8)
#define  GPMR_EN		BIT(31)

#endif /* SOC_INTEL_COMMON_BLOCK_PCR_GPMR_H */
