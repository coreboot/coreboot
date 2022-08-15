/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbmem.h>
#include <commonlib/bsd/mem_chip_info.h>
#include <soc/emi.h>

size_t sdram_size(void)
{
	const struct mem_chip_info *mc;
	size_t size = 0;

	if (ENV_RAMINIT) {
		size = mtk_dram_size();
		printk(BIOS_INFO, "dram size (romstage): %#lx\n", size);
		return size;
	}

	mc = cbmem_find(CBMEM_ID_MEM_CHIP_INFO);
	assert(mc);

	for (unsigned int i = 0; i < mc->num_channels; ++i)
		size += mc->channel[i].density;

	printk(BIOS_INFO, "dram size: %#lx\n", size);
	return size;
}

void mt_set_emi(struct dramc_param *dparam)
{
	/* Do nothing */
}
