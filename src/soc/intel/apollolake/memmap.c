/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Andrey Petrov <andrey.petrov@intel.com> for Intel Corp.)
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
#include <assert.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <soc/systemagent.h>
#include <soc/pci_devs.h>

#include "chip.h"

void *cbmem_top(void)
{
	const config_t *config;
	void *tolum = (void *)sa_get_tseg_base();

	if (!CONFIG(SOC_INTEL_GLK))
		return tolum;

	config = config_of_path(PCH_DEVFN_LPC);

	/* FSP allocates 2x PRMRR Size Memory for alignment */
	if (config->sgx_enable)
		tolum -= config->PrmrrSize * 2;

	return tolum;
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = sa_get_tseg_base();
	*size = sa_get_tseg_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;
	uintptr_t smm_base;
	size_t smm_size;

	/*
	 * We need to make sure ramstage will be run cached. At this point exact
	 * location of ramstage in cbmem is not known. Instruct postcar to cache
	 * 16 megs under cbmem top which is a safe bet to cover ramstage.
	 */
	top_of_ram = (uintptr_t) cbmem_top();
	/* cbmem_top() needs to be at least 16 MiB aligned */
	assert(ALIGN_DOWN(top_of_ram, 16*MiB) == top_of_ram);
	postcar_frame_add_mtrr(pcf, top_of_ram - 16*MiB, 16*MiB,
		MTRR_TYPE_WRBACK);

	/*
	* Cache the TSEG region at the top of ram. This region is
	* not restricted to SMM mode until SMM has been relocated.
	* By setting the region to cacheable it provides faster access
	* when relocating the SMM handler as well as using the TSEG
	* region for other purposes.
	*/
	smm_region(&smm_base, &smm_size);
	postcar_frame_add_mtrr(pcf, smm_base, smm_size, MTRR_TYPE_WRBACK);
}
