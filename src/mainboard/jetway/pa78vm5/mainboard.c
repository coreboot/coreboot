/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Wang Qing Pei <wangqingpei@gmail.com>
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
#include "southbridge/amd/sb700/sb700.h"
#include "southbridge/amd/sb700/smbus.h"

void set_pcie_dereset(void);
void set_pcie_reset(void);
u8 is_dev3_present(void);

/*
 * the board uses GPIO 6 as PCIe slot reset, GPIO4 as GFX slot reset. We need to
 * pull it up before training the slot.
 ***/
void set_pcie_dereset()
{
	u16 word;
	struct device *sm_dev;
	/* GPIO 6 reset PCIe slot, GPIO 4 reset GFX PCIe */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	word = pci_read_config16(sm_dev, 0xA8);
	word |= (1 << 0) | (1 << 2);	/* Set Gpio6,4 as output */
	word &= ~((1 << 8) | (1 << 10));
	pci_write_config16(sm_dev, 0xA8, word);
}

void set_pcie_reset()
{
	u16 word;
	struct device *sm_dev;
	/* GPIO 6 reset PCIe slot, GPIO 4 reset GFX PCIe */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	word = pci_read_config16(sm_dev, 0xA8);
	word &= ~((1 << 0) | (1 << 2));	/* Set Gpio6,4 as output */
	word &= ~((1 << 8) | (1 << 10));
	pci_write_config16(sm_dev, 0xA8, word);
}

#if 0	     /* not tested yet. */
/********************************************************
* board uses SB700 GPIO9 to detect IDE_DMA66.
* IDE_DMA66 is routed to GPIO 9. So we read Gpio 9 to
* get the cable type, 40 pin or 80 pin?
********************************************************/
static void get_ide_dma66(void)
{
	u8 byte;
	/*u32 sm_dev, ide_dev; */
	struct device *sm_dev, ide_dev;

	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	byte = pci_read_config8(sm_dev, 0xA9);
	byte |= (1 << 5);	/* Set Gpio9 as input */
	pci_write_config8(sm_dev, 0xA9, byte);

	ide_dev = dev_find_slot(0, PCI_DEVFN(0x14, 1));
	byte = pci_read_config8(ide_dev, 0x56);
	byte &= ~(7 << 0);
	if ((1 << 5) & pci_read_config8(sm_dev, 0xAA))
		byte |= 2 << 0;	/* mode 2 */
	else
		byte |= 5 << 0;	/* mode 5 */
	pci_write_config8(ide_dev, 0x56, byte);
}
#endif	/* get_ide_dma66() */

u8 is_dev3_present(void)
{
	return 0;
}

/*************************************************
* enable the dedicated function in this board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard PA78VM5 Enable. dev=0x%p\n", dev);

	set_pcie_dereset();
	/* get_ide_dma66(); */
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
