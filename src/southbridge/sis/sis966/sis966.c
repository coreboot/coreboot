/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
 * Copyright (C) 2006,2007 AMD
 * Written by Yinghai Lu <yinghai.lu@amd.com> for AMD.
 * Copyright (C) 2007 Silicon Integrated Systems Corp. (SiS)
 * Written by Morgan Tsai <my_tsai@sis.com> for SiS.
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
 */

#include <console/console.h>

#include <arch/io.h>

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include "sis966.h"

static uint32_t final_reg;

static struct device *find_lpc_dev( struct device *dev,  unsigned devfn)
{

	struct device *lpc_dev;

	lpc_dev = dev_find_slot(dev->bus->secondary, devfn);

	if ( !lpc_dev ) return lpc_dev;

if ((lpc_dev->vendor != PCI_VENDOR_ID_SIS) || (
		(lpc_dev->device != PCI_DEVICE_ID_SIS_SIS966_LPC)
		) ) {
			uint32_t id;
			id = pci_read_config32(lpc_dev, PCI_VENDOR_ID);
			if ( (id < (PCI_VENDOR_ID_SIS | (PCI_DEVICE_ID_SIS_SIS966_LPC << 16)))
				) {
				lpc_dev = 0;
			}
	}

	return lpc_dev;
}

void sis966_enable(struct device *dev)
{
	struct device *lpc_dev = NULL;
	struct device *sm_dev = NULL;
	uint16_t index = 0;
	uint16_t index2 = 0;
	uint32_t reg_old, reg;
	uint8_t byte;
	uint16_t deviceid;
	uint16_t vendorid;
	uint16_t devfn;

	struct southbridge_sis_sis966_config *conf;
	conf = dev->chip_info;
	int i;

	if (dev->device == 0x0000) {
		reg = pci_read_config32(dev, PCI_VENDOR_ID);
		deviceid = (reg >> 16) & 0xffff;
		vendorid = reg & 0xffff;
	} else {
//		vendorid = dev->vendor;
		deviceid = dev->device;
	}

	devfn = (dev->path.pci.devfn) & ~7;
	switch (deviceid) {
		case PCI_DEVICE_ID_SIS_SIS966_USB:
			devfn -= (1<<3);
			index = 8;
			break;
		case PCI_DEVICE_ID_SIS_SIS966_USB2:
			devfn -= (1<<3);
			index = 20;
			break;
		case PCI_DEVICE_ID_SIS_SIS966_NIC:
			devfn -= (7<<3);
			index = 10;
			for (i=0;i<2;i++) {
				lpc_dev = find_lpc_dev(dev, devfn - (i<<3));
				if (!lpc_dev) continue;
				index -= i;
				devfn -= (i<<3);
				break;
			}
			break;
		case PCI_DEVICE_ID_SIS_SIS966_HD_AUDIO:
			devfn -= (5<<3);
			index = 11;
			break;
		case PCI_DEVICE_ID_SIS_SIS966_IDE:
			devfn -= (3<<3);
			index = 14;
			break;
		case PCI_DEVICE_ID_SIS_SIS966_SATA:
			devfn -= (4<<3);
			index = 22;
			i = (dev->path.pci.devfn) & 7;
			if (i>0) {
				index -= (i+3);
			}
			break;
		case PCI_DEVICE_ID_SIS_SIS966_PCIE:
			devfn -= (0x9<<3);  // to LPC
			index2 = 9;
			break;
		default:
			index = 0;
	}

	if (!lpc_dev)
		lpc_dev = find_lpc_dev(dev, devfn);

	if ( !lpc_dev )	return;

	if (index2!=0) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if (!sm_dev) return;

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

	if ( index == 16) {
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if (!sm_dev) return;

		final_reg = pci_read_config32(sm_dev, 0xe8);
		final_reg &= ~0x0057cf00;
		pci_write_config32(sm_dev, 0xe8, final_reg); //enable all at first
	}

	if (!dev->enabled) {
		final_reg |= (1 << index);// disable it
		/*
		 * The reason for using final_reg is that if func 1 is disabled,
		 * then func 2 will become func 1.
		 * Because of this, we need loop through disabling them all at
		 * the same time.
		 */
	}

	if (index == 9 ) { //NIC1 is the final, We need update final reg to 0xe8
		sm_dev = dev_find_slot(dev->bus->secondary, devfn + 1);
		if (!sm_dev) return;
		reg_old = pci_read_config32(sm_dev, 0xe8);
		if (final_reg != reg_old) {
			pci_write_config32(sm_dev, 0xe8, final_reg);
		}

	}
}

struct chip_operations southbridge_sis_sis966_ops = {
	CHIP_NAME("SiS SiS966 Southbridge")
	.enable_dev	= sis966_enable,
};
