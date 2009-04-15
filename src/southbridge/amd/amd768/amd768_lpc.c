/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric W. Biederman <ebiederm@xmission.com>
 * Copyright (C) 2003 Linux Networx
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
#include <southbridge/amd/amd768.h>

void amd768_enable_serial_irqs(int continuous, unsigned frames, unsigned startclocks)
{
	struct pci_dev *dev;
	unsigned value;
	value = ((!!continuous) << 6)|((frames & 0xf) << 2)|((startclocks & 3) << 0);
	
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7443,0);
	if (dev != NULL) {
		/* Setup serial irq's for the LPC bus. */
		pci_write_config_byte(dev, 0x4a, value);
	}
	
}
