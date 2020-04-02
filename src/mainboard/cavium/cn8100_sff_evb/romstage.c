/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/stages.h>
#include <soc/sdram.h>
#include <soc/timer.h>
#include <soc/mmu.h>
#include <libbdk-hal/bdk-config.h>

extern const struct bdk_devicetree_key_value devtree[];

void platform_romstage_main(void)
{
	watchdog_poke(0);

	bdk_config_set_fdt(devtree);

	sdram_init();
	soc_mmu_init();

	watchdog_poke(0);
}
