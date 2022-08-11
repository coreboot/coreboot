/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <emi.h>
#include <soc/emi.h>

size_t sdram_size(void)
{
	int rank_num;
	u64 rank_size[RANK_MAX];
	static size_t dram_size = 0;

	if (dram_size)
		return dram_size;

	get_rank_size_by_emi(rank_size);
	rank_num = get_rank_nr_by_emi();

	for (int i = 0; i < rank_num; i++)
		dram_size += rank_size[i];

	printk(BIOS_INFO, "dram size = %#lx\n", dram_size);

	return dram_size;
}
