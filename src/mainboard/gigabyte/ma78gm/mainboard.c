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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
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
 * ma78gm-us2h uses GPIO 6 as PCIe slot reset, GPIO4 as GFX slot reset. We need to
 * pull it up before training the slot.
 ***/
void set_pcie_dereset()
{
	u16 word;
	device_t sm_dev;
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
	device_t sm_dev;
	/* GPIO 6 reset PCIe slot, GPIO 4 reset GFX PCIe */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	word = pci_read_config16(sm_dev, 0xA8);
	word &= ~((1 << 0) | (1 << 2));	/* Set Gpio6,4 as output */
	word &= ~((1 << 8) | (1 << 10));
	pci_write_config16(sm_dev, 0xA8, word);
}


u8 is_dev3_present(void)
{
	return 0;
}

/*************************************************
* enable the dedicated function in board.
* This function called early than rs780_enable.
*************************************************/
static void mainboard_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard MA78GM-US2H Enable. dev=0x%p\n", dev);

	set_pcie_dereset();
	/* get_ide_dma66(); */
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
