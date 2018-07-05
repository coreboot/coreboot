/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#ifndef NORTHBRIDGE_AMD_AMDFAM10_H
#define NORTHBRIDGE_AMD_AMDFAM10_H

u32 amdfam10_scan_root_bus(struct device *root, u32 max);
void get_pci1234(void);

#endif /* NORTHBRIDGE_AMD_AMDFAM10_H */
