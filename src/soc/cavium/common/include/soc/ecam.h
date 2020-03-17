/*
 * This file is part of the coreboot project.
 *
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

#ifndef __COREBOOT_SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_ECAM_H
#define __COREBOOT_SRC_SOC_CAVIUM_COMMON_INCLUDE_SOC_ECAM_H

#ifdef __SIMPLE_DEVICE__
#include <device/pci_type.h>

uint64_t ecam0_get_bar_val(pci_devfn_t dev, u8 bar);
#else
#include <device/device.h>

uint64_t ecam0_get_bar_val(struct device *dev, u8 bar);
#endif

#endif
