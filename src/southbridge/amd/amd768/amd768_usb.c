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

void amd768_usb_setup(void)
{
	/* FIXME this is untested incomplete implementation. */
	struct pci_dev *dev;
	dev = pci_find_device(PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_SB768_7449, 0);
	if (dev) {
		u32 cmd;
		pci_read_config_dword(dev, PCI_COMMAND, &cmd);
		pci_write_config_dword(dev, PCI_COMMAND, 
			cmd | PCI_COMMAND_IO | PCI_COMMAND_MEMORY | 
			PCI_COMMAND_MASTER | PCI_COMMAND_INVALIDATE);
	}
}
