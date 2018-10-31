/*
 * This file is part of the coreboot project.
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

#ifndef SOUTHBRIDGE_INTEL_COMMON_PCIEHP_H
#define SOUTHBRIDGE_INTEL_COMMON_PCIEHP_H

void intel_acpi_pcie_hotplug_generator(u8 *hotplug_map, int port_number);
void intel_acpi_pcie_hotplug_scan_slot(struct bus *bus);

#endif
