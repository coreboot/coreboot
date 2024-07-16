/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cpu.h>
#include <console/console.h>
#include <intelblocks/car_lib.h>

/*
 * Gathers and prints information about the CPU's L3 cache.
 *
 *  This function does the following:
 *      1. Sets the cache level of interest to L3.
 *      2. Prints the following cache details to the console:
 *          - Cache level
 *          - Associativity (number of ways)
 *          - Number of physical partitions
 *          - Line size (in bytes)
 *          - Number of sets
 *          - Total cache size (in MiB), calculated using the 'get_cache_size' function.
 */
void car_report_cache_info(void)
{
	int cache_level = CACHE_L3;
	struct cpu_cache_info info;

	if (!fill_cpu_cache_info(cache_level, &info))
		return;

	printk(BIOS_INFO, "Cache: Level %d: ", cache_level);
	printk(BIOS_INFO, "Associativity = %zd Partitions = %zd Line Size = %zd Sets = %zd\n",
		info.num_ways, info.physical_partitions, info.line_size, info.num_sets);

	printk(BIOS_INFO, "Cache size = %zu MiB\n", get_cache_size(&info)/MiB);
}
