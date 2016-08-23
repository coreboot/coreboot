/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2016 Intel Corporation.
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

#include <chip.h>
#include <bootstate.h>
#include <device/pci.h>
#include <fsp/api.h>

/* UPD parameters to be initialized before SiliconInit */
void platform_fsp_silicon_init_params_cb(struct FSPS_UPD *supd)
{
}

struct pci_operations soc_pci_ops = {
	/* TODO: Add set subsystem id function */
};

