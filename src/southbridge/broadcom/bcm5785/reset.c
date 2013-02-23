/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2005 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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
#include <reset.h>

#define PCI_DEV(BUS, DEV, FN) ( \
        (((BUS) & 0xFFF) << 20) | \
        (((DEV) & 0x1F) << 15) | \
        (((FN)  & 0x7) << 12))

typedef unsigned device_t;

static void pci_write_config32(device_t dev, unsigned where, unsigned value)
{
        unsigned addr;
        addr = (dev>>4) | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        outl(value, 0xCFC);
}

static unsigned pci_read_config32(device_t dev, unsigned where)
{
        unsigned addr;
        addr = (dev>>4) | where;
        outl(0x80000000 | (addr & ~3), 0xCF8);
        return inl(0xCFC);
}

#include "../../../northbridge/amd/amdk8/reset_test.c"

void hard_reset(void)
{
	set_bios_reset();
        /* Try rebooting through port 0xcf9 */
	/* Actually it is not a real hard_reset --- it only reset coherent link table, but not reset link freq and width */
	outb((0 <<3)|(0<<2)|(1<<1), 0xcf9);
      	outb((0 <<3)|(1<<2)|(1<<1), 0xcf9);
}
