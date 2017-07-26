/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Advanced Micro Devices, Inc.
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

#ifndef PI_STONEYRIDGE_NORTHBRIDGE_H
#define PI_STONEYRIDGE_NORTHBRIDGE_H

#include <arch/cpu.h>
#include <arch/io.h>
#include <device/device.h>

void cpu_bus_scan(device_t dev);
void domain_enable_resources(device_t dev);
void domain_read_resources(device_t dev);
void domain_set_resources(device_t dev);
void fam15_finalize(void *chip_info);
void setup_uma_memory(void);

/* todo: remove this when postcar stage is in place */
asmlinkage void chipset_teardown_car(void);

#endif /* PI_STONEYRIDGE_NORTHBRIDGE_H */
