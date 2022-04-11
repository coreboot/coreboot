/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <soc/bootblock.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_basetime(base_timestamp);
}

void bootblock_soc_early_init(void)
{
	bootblock_ioe_die_early_init();
	bootblock_soc_die_early_init();
}

void bootblock_soc_init(void)
{
	report_platform_info();
	bootblock_soc_die_init();
}
