/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017-2018 Intel Corp.
 * (Written by Bora Guvendik <bora.guvendik@intel.com> for Intel Corp.)
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


/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* PCR access */
#include <soc/intel/common/acpi/pcr.asl>

/* eMMC, SD Card */
#include "scs.asl"

/* GPIO controller */
#include "gpio.asl"

/* LPC 0:1f.0 */
#include "lpc.asl"

/* PCH HDA */
#include "pch_hda.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include "smbus.asl"

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* CNVi */
#include "cnvi.asl"

/* GBe 0:1f.6 */
#include "pch_glan.asl"
