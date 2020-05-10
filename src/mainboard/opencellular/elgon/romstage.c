/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <cbmem.h>
#include <soc/sdram.h>
#include <soc/timer.h>
#include <soc/mmu.h>
#include <console/console.h>
#include <program_loading.h>
#include <libbdk-hal/bdk-config.h>
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
