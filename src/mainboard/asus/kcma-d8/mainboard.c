/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb700/sb700.h>
#include <southbridge/amd/sr5650/cmn.h>

void set_pcie_reset(void)
{
	struct device *pcie_core_dev;

	pcie_core_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	set_htiu_enable_bits(pcie_core_dev, 0xA8, 0xFFFFFFFF, 0x28282828);
	set_htiu_enable_bits(pcie_core_dev, 0xA9, 0x000000FF, 0x00000028);
}

void set_pcie_dereset(void)
{
	struct device *pcie_core_dev;

	pcie_core_dev = dev_find_slot(0, PCI_DEVFN(0, 0));
	set_htiu_enable_bits(pcie_core_dev, 0xA8, 0xFFFFFFFF, 0x6F6F6F6F);
	set_htiu_enable_bits(pcie_core_dev, 0xA9, 0x000000FF, 0x0000006F);
}

/*************************************************
* enable the dedicated function in kgpe-d16 board.
* This function is called earlier than sr5650_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard KCMA-D8 initializing, dev=0x%p\n", dev);

	msr_t msr, msr2;

	/* TOP_MEM: the top of DRAM below 4G */
	msr = rdmsr(TOP_MEM);
	printk
	    (BIOS_INFO, "%s, TOP MEM: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
	     __func__, msr.lo, msr.hi);

	/* TOP_MEM2: the top of DRAM above 4G */
	msr2 = rdmsr(TOP_MEM2);
	printk
	    (BIOS_INFO, "%s, TOP MEM2: msr2.lo = 0x%08x, msr2.hi = 0x%08x\n",
	     __func__, msr2.lo, msr2.hi);

	set_pcie_dereset();
	/* get_ide_dma66(); */
}

/* override the default SATA PHY setup */
void sb7xx_51xx_setup_sata_phys(struct device *dev)
{
	/* RPR7.6.1 Program the PHY Global Control to 0x2C00 */
	pci_write_config16(dev, 0x86, 0x2c00);

	/* RPR7.6.2 SATA GENI PHY ports setting */
	pci_write_config32(dev, 0x88, 0x01b48016);
	pci_write_config32(dev, 0x8c, 0x01b48016);
	pci_write_config32(dev, 0x90, 0x01b48016);
	pci_write_config32(dev, 0x94, 0x01b48016);
	pci_write_config32(dev, 0x98, 0x01b48016);
	pci_write_config32(dev, 0x9c, 0x01b48016);

	/* RPR7.6.3 SATA GEN II PHY port setting for port [0~5]. */
	pci_write_config16(dev, 0xa0, 0xa07a);
	pci_write_config16(dev, 0xa2, 0xa07a);
	pci_write_config16(dev, 0xa4, 0xa07a);
	pci_write_config16(dev, 0xa6, 0xa07a);
	pci_write_config16(dev, 0xa8, 0xa07a);
	pci_write_config16(dev, 0xaa, 0xa07a);
}

/* override the default SATA port setup */
void sb7xx_51xx_setup_sata_port_indication(void *sata_bar5)
{
	uint32_t dword;

	/* RPR7.9 Program Port Indication Registers */
	dword = read32(sata_bar5 + 0xf8);
	dword &= ~(0x3f << 12);	/* All ports are iSATA */
	dword &= ~0x3f;
	write32(sata_bar5 + 0xf8, dword);

	dword = read32(sata_bar5 + 0xfc);
	dword &= ~(0x1 << 20);	/* No eSATA ports are present */
	write32(sata_bar5 + 0xfc, dword);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
