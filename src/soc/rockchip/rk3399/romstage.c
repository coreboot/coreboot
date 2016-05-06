/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
 */

#include <arch/cache.h>
#include <arch/cpu.h>
#include <arch/exception.h>
#include <arch/io.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <soc/addressmap.h>
#include <soc/grf.h>
#include <soc/mmu_operations.h>
#include <soc/pwm.h>
#include <soc/tsadc.h>
#include <soc/sdram.h>
#include <symbols.h>

static const uint64_t dram_size =
	(uint64_t)min((uint64_t)CONFIG_DRAM_SIZE_MB * MiB, MAX_DRAM_ADDRESS);

static void init_dvs_outputs(void)
{
	uint32_t i;

	write32(&rk3399_grf->iomux_pwm_0, IOMUX_PWM_0);		/* GPU */
	write32(&rk3399_grf->iomux_pwm_1, IOMUX_PWM_1);		/* Big */
	write32(&rk3399_pmugrf->iomux_pwm_2, IOMUX_PWM_2);	/* Little */
	write32(&rk3399_pmugrf->iomux_pwm_3a, IOMUX_PWM_3_A);	/* Centerlog */

	/*
	 * Notes:
	 *
	 * design_min = 0.8
	 * design_max = 1.5
	 *
	 * period = 3333     # 300 kHz
	 * volt = 1.1
	 *
	 * # Intentionally round down (higher volt) to be safe.
	 * int((period / (design_max - design_min)) * (design_max - volt))
	 *
	 * Tested on kevin rev0 board 82 w/ all 4 PWMs:
	 *
	 *   period = 3333, volt = 1.1: 1904 -- Worked for me!
	 *   period = 3333, volt = 1.0: 2380 -- Bad
	 *   period = 3333, volt = 0.9: 2856 -- Bad
	 *
	 *   period = 25000, volt = 1.1: 14285 -- Bad
	 *   period = 25000, volt = 1.0: 17857 -- Bad
	 *
	 * TODO: Almost certainly we don't need all 4 PWMs set to the same
	 * thing.  We should experiment
	 */
	for (i = 0; i < 4; i++)
		pwm_init(i, 3333, 1904);
}

void main(void)
{
	console_init();
	tsadc_init(TSHUT_POL_HIGH);
	exception_init();

	/* Init DVS to conservative values. */
	init_dvs_outputs();

	sdram_init(get_sdram_config());

	mmu_config_range((void *)0, (uintptr_t)dram_size, CACHED_MEM);
	mmu_config_range(_dma_coherent, _dma_coherent_size, UNCACHED_MEM);
	cbmem_initialize_empty();
	run_ramstage();
}
