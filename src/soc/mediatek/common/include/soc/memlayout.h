/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <memlayout.h>
#include <arch/header.ld>

/*
 * SRAM_L2C is the half part of L2 cache that we borrow to be used as SRAM.
 * It will be returned before starting the ramstage.
 * SRAM_L2C and SRAM can be cached, but only SRAM is DMA-able.
 */
#define SRAM_L2C_START(addr) REGION_START(sram_l2c, addr)
#define SRAM_L2C_END(addr) REGION_END(sram_l2c, addr)
#define DRAM_INIT_CODE(addr, size) \
	REGION(dram_init_code, addr, size, 64K)

#define DRAM_DMA(addr, size) \
	REGION(dram_dma, addr, size, 4K) \
	_ = ASSERT(size % 4K == 0, \
		"DRAM DMA buffer should be multiple of smallest page size (4K)!");

#define FSP_RAMSTAGE_INIT_CODE(addr, size) \
	REGION(fsp_ramstage_init_code, addr, size, 64K)

#define EARLY_INIT(addr, size) \
	REGION(early_init_data, addr, size, 4)
