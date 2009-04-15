/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001-2003 Eric W. Biederman <ebiederm@xmission.com>
 * Copyright (C) 2001-2003 Linux Networx
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <mem.h>
#include <pci.h>
#include <cpu/p5/io.h>
#include <part/sizeram.h>
#include <printk.h>


struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	u32 size, size_10, pci_low;
	/* Use the PCI top memory register */
	pcibios_read_config_dword(0, 0, 0x9c, &size);

	/* change the memory size if it is above pci space */
	pci_low = (u32)(pci_memory_base & 0xf8000000);
	if(size > pci_low) {
		size = pci_low;
		pcibios_write_config_dword(0, 0, 0x9c, size);
	}
		
	/* Convert size in bytes to size in K */
	size_10 = size >> 10;

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = size_10 - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return mem;
}

