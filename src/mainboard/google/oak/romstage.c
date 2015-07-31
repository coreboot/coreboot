/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/io.h>
#include <arch/mmu.h>

#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <symbols.h>
#include <timestamp.h>

#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/rtc.h>

void main(void)
{
	timestamp_add_now(TS_START_ROMSTAGE);

	/* init uart baudrate when pll on */
	console_init();
	exception_init();

	rtc_boot();

	/* init memory */
	mt_mem_init(get_sdram_config());

	mt8173_mmu_after_dram();

	/* should be called after memory init */
	cbmem_initialize_empty();

	run_ramstage();
}
