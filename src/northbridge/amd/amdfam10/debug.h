/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Damien Zammit <damien@zamaudio.com>
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

#ifndef AMDFAM10_DEBUG_H
#define AMDFAM10_DEBUG_H

#include <inttypes.h>
#include "pci.h"

void print_debug_addr(const char *str, void *val);
void print_debug_pci_dev(u32 dev);
void print_pci_devices(void);
void print_pci_devices_on_bus(u32 busn);
void dump_pci_device_range(u32 dev, u32 start_reg, u32 size);
void dump_pci_device(u32 dev);
void dump_pci_device_index_wait_range(u32 dev, u32 index_reg, u32 start,
					u32 size);
void dump_pci_device_index_wait(u32 dev, u32 index_reg);
void dump_pci_device_index(u32 dev, u32 index_reg, u32 type, u32 length);
void dump_pci_devices(void);
void dump_pci_devices_on_bus(u32 busn);

#if CONFIG_DEBUG_SMBUS
void dump_spd_registers(const struct mem_controller *ctrl);
void dump_smbus_registers(void);
#endif

void dump_io_resources(u32 port);
void dump_mem(u32 start, u32 end);

void print_tx(const char *strval, u32 val);
void print_t(const char *strval);
void print_tf(const char *func, const char *strval);
#endif
