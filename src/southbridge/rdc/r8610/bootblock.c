/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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

#include <arch/io.h>
#include <device/pci_def.h>

static void bootblock_southbridge_init(void) {
        uint32_t tmp;
	tmp = pci_read_config32(PCI_DEV(0,7,0), 0x40);
	/* decode all flash ranges */
	pci_write_config32(PCI_DEV(0,7,0), 0x40, tmp | 0x07ff0000);
}
