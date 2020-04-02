/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef SOUTHBRIDGE_INTEL_COMMON_PCIEHP_H
#define SOUTHBRIDGE_INTEL_COMMON_PCIEHP_H

void intel_acpi_pcie_hotplug_generator(u8 *hotplug_map, int port_number);
void intel_acpi_pcie_hotplug_scan_slot(struct bus *bus);

#endif
