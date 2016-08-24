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
#include <boardid.h>
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
#include <soc/usb.h>

static const uint64_t dram_size =
	(uint64_t)min((uint64_t)CONFIG_DRAM_SIZE_MB * MiB, MAX_DRAM_ADDRESS);

static void init_dvs_outputs(void)
{
	int duty_ns;
	uint32_t i;
	uint32_t id;

	write32(&rk3399_grf->iomux_pwm_0, IOMUX_PWM_0);		/* GPU */
	write32(&rk3399_grf->iomux_pwm_1, IOMUX_PWM_1);		/* Big */
	write32(&rk3399_pmugrf->iomux_pwm_2, IOMUX_PWM_2);	/* Little */
	write32(&rk3399_pmugrf->iomux_pwm_3a, IOMUX_PWM_3_A);	/* Centerlog */

	/*
	 * Set up voltages for all DVS rails.
	 *
	 * LITTLE CPU: At the speed we're running at right now and on the
	 * early silicon, .9V is sane.  If/when we run faster, let's bump this.
	 *
	 * CENTER LOGIC: There are some claims that this should simply always
	 * be .9 V.  There are other claims that say that we need to adjust this
	 * dynamically depending on the memory frequency.  Until this is sorted
	 * out, it appears that .9 V works for the 800 MHz.
	 *
	 * BIG CPU / GPU: These aren't used in coreboot.  Init to .9V which is
	 * supposed to be a good default.
	 *
	 * Details:
	 *   design_min = 0.8
	 *   design_max = 1.5
	 *   period = 3337     # 300 kHz
	 *   volt = 1.1
	 *   # Intentionally round down (higher volt) to be safe.
	 *   int((period / (design_max - design_min)) * (design_max - volt))
	 *
	 * Apparently a period of 3333 is determined by EEs to be ideal for our
	 * board design / resistors / capacitors / regulators but due to
	 * clock dividers we actually get 3337.  Solving, we get:
	 *   period = 3337, volt = 1.1: 1906
	 *   period = 3337, volt = 1.0: 2383
	 *   period = 3337, volt = 0.9: 2860
	 */
	duty_ns = 2860; /* 0.9v */

	/* TODO: Clean all this up, implement proper pwm_regulator driver. */
	if (IS_ENABLED(CONFIG_BOARD_GOOGLE_KEVIN)) {
		id = board_id();
		if (id <= 2)
			duty_ns = 1906; /* 1.1v */
		else if (id == 3)
			duty_ns = 2621; /* 0.95v */
		else if (id >= 6) {
			/* GPU: 3337 * (12043 - 9000) / (12043 - 7984) = 2501 */
			pwm_init(0, 3337, 2501);
			/* BIG: 3337 * (12837 - 9000) / (12837 - 7985) = 2638 */
			pwm_init(1, 3337, 2638);
			/* LIT: 3337 * (12807 - 9000) / (12807 - 8009) = 2647 */
			pwm_init(2, 3337, 2647);
			/* CTR: 3337 * (10507 - 9500) / (10507 - 7996) = 1338 */
			pwm_init(3, 3337, 1338);
			return;
		}
	}

	for (i = 0; i < 4; i++)
		pwm_init(i, 3337, duty_ns);
}

static void prepare_usb(void)
{
	/*
	 * Do dwc3 core soft reset and phy reset. Kick these resets
	 * off early so they get at least 100ms to settle.
	 */
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

	mmu_config_range((void *)0, (uintptr_t)dram_size, CACHED_MEM);
	mmu_config_range(_dma_coherent, _dma_coherent_size, UNCACHED_MEM);
	cbmem_initialize_empty();
	run_ramstage();
}
