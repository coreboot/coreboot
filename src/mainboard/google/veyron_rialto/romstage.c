/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <arch/exception.h>
#include <device/mmio.h>
#include <armv7.h>
#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/sdram.h>
#include <soc/clock.h>
#include <soc/pwm.h>
#include <soc/grf.h>
#include <soc/rk808.h>
#include <soc/tsadc.h>
#include <symbols.h>
#include <timestamp.h>
#include <types.h>

#include "board.h"

static void regulate_vdd_log(unsigned int mv)
{
	unsigned int duty_ns;
	const u32 period_ns = 2000;	/* pwm period: 2000ns */
	const u32 max_regulator_mv = 1350;	/* 1.35V */
	const u32 min_regulator_mv = 870;	/* 0.87V */

	write32(&rk3288_grf->iomux_pwm1, IOMUX_PWM1);

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
	timestamp_add_now(TS_ROMSTAGE_START);

	console_init();
	exception_init();
	configure_l2ctlr();
	tsadc_init();

	/* Need to power cycle SD card to ensure it is properly reset. */
	sdmmc_power_off();

	/* vdd_log 1200mv is enough for ddr run 666Mhz */
	regulate_vdd_log(1200);

	timestamp_add_now(TS_INITRAM_START);

	sdram_init(get_sdram_config());

	timestamp_add_now(TS_INITRAM_END);

	/* Now that DRAM is up, add mappings for it and DMA coherency buffer. */
	mmu_config_range((uintptr_t)_dram/MiB,
			 sdram_size_mb(), DCACHE_WRITEBACK);
	mmu_config_range((uintptr_t)_dma_coherent/MiB,
			 REGION_SIZE(dma_coherent)/MiB, DCACHE_OFF);

	cbmem_initialize_empty();

	run_ramstage();
}
