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
 */

#include <arch/cpu.h>
#include <arch/lib_helpers.h>
#include <arch/stages.h>
#include <gic.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/mmu_operations.h>
#include <soc/mtc.h>

static void arm64_arch_timer_init(void)
{
	uint32_t freq = clock_get_osc_khz() * 1000;
	// Set the cntfrq register.
	raw_write_cntfrq_el0(freq);
}

static void mselect_enable_wrap(void)
{
	uint32_t reg;

#define ERR_RESP_EN_SLAVE1		(0x1 << 24)
#define ERR_RESP_EN_SLAVE2		(0x1 << 25)
#define WRAP_TO_INCR_SLAVE0		(0x1 << 27)
#define WRAP_TO_INCR_SLAVE1		(0x1 << 28)
#define WRAP_TO_INCR_SLAVE2		(0x1 << 29)

	reg = read32((void *)TEGRA_MSELECT_CONFIG);
	/* Disable error mechanism */
	reg &= ~(ERR_RESP_EN_SLAVE1 | ERR_RESP_EN_SLAVE2);
	/* Enable WRAP type conversion */
	reg |= (WRAP_TO_INCR_SLAVE0 | WRAP_TO_INCR_SLAVE1 |
		WRAP_TO_INCR_SLAVE2);
	write32((void *)TEGRA_MSELECT_CONFIG, reg);
}

/* Tegra-specific entry point, called from assembly in stage_entry.S */
void ramstage_entry(void);
void ramstage_entry(void)
{
	/* TODO: Is this still needed? */
	gic_init();

	/* TODO: Move arch timer setup to BL31? */
	arm64_arch_timer_init();

	/* Enable WRAP to INCR burst type conversion in MSELECT */
	mselect_enable_wrap();

	/* TODO: Move TrustZone setup to BL31? */
	trustzone_region_init();

	tegra210_mmu_init();

	clock_init_arm_generic_timer();

	if (tegra210_run_mtc() != 0)
		printk(BIOS_ERR, "MTC: No training data.\n");

	/* Jump to boot state machine in common code. */
	main();
}
