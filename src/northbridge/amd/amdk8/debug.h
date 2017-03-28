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

#ifndef AMDK8_DEBUG_H
#define AMDK8_DEBUG_H

#include <inttypes.h>
#include <arch/io.h>

void print_debug_addr(const char *str, void *val);
void print_debug_pci_dev(unsigned int dev);
void print_pci_devices(void);
void dump_pci_device(unsigned int dev);
void dump_pci_device_index_wait(unsigned int dev, uint32_t index_reg);
uint32_t pci_read_config32_index_wait(pci_devfn_t dev,
				uint32_t index_reg, uint32_t index);
void dump_pci_devices(void);
void dump_pci_devices_on_bus(unsigned int busn);
void dump_io_resources(unsigned int port);
void dump_mem(unsigned start, unsigned end);

#endif
