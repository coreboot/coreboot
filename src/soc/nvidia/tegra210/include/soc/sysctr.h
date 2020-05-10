/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_NVIDIA_TEGRA210_SYSCTR_H__
#define __SOC_NVIDIA_TEGRA210_SYSCTR_H__

#include <stdint.h>

enum {
	SYSCTR_CNTCR_EN = 1 << 0,
	SYSCTR_CNTCR_HDBG = 1 << 1,
	SYSCTR_CNTCR_FCREQ = 1 << 8
};

struct sysctr_regs {
	uint32_t cntcr;
	uint32_t cntsr;
	uint32_t cntcv0;
	uint32_t cntcv1;
	uint8_t _rsv0[0x10];
	uint32_t cntfid0;
	uint32_t cntfid1;
	uint8_t _rsv1[0xfa8];
	uint32_t counterid4;
	uint32_t counterid5;
	uint32_t counterid6;
	uint32_t counterid7;
	uint32_t counterid0;
	uint32_t counterid1;
	uint32_t counterid2;
	uint32_t counterid3;
	uint32_t counterid8;
	uint32_t counterid9;
	uint32_t counterid10;
	uint32_t counterid11;
};
check_member(sysctr_regs, counterid11, 0xffc);

#endif	/* __SOC_NVIDIA_TEGRA210_SYSCTR_H__ */
