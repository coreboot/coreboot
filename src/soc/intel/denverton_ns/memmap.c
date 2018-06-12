/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 - 2017 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <cbfs.h>
#include <cbmem.h>
#include <assert.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <soc/smm.h>
#include <lib.h>

/* Returns base of requested region encoded in the system agent. */
static inline uintptr_t system_agent_region_base(size_t reg)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = SA_DEV_ROOT;
#else
	struct device *dev = SA_DEV_ROOT;
#endif
	/* All regions concerned for have 1 MiB alignment. */
	return ALIGN_DOWN(pci_read_config32(dev, reg), 1 * MiB);
}

/* Returns min power of 2 >= size */
static inline u32 power_of_2(u32 size)
{
	return size ? 1 << (1 + log2(size - 1)) : 0;
}

u32 top_of_32bit_ram(void)
{
	u32 iqat_region_size = 0;
	u32 tseg_region_size = system_agent_region_base(TOLUD) -
			       system_agent_region_base(TSEGMB);

/*
 * Add IQAT region size if enabled.
 */
#if IS_ENABLED(CONFIG_IQAT_ENABLE)
	iqat_region_size = CONFIG_IQAT_MEMORY_REGION_SIZE;
#endif
	return system_agent_region_base(TOLUD) -
	       power_of_2(iqat_region_size + tseg_region_size);
}

void *cbmem_top(void) { return (void *)top_of_32bit_ram(); }

static inline uintptr_t smm_region_start(void)
{
	return system_agent_region_base(TSEGMB);
}

static inline size_t smm_region_size(void)
{
	return system_agent_region_base(TOLUD) - smm_region_start();
}

void smm_region(void **start, size_t *size)
{
	*start = (void *)smm_region_start();
	*size = smm_region_size();
}

int smm_subregion(int sub, void **start, size_t *size)
{
	uintptr_t sub_base;
	size_t sub_size;
	const size_t cache_size = CONFIG_SMM_RESERVED_SIZE;

	sub_base = smm_region_start();
	sub_size = smm_region_size();

	assert(sub_size > CONFIG_SMM_RESERVED_SIZE);

	switch (sub) {
	case SMM_SUBREGION_HANDLER:
		/* Handler starts at the base of TSEG. */
		sub_size -= cache_size;
		break;
	case SMM_SUBREGION_CACHE:
		/* External cache is in the middle of TSEG. */
		sub_base += sub_size - cache_size;
		sub_size = cache_size;
		break;
	default:
		return -1;
	}

	*start = (void *)sub_base;
	*size = sub_size;

	return 0;
}
