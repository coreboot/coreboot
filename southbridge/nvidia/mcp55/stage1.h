/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 Ronald G. Minnich
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#define pci_read_config32(bdf, where) pci_cf8_conf1.read32(bdf, where)
#define pci_write_config32(bdf, where, what) pci_cf8_conf1.write32(bdf, where, what)
#define pci_read_config16(bdf, where) pci_cf8_conf1.read32(bdf, where)
#define pci_write_config16(bdf, where, what) pci_cf8_conf1.write32(bdf, where, what)
#define pci_locate_device(vid, did, slot) pci_cf8_conf1.find(vid, did, slot)
