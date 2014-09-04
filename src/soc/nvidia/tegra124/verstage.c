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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/cache.h>
#include <arch/exception.h>
#include <console/console.h>
#include <soc/nvidia/tegra124/cache.h>
#include <soc/nvidia/tegra124/early_configs.h>
#include <vendorcode/google/chromeos/chromeos.h>

static void enable_cache(void)
{
	mmu_init();
	/* Whole space is uncached. */
	mmu_config_range(0, 4096, DCACHE_OFF);
	/* SRAM is cached. Round the size up to 2MB, the LPAE page size. */
	mmu_config_range(0x40000000 >> 20, 1, DCACHE_WRITEBACK);
	mmu_disable_range(0, 1);
	dcache_mmu_enable();
}

/* Do the minimum to run vboot at full speed */
static void soc_init(void)
{
	configure_l2_cache();
	console_init();
	exception_init();
	enable_cache();
}

void main(void)
{
	asm volatile ("bl arm_init_caches"
		      : : : "r0", "r1", "r2", "r3", "r4", "r5", "ip");

	soc_init();
	early_mainboard_init();
	vboot2_verify_firmware();
}
