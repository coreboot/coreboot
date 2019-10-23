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

#include <arch/romstage.h>
#include <cbmem.h>
#include <assert.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/systemagent.h>
#include <lib.h>

/* Returns base of requested region encoded in the system agent. */
static inline uintptr_t system_agent_region_base(size_t reg)
{
#if defined(__SIMPLE_DEVICE__)
	pci_devfn_t dev = SA_DEV_ROOT;
#else
	struct device *dev = pcidev_path_on_root(SA_DEVFN_ROOT);
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
#if CONFIG(IQAT_ENABLE)
	iqat_region_size = CONFIG_IQAT_MEMORY_REGION_SIZE;
#endif
	return system_agent_region_base(TOLUD) -
	       power_of_2(iqat_region_size + tseg_region_size);
}

void *cbmem_top_chipset(void) { return (void *)top_of_32bit_ram(); }

static inline uintptr_t smm_region_start(void)
{
	return system_agent_region_base(TSEGMB);
}

static inline size_t smm_region_size(void)
{
	return system_agent_region_base(TOLUD) - smm_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = smm_region_start();
	*size = smm_region_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t)cbmem_top();
	postcar_frame_add_mtrr(pcf, top_of_ram - 16 * MiB, 16 * MiB,
			       MTRR_TYPE_WRBACK);

	/* Cache the TSEG region */
	postcar_enable_tseg_cache(pcf);
}
