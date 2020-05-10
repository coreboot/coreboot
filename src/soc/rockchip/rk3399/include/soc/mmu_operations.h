/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_ROCKCHIP_RK3399_MMU_H__
#define __SOC_ROCKCHIP_RK3399_MMU_H__

#include <arch/mmu.h>

enum {
	DEV_MEM		= MA_DEV | MA_S | MA_RW,
	CACHED_MEM	= MA_MEM | MA_NS | MA_RW,
	SECURE_MEM	= MA_MEM | MA_S  | MA_RW,
	UNCACHED_MEM	= MA_MEM | MA_NS | MA_RW | MA_MEM_NC,
};

#endif
