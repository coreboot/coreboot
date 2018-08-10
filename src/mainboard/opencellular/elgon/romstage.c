/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017-present Facebook, Inc.
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

#include <arch/exception.h>
#include <cbmem.h>
#include <romstage_handoff.h>
#include <soc/sdram.h>
#include <soc/timer.h>
#include <soc/mmu.h>
#include <stdlib.h>
#include <console/console.h>
#include <program_loading.h>
#include <libbdk-hal/bdk-config.h>
#include <string.h>
#include <arch/stages.h>

extern const struct bdk_devicetree_key_value devtree[];

void platform_romstage_main(void)
{
	watchdog_poke(0);

	console_init();
	exception_init();

	bdk_config_set_fdt(devtree);

	sdram_init();
	soc_mmu_init();

	watchdog_poke(0);

	cbmem_initialize_empty();
	run_ramstage();
}
