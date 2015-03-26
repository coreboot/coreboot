/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef _BROADWELL_RAMSTAGE_H_
#define _BROADWELL_RAMSTAGE_H_

#include <device/device.h>
#include <soc/intel/broadwell/chip.h>

void broadwell_init_pre_device(void *chip_info);
void broadwell_init_cpus(device_t dev);
void broadwell_pch_enable_dev(device_t dev);

#if CONFIG_HAVE_REFCODE_BLOB
void broadwell_run_reference_code(void);
#else
static inline void broadwell_run_reference_code(void) { }
#endif

extern struct pci_operations broadwell_pci_ops;

#endif
