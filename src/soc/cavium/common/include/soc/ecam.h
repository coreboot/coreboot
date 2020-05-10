/* SPDX-License-Identifier: GPL-2.0-only */

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
