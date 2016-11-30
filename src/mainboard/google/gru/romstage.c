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
#include <arch/mmu.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <romstage_handoff.h>
#include <soc/addressmap.h>
#include <soc/mmu_operations.h>
#include <soc/tsadc.h>
#include <soc/sdram.h>
#include <symbols.h>
#include <soc/usb.h>
#include <stdlib.h>

#include "pwm_regulator.h"

static void init_dvs_outputs(void)
{
	pwm_regulator_configure(PWM_REGULATOR_GPU, 900);
	pwm_regulator_configure(PWM_REGULATOR_BIG, 900);
	pwm_regulator_configure(PWM_REGULATOR_CENTERLOG, 950);
}

static void prepare_usb(void)
{
	/* Do dwc3 core soft reset and phy reset. Kick these resets
	 * off early so they get at least 100ms to settle. */
	reset_usb_otg0();
	reset_usb_otg1();
}

void main(void)
{
	console_init();
	tsadc_init(TSHUT_POL_HIGH);
	exception_init();

	/* Init DVS to conservative values. */
	init_dvs_outputs();

	prepare_usb();

	sdram_init(get_sdram_config());

	mmu_config_range((void *)0, (uintptr_t)sdram_size_mb() * MiB,
			 CACHED_MEM);
	mmu_config_range(_dma_coherent, _dma_coherent_size, UNCACHED_MEM);
	cbmem_initialize_empty();
	run_ramstage();
}
