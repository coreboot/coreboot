/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef NORTHBRIDGE_AMD_AGESA_FAM12H_H
#define NORTHBRIDGE_AMD_AGESA_FAM12H_H

static struct device_operations pci_domain_ops;
static struct device_operations cpu_bus_ops;

device_t get_node_pci(u32 nodeid, u32 fn);

#endif /* NORTHBRIDGE_AMD_AGESA_FAM12H_H */
