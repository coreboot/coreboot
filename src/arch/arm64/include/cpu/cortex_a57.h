/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __ARCH_ARM64_CORTEX_A57_H__
#define __ARCH_ARM64_CORTEX_A57_H__

#define CPUACTLR_EL1	s3_1_c15_c2_0
#define BTB_INVALIDATE	(1 << 0)

#define CPUECTLR_EL1	S3_1_c15_c2_1
#define SMPEN_SHIFT	6

/* Cortex MIDR[15:4] PN */
#define CORTEX_A53_PN	0xd03

/* Double lock control bit */
#define OSDLR_DBL_LOCK_BIT	1

#endif /* __ARCH_ARM64_CORTEX_A57_H__ */
