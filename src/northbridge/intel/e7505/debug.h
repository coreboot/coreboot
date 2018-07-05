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

#ifndef E7505_DEBUG_H
#define E7505_DEBUG_H

void print_debug_pci_dev(unsigned dev);
void print_pci_devices(void);
void dump_pci_device(unsigned dev);
void dump_pci_devices(void);
void dump_pci_devices_on_bus(unsigned busn);
void dump_spd_registers(const struct mem_controller *ctrl);
void dump_smbus_registers(void);
void dump_io_resources(unsigned port);
void dump_mem(unsigned start, unsigned end);

#endif
