/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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

#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <console/console.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>

static void dram_enable(struct device *dev)
{
	msr_t msr;
	u16 reg;

	/*
	 * Enable Lowest Interrupt arbitration for APIC, enable NB APIC
	 * decoding, MSI support, no SMRAM, compatible SMM.
	 */
	pci_write_config8(dev, 0x86, 0x39);

	/*
	 * We want to use the 0xC0000-0xEFFFF as RAM mark area as RW, even if
	 * memory is doing K8 the DMA from SB will fail if we have it wrong,
	 * AND even we have it here, we must later copy it to SB to make it work :/
	 */

	/* For CC000-CFFFF, bits 7:6 (10 = REn, 01 = WEn) bits 1:0 for
	 * C0000-C3FFF etc.
	 */
	pci_write_config8(dev, 0x80, 0xff);
	/* For page D0000-DFFFF */
	pci_write_config8(dev, 0x81, 0xff);
	/* For page E0000-EFFFF */
	pci_write_config8(dev, 0x82, 0xff);
	pci_write_config8(dev, 0x83, 0x30);

	msr = rdmsr(TOP_MEM);
	reg = pci_read_config16(dev, 0x84);
	reg &= 0xf;
	pci_write_config16(dev, 0x84, (msr.lo >> 16) | reg);

	reg = pci_read_config16(dev, 0x88);
	reg &= 0xf800;

	pci_write_config16(dev, 0x88, (msr.lo >> 24) | reg);
}

static const struct device_operations dram_ops = {
	.read_resources		= pci_dev_read_resources,
	.set_resources		= pci_dev_set_resources,
	.enable_resources	= pci_dev_enable_resources,
	.enable			= dram_enable,
	.ops_pci		= 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops	= &dram_ops,
	.vendor	= PCI_VENDOR_ID_VIA,
	.device	= PCI_DEVICE_ID_VIA_K8T890CE_3,
};
