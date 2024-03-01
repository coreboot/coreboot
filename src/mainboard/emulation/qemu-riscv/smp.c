/* SPDX-License-Identifier: GPL-2.0-only */

#include <mcall.h>
#include <arch/smp/smp.h>
#include <commonlib/device_tree.h>
#include <console/console.h>

unsigned int smp_get_hart_count(void)
{
	if (!fdt_is_valid(HLS()->fdt))
		goto error;

	uint32_t cpus_offset = fdt_find_node_by_path(HLS()->fdt, "/cpus", NULL, NULL);
	if (!cpus_offset)
		goto error;

	static u32 harts[CONFIG_MAX_CPUS]; // too big for the stack
	size_t count_harts = fdt_find_subnodes_by_prefix(HLS()->fdt, cpus_offset, "cpu@",
							 NULL, NULL, harts, CONFIG_MAX_CPUS);
	if (!count_harts)
		goto error;

	printk(BIOS_DEBUG, "found %zu harts in devicetree\n", count_harts);
	return count_harts;
error:
	printk(BIOS_ERR, "%s: Failed to read devicetree to get number of harts\n", __func__);
	return 1; // Return single hart on failure to keep booting
}
