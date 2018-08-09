/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
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

#include <intelblocks/pcr.h>
#include <soc/gpe.h>

/* PCIE device */
#include "pcie.asl"

/* LPSS device */
#include "lpss.asl"

/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* GPIO controller */
#include "gpio.asl"

#include "xhci.asl"

/* LPC */
#include "lpc.asl"

/* eMMC */
#include "scs.asl"

/* PMC IPC controller */
#include "pmc_ipc.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* SGX */
#if IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_SGX)
#include <soc/intel/common/acpi/sgx.asl>
#endif
