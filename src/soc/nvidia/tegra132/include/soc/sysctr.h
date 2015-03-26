/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __SOC_NVIDIA_TEGRA132_SYSCTR_H__
#define __SOC_NVIDIA_TEGRA132_SYSCTR_H__

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

#endif	/* __SOC_NVIDIA_TEGRA132_SYSCTR_H__ */
