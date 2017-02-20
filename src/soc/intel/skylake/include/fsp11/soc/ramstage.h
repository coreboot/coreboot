/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015-2017 Intel Corporation.
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

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <chip.h>
#include <device/device.h>
#include <fsp/gop.h>
#include <fsp/ramstage.h>
#include <fsp/soc_binding.h>

#define FSP_SIL_UPD SILICON_INIT_UPD
#define FSP_MEM_UPD MEMORY_INIT_UPD

void soc_irq_settings(FSP_SIL_UPD *params);
void pch_enable_dev(device_t dev);
void soc_init_pre_device(void *chip_info);
void soc_fsp_load(void);
const char *soc_acpi_name(struct device *dev);
int init_igd_opregion(igd_opregion_t *igd_opregion);
extern struct pci_operations soc_pci_ops;

/* Get igd framebuffer bar */
uintptr_t fsp_soc_get_igd_bar(void);

#endif
