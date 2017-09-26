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
#include <fsp/api.h>
#include <fsp/util.h>

#define FSP_SIL_UPD FSP_S_CONFIG
#define FSP_MEM_UPD FSP_M_CONFIG

void mainboard_silicon_init_params(FSP_S_CONFIG *params);
void soc_fsp_load(void);
void soc_init_pre_device(void *chip_info);
void soc_irq_settings(FSP_SIL_UPD *params);
const char *soc_acpi_name(const struct device *dev);

#endif
