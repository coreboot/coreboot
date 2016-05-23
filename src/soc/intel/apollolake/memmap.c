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

/*
 * The device_t returned by dev_find_slot() is different than the device_t
 * passed to pci_write_config32(). If one needs to get access to the config.h
 * of a device and perform i/o things are incorrect. One is a pointer while
 * the other is a 32-bit integer.
 */
#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <cbmem.h>
#include <device/pci.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/smm.h>

static uintptr_t smm_region_start(void)
{
	return ALIGN_DOWN(pci_read_config32(NB_DEV_ROOT, TSEG), 1*MiB);
}

static size_t smm_region_size(void)
{
	uintptr_t smm_end =
		ALIGN_DOWN(pci_read_config32(NB_DEV_ROOT, BGSM), 1*MiB);
	return smm_end - smm_region_start();
}

void *cbmem_top(void)
{
	return (void *)smm_region_start();
}

void smm_region(void **start, size_t *size)
{
	*start = (void *)smm_region_start();
	*size = smm_region_size();
}
