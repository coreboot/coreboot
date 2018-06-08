/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Alec Ari <neotheuser@ymail.com>
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
int is_dev3_present(void);

void set_pcie_dereset()
{
	u8 byte;
	u16 word;
	struct device *sm_dev;
	/* set 0 to bit1 :disable GPM9 as SLP_S2 output */
	/* set 0 to bit2 :disable GPM8 as AZ_RST output */
	byte = pm_ioread(0x8d);
	byte &= ~((1 << 1) | (1 << 2));
	pm_iowrite(0x8d, byte);

	/* set the GPM8 and GPM9 output enable and the value to 1 */
	byte = pm_ioread(0x94);
	byte &= ~((1 << 2) | (1 << 3));
	byte |=  ((1 << 0) | (1 << 1));
	pm_iowrite(0x94, byte);

	/* set the GPIO65 output enable and the value is 1 */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	word = pci_read_config16(sm_dev, 0x7e);
	word |= (1 << 0);
	word &= ~(1 << 4);
	pci_write_config16(sm_dev, 0x7e, word);
}

void set_pcie_reset()
{
	u8 byte;
	u16 word;
	struct device *sm_dev;

	/* set 0 to bit1 :disable GPM9 as SLP_S2 output */
	/* set 0 to bit2 :disable GPM8 as AZ_RST output */
	byte = pm_ioread(0x8d);
	byte &= ~((1 << 1) | (1 << 2));
	pm_iowrite(0x8d, byte);

	/* set the GPM8 and GPM9 output enable and the value to 0 */
	byte = pm_ioread(0x94);
	byte &= ~((1 << 2) | (1 << 3));
	byte &= ~((1 << 0) | (1 << 1));
	pm_iowrite(0x94, byte);

	/* set the GPIO65 output enable and the value is 0 */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	word = pci_read_config16(sm_dev, 0x7e);
	word &= ~(1 << 0);
	word &= ~(1 << 4);
	pci_write_config16(sm_dev, 0x7e, word);
}

/*
 * dev3 does not exist on ma785gm
 */
int is_dev3_present(void)
{
	return 0;
}

/*
 * set gpio40 gfx
 */
static void set_gpio40_gfx(void)
{
	u8 byte;
//	u16 word;
	u32 dword;
	struct device *sm_dev;
	/* disable the GPIO40 as CLKREQ2# function */
	byte = pm_ioread(0xd3);
	byte &= ~(1 << 7);
	pm_iowrite(0xd3, byte);

	/* disable the GPIO40 as CLKREQ3# function */
	byte = pm_ioread(0xd4);
	byte &= ~(1 << 0);
	pm_iowrite(0xd4, byte);

	/* enable pull up for GPIO68 */
	byte = pm2_ioread(0xf1);
	byte &=	~(1 << 4);
	pm2_iowrite(0xf1, byte);

	/* access the smbus extended register */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	/* set the gfx to 1x16 lanes */
	printk(BIOS_INFO, "Dev3 is not present. GFX Configuration is One x16 slot\n");
	/* when the gpio40 is configured as GPIO, this will enable the output */
	pci_write_config32(sm_dev, 0xf8, 0x4);
	dword = pci_read_config32(sm_dev, 0xfc);
	dword &= ~(1 << 10);

        /* When the gpio40 is configured as GPIO, this will represent the output value*/
	/* 1 :enable two x8  , 0 : master slot enable only */
	dword &=  ~(1 << 26);
	pci_write_config32(sm_dev, 0xfc, dword);
}

/*************************************************
* enable the dedicated function in ma785gm board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard MA785GM-US2H Enable. dev=0x%p\n", dev);

	set_pcie_dereset();
	/* get_ide_dma66(); */
	set_gpio40_gfx();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
