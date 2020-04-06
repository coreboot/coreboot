/*
 * This file is part of the coreboot project.
 *
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

#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/itss.h>
#include <soc/pcr_ids.h>

/* PCI IRQ assignment */
#include "pci_irqs.asl"

/* PCR access */
#include <soc/intel/common/acpi/pcr.asl>

/* PCH clock */
#include "camera_clock_ctl.asl"

/* GPIO controller */
#include "gpio.asl"

/* ESPI 0:1f.0 */
#include <soc/intel/common/block/acpi/acpi/lpc.asl>

/* PCH HDA */
#include "pch_hda.asl"

/* PCIE Ports */
#include "pcie.asl"

/* pmc 0:1f.2 */
#include "pmc.asl"

/* Serial IO */
#include "serialio.asl"

/* SMBus 0:1f.4 */
#include "smbus.asl"

/* ISH 0:12.0 */
#include "ish.asl"

/* USB XHCI 0:14.0 */
#include "xhci.asl"

/* PCI _OSC */
#include <soc/intel/common/acpi/pci_osc.asl>

/* PMC Core*/
#include <soc/intel/common/block/acpi/acpi/pmc.asl>

/* EMMC/SD card */
#include "scs.asl"
