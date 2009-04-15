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

#include <pci.h>
#include <pci_ids.h>
#include <northbridge/amd/amd76x.h>

/* FIXME add support for 66Mhz pci */
void amd76x_setup_pci_arbiter(void)
{
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FE_GATE_700C, 0);
	if (dev) {
		/* Enable:
		 * PCI parking
		 * memory prefetching
		 * EV6 mode
		 * Enable power management registers
		 * The southbridge lock
		 * Read data error disable
		 * PCI retries
		 * AGP retries
		 * AGP chaining
		 * PCI chaining
		 */
		pci_write_config_dword(dev, 0x84, 
			(0<<24)
			|(1<<23)
			|(1<<17)|(1<<16)
			|(0<<15)|(1<<14)|(1<<13)|(1<<12)
			|(0<<11)|(0<<10)|(0<<9)|(0<<8)
			|(1<<7)|(0<<6)|(0<<5)|(1<<4)
			|(0<<3)|(1<<2)|(1<<1)|(1<<0));
	}
}


