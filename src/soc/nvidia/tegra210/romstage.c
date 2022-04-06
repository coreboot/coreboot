/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <console/console.h>
#include <lib.h>
#include <program_loading.h>
#include <romstage_common.h>
#include <soc/addressmap.h>
#include <soc/ccplex.h>
#include <soc/clock.h>
#include <soc/nvidia/tegra/apbmisc.h>
#include <soc/romstage.h>
#include <soc/sdram.h>
#include <soc/sdram_configs.h>
#include <symbols.h>
#include <vendorcode/google/chromeos/chromeos.h>

void __weak romstage_mainboard_init(void)
{
	/* Default empty implementation. */
}

void romstage(void)
{
	console_init();
	exception_init();

	romstage_main();
}

void __noreturn romstage_main(void)
{
	printk(BIOS_INFO, "T210: romstage here\n");

#if CONFIG(BOOTROM_SDRAM_INIT)
	printk(BIOS_INFO, "T210 romstage: SDRAM init done by BootROM, RAMCODE = %d\n",
		sdram_get_ram_code());
#else
	sdram_init(get_sdram_config());
	printk(BIOS_INFO, "T210 romstage: sdram_init done\n");
#endif

	/*
	 * IMPORTANT:
	 * DO NOT INITIALIZE ANY CARVEOUT BEFORE TZ.
	 *
	 * Trust Zone needs to be initialized after the DRAM initialization
	 * because carveout registers are programmed during DRAM init.
	 * cbmem_initialize() is dependent on the Trust Zone region
	 * initialization because CBMEM lives right below the Trust Zone which
	 * needs to be properly identified.
	 */
	trustzone_region_init();

	/* Now do various other carveouts */
	gpu_region_init();
	nvdec_region_init();
	tsec_region_init();
	vpr_region_init();

	/*
	 * When romstage is running it's always on the reboot path -- never a
	 * resume path where cbmem recovery is required. Therefore, always
	 * initialize the cbmem area to be empty.
	 */
	cbmem_initialize_empty();

	ccplex_cpu_prepare();
	printk(BIOS_INFO, "T210 romstage: CPU prepare done\n");

	romstage_mainboard_init();

	run_ramstage();
}

void platform_prog_run(struct prog *prog)
{
	/* We'll switch to a new stack, so validate our old one here. */
	checkstack(_estack, 0);

	ccplex_cpu_start(prog_entry(prog));

	clock_halt_avp();
}
