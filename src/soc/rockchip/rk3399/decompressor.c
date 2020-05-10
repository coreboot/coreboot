/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/mmu.h>
#include <bootblock_common.h>
#include <soc/mmu_operations.h>
#include <symbols.h>

void decompressor_soc_init(void)
{
	mmu_init();

	/* Set 0x0 to max sdram(4GiB) supported by RK3399 as device memory.
	 * We want to configure mmio space(start at 0xf8000000) to DEV_MEM,
	 * some boards may use 2GB sdram in future(who knows).
	 */
	mmu_config_range((void *)0, (uintptr_t)4 * GiB, DEV_MEM);

	mmu_config_range(_sram, REGION_SIZE(sram), SECURE_MEM);

	mmu_enable();
}
