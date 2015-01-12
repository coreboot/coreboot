/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/exception.h>
#include <arch/stages.h>
#include <armv7.h>
#include <assert.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <soc/sdram.h>
#include <soc/clock.h>
#include <soc/pwm.h>
#include <soc/grf.h>
#include <soc/rk808.h>
#include <soc/tsadc.h>
#include <stdlib.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

static void regulate_vdd_log(unsigned int mv)
{
	unsigned int duty_ns;
	const u32 period_ns = 2000;	/* pwm period: 2000ns */
	const u32 max_regulator_mv = 1350;	/* 1.35V */
	const u32 min_regulator_mv = 870;	/* 0.87V */

	writel(IOMUX_PWM1, &rk3288_grf->iomux_pwm1);

	assert((mv >= min_regulator_mv) && (mv <= max_regulator_mv));

	duty_ns = (max_regulator_mv - mv) * period_ns /
			(max_regulator_mv - min_regulator_mv);

	pwm_init(1, period_ns, duty_ns);
}

static void configure_l2ctlr(void)
{
	uint32_t l2ctlr;

	l2ctlr = read_l2ctlr();
	l2ctlr &= 0xfffc0000; /* clear bit0~bit17 */

	/*
	* Data RAM write latency: 2 cycles
	* Data RAM read latency: 2 cycles
	* Data RAM setup latency: 1 cycle
	* Tag RAM write latency: 1 cycle
	* Tag RAM read latency: 1 cycle
	* Tag RAM setup latency: 1 cycle
	*/
	l2ctlr |= (1 << 3 | 1 << 0);
	write_l2ctlr(l2ctlr);
}

static void sdmmc_power_off(void)
{
	rk808_configure_ldo(4, 0); /* VCCIO_SD */
	rk808_configure_ldo(5, 0); /* VCC33_SD */
}

void main(void)
{
#if CONFIG_COLLECT_TIMESTAMPS
	uint64_t start_romstage_time;
	uint64_t before_dram_time;
	uint64_t after_dram_time;
	uint64_t base_time = timestamp_get();
	start_romstage_time = timestamp_get();
#endif

	console_init();
	configure_l2ctlr();
	tsadc_init();

	/* Need to power cycle SD card to ensure it is properly reset. */
	sdmmc_power_off();

	/* vdd_log 1200mv is enough for ddr run 666Mhz */
	regulate_vdd_log(1200);
#if CONFIG_COLLECT_TIMESTAMPS
	before_dram_time = timestamp_get();
#endif
	sdram_init(get_sdram_config());
#if CONFIG_COLLECT_TIMESTAMPS
	after_dram_time = timestamp_get();
#endif

	/* Now that DRAM is up, add mappings for it and DMA coherency buffer. */
	mmu_config_range((uintptr_t)_dram/MiB,
			 CONFIG_DRAM_SIZE_MB, DCACHE_WRITEBACK);
	mmu_config_range((uintptr_t)_dma_coherent/MiB,
			 _dma_coherent_size/MiB, DCACHE_OFF);

	cbmem_initialize_empty();

#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(base_time);
	timestamp_add(TS_START_ROMSTAGE, start_romstage_time);
	timestamp_add(TS_BEFORE_INITRAM, before_dram_time);
	timestamp_add(TS_AFTER_INITRAM, after_dram_time);
	timestamp_add_now(TS_END_ROMSTAGE);
#endif

#if IS_ENABLED(CONFIG_VBOOT_VERIFY_FIRMWARE)
	void *entry = vboot2_load_ramstage();
	if (entry != NULL)
		stage_exit(entry);
#endif

	run_ramstage();
}
