/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_IOC_GPMR_H
#define SOC_INTEL_COMMON_BLOCK_IOC_GPMR_H
#include <assert.h>

#define GPMR_LPCLGIR1		0x7a30
#define GPMR_LPCGMR		0x7a40
#define GPMR_GCS		0x7a4c
#define  GPMR_GCS_BILD		0x1
#define GPMR_LPCIOD		0x7a70
#define GPMR_LPCIOE		0x7a74
#define GPMR_TCOBASE		0x7a78
#define  GPMR_TCOEN		(1 << 1)

#define MAX_GPMR_REGS		3

#define GPMR_OFFSET(x)		(0x7a7c + (x) * 8)
#define  GPMR_LIMIT_MASK	0xffff0000
#define  GPMR_BASE_SHIFT	16
#define  GPMR_BASE_MASK		0xffff

#define GPMR_DID_OFFSET(x)	(0x7a80 + (x) * 8)
#define  GPMR_EN		BIT(31)

#define GPMR_DMICTL		dead_code_t(unsigned int)
#define  GPMR_DMICTL_SRLOCK	dead_code_t(unsigned int)

#endif
