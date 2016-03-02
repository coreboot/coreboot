/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2015, The Linux Foundation. All rights reserved.
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
 */

#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/soc_services.h>
#include "mmu.h"

void main(void)
{
	console_init();
	initialize_dram();

	/* Add dram mappings to mmu tables. */
	setup_dram_mappings(DRAM_INITIALIZED);

	cbmem_initialize_empty();
	run_ramstage();
}
