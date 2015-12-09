/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google, Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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

#include <arch/io.h>
#include <cbmem.h>
#include <soc/iosf.h>
#include <soc/smm.h>
#include <drivers/intel/fsp1_0/fsp_util.h>

uintptr_t smm_region_start(void)
{
	return (iosf_bunit_read(BUNIT_SMRRL) << 20);
}

/** @brief get the top of usable low memory from the FSP's HOB list
 *
 * The FSP's reserved memory sits just below the SMM region.  The memory
 * region below it is usable memory.
 *
 * The entire memory map is shown in northcluster.c
 *
 * @return pointer to the first byte of reserved memory
 */

void *cbmem_top(void)
{
	return find_fsp_reserved_mem(*(void **)CBMEM_FSP_HOB_PTR);
}
