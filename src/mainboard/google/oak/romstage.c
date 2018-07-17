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
#include <boardid.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <symbols.h>
#include <timer.h>
#include <timestamp.h>

#include <soc/emi.h>
#include <soc/mmu_operations.h>
#include <soc/mt6391.h>
#include <soc/pll.h>
#include <soc/rtc.h>

void main(void)
{
	int stabilize_usec;
	struct stopwatch sw;

	timestamp_add_now(TS_START_ROMSTAGE);

	/* init uart baudrate when pll on */
	console_init();
	exception_init();

	rtc_boot();

	/* Raise CPU voltage to allow higher frequency */
	stabilize_usec = mt6391_configure_ca53_voltage(1125000);

	stopwatch_init_usecs_expire(&sw, stabilize_usec);

	/* init memory */
	mt_mem_init(get_sdram_config());

	stopwatch_wait_until_expired(&sw);

	/* Set to maximum frequency */
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		mt_pll_raise_ca53_freq(1600 * MHz);
	else
		mt_pll_raise_ca53_freq(1700 * MHz);

	mtk_mmu_after_dram();

	/* should be called after memory init */
	cbmem_initialize_empty();

	run_ramstage();
}
