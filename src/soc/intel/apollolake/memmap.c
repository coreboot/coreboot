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

#include <cbmem.h>
#include <intelblocks/cpulib.h>
#include <soc/systemagent.h>

#include "chip.h"

void *cbmem_top_chipset(void)
{
	void *tolum = (void *)sa_get_tseg_base();

	if (!CONFIG(SOC_INTEL_GLK))
		return tolum;

	/* FSP allocates 2x PRMRR Size Memory for alignment */
	tolum -= get_prmrr_size() * 2;

	return tolum;
}
