/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
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

#include <console/console.h>

#include <arch/io.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "mcp55.h"

static uint32_t final_reg;

static device_t find_lpc_dev( device_t dev,  unsigned devfn)
{

	device_t lpc_dev;

	lpc_dev = dev_find_slot(dev->bus->secondary, devfn);

	if ( !lpc_dev ) return lpc_dev;

	if ((lpc_dev->vendor != PCI_VENDOR_ID_NVIDIA) || (
		(lpc_dev->device < PCI_DEVICE_ID_NVIDIA_MCP55_LPC) ||
		(lpc_dev->device > PCI_DEVICE_ID_NVIDIA_MCP55_PRO)
		) ) {
			uint32_t id;
			id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
			if ( (id < (PCI_VENDOR_ID_NVIDIA | (PCI_DEVICE_ID_NVIDIA_MCP55_LPC << 16))) ||
				(id > (PCI_VENDOR_ID_NVIDIA | (PCI_DEVICE_ID_NVIDIA_MCP55_PRO << 16)))
				) {
				lpc_dev = 0;
			}
	}

	return lpc_dev;
}

void mcp55_enable(device_t dev)
{
	device_t lpc_dev = 0;
	device_t sm_dev = 0;
	unsigned index = 0;
	unsigned index2 = 0;
	uint32_t reg_old, reg;
	uint8_t byte;
	unsigned deviceid;
	unsigned vendorid;

	struct southbridge_nvidia_mcp55_config *conf;
	conf = dev->chip_info;
	int i;

	unsigned devfn;

	if(dev->device==0x0000) {
		vendorid = pci_read_config32(dev, PCI_VENDOR_ID);
		deviceid = (vendorid>>16) & 0xffff;
//		vendorid &= 0xffff;
	} else {
//		vendorid = dev->vendor;
		deviceid = dev->device;
	}

	devfn = (dev->path.pci.devfn) & ~7;
	switch(deviceid) {
		case PCI_DEVICE_ID_NVIDIA_MCP55_HT:
			return;

		case PCI_DEVICE_ID_NVIDIA_MCP55_SM2://?
			index = 16;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_USB:
			devfn -= (1<<3);
			index = 8;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_USB2:
			devfn -= (1<<3);
			index = 20;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_NIC: //two
		case PCI_DEVICE_ID_NVIDIA_MCP55_NIC_BRIDGE://two
			devfn -= (7<<3);
			index = 10;
			for(i=0;i<2;i++) {
				lpc_dev = find_lpc_dev(dev, devfn - (i<<3));
				if(!lpc_dev) continue;
				index -= i;
				devfn -= (i<<3);
				break;
			}
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_AZA:
			devfn -= (5<<3);
			index = 11;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_IDE:
			devfn -= (3<<3);
			index = 14;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_SATA0: //three
		case PCI_DEVICE_ID_NVIDIA_MCP55_SATA1: //three
			devfn -= (4<<3);
			index = 22;
			i = (dev->path.pci.devfn) & 7;
			if(i>0) {
				index -= (i+3);
			}
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_PCI:
			devfn -= (5<<3);
			index = 15;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_A:
			devfn -= (0x9<<3);  // to LPC
			index2 = 9;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_B_C: //two
			devfn -= (0xa<<3);  // to LPC
			index2 = 8;
			for(i=0;i<2;i++) {
				lpc_dev = find_lpc_dev(dev, devfn - (i<<3));
				if(!lpc_dev) continue;
				index2 -= i;
				devfn -= (i<<3);
				break;
			}
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_D:
			devfn -= (0xc<<3);  // to LPC
			index2 = 6;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_E:
			devfn -= (0xd<<3);  // to LPC
			index2 = 5;
			break;
		case PCI_DEVICE_ID_NVIDIA_MCP55_PCIE_F:
			devfn -= (0xe<<3);  // to LPC
			index2 = 4;
			break;
		default:
			index = 0;
	}

	if(!lpc_dev)
		lpc_dev = find_lpc_dev(dev, devfn);

	if ( !lpc_dev )	return;

	if(index2!=0) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if(!sm_dev) return;

		if ( sm_dev ) {
			reg_old = reg =  pci_read_config32(sm_dev, 0xe4);

			if (!dev->enabled) { //disable it
				reg |= (1<<index2);
			}

			if (reg != reg_old) {
				pci_write_config32(sm_dev, 0xe4, reg);
			}
		}

		index2 = 0;
		return;
	}


	if ( index == 0) {  // for LPC

		// expose ioapic base
		byte = pci_read_config8(lpc_dev, 0x74);
		byte |= ((1<<1)); // expose the BAR
		pci_write_config8(dev, 0x74, byte);

		// expose trap base
		byte = pci_read_config8(lpc_dev, 0xdd);
		byte |= ((1<<0)|(1<<3)); // expose the BAR and enable write
		pci_write_config8(dev, 0xdd, byte);

		return;

	}

	if( index == 16) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if(!sm_dev) return;

		final_reg = pci_read_config32(sm_dev, 0xe8);
		final_reg &= ~((1<<16)|(1<<8)|(1<<20)|(1<<14)|(1<<22)|(1<<18)|(1<<17)|(1<<15)|(1<<11)|(1<<10)|(1<<9));
		pci_write_config32(sm_dev, 0xe8, final_reg); //enable all at first
#if 0
		reg_old = reg = pci_read_config32(sm_dev, 0xe4);
//		reg |= (1<<0);
		reg &= ~(0x3f<<4);
		if (reg != reg_old) {
			printk(BIOS_DEBUG, "mcp55.c pcie enabled\n");
			pci_write_config32(sm_dev, 0xe4, reg);
		}
#endif
	}

	if (!dev->enabled) {
		final_reg |= (1 << index);// disable it
		//The reason for using final_reg, if diable func 1, the func 2 will be func 1 so We need disable them one time.
	}

	if(index == 9 ) { //NIC1 is the final, We need update final reg to 0xe8
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if(!sm_dev) return;
		reg_old = pci_read_config32(sm_dev, 0xe8);
		if (final_reg != reg_old) {
			pci_write_config32(sm_dev, 0xe8, final_reg);
		}

	}


}

static void mcp55_set_subsystem(device_t dev, unsigned vendor, unsigned device)
{
	pci_write_config32(dev, 0x40,
			   ((device & 0xffff) << 16) | (vendor & 0xffff));
}

struct pci_operations mcp55_pci_ops = {
	.set_subsystem = mcp55_set_subsystem,
}; 

struct chip_operations southbridge_nvidia_mcp55_ops = {
	CHIP_NAME("NVIDIA MCP55 Southbridge")
	.enable_dev	= mcp55_enable,
};
