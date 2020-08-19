/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/exception.h>
#include <console/console.h>
#include <delay.h>
#include <program_loading.h>
#include <symbols.h>
#include <timestamp.h>
#include <soc/bootblock.h>

__attribute__((weak)) void bootblock_mainboard_early_init(void) { /* no-op */ }
__attribute__((weak)) void bootblock_soc_early_init(void) { /* do nothing */ }
__attribute__((weak)) void bootblock_soc_init(void) { /* do nothing */ }
__attribute__((weak)) void bootblock_mainboard_init(void) { /* do nothing */ }

/* C code entry point for the boot block */
void bootblock_main(const uint64_t reg_x0,
		    const uint64_t reg_pc)
{
	init_timer();

	/* Initialize timestamps if we have TIMESTAMP region in memlayout.ld. */
	if (CONFIG(COLLECT_TIMESTAMPS) && REGION_SIZE(timestamp) > 0)
		timestamp_init(timestamp_get());

	bootblock_soc_early_init();
	bootblock_mainboard_early_init();

	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
		exception_init();

		if (reg_x0)
			printk(BIOS_ERR,
			       "BOOTBLOCK: RST Boot Failure Code %lld\n",
			       reg_x0);
	}

	bootblock_soc_init();
	bootblock_mainboard_init();

	run_romstage();
}
