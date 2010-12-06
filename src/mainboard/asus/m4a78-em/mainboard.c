/*
 * This file is part of the coreboot project.
 *
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <arch/io.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include <southbridge/amd/sb700/sb700.h>
#include "chip.h"


extern int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
extern int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address,
			       u8 val);


#define SMBUS_IO_BASE 0x6000

uint64_t uma_memory_base, uma_memory_size;

void set_pcie_dereset(void);
void set_pcie_reset(void);
u8 is_dev3_present(void);

void set_pcie_dereset()
{
	u8 byte;
	u16 word;
	device_t sm_dev;
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
	device_t sm_dev;

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

#if 0	     /* TODO: */
/********************************************************
* tilapia uses SB700 GPIO8 to detect IDE_DMA66.
* IDE_DMA66 is routed to GPIO 8. So we read Gpio 8 to
* get the cable type, 40 pin or 80 pin?
********************************************************/
static void get_ide_dma66(void)
{
	u8 byte;
	/*u32 sm_dev, ide_dev; */
	device_t sm_dev, ide_dev;

	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	byte = pci_read_config8(sm_dev, 0xA9);
	byte |= (1 << 4);	/* Set Gpio8 as input */
	pci_write_config8(sm_dev, 0xA9, byte);

	ide_dev = dev_find_slot(0, PCI_DEVFN(0x14, 1));
	byte = pci_read_config8(ide_dev, 0x56);
	byte &= ~(7 << 0);
	if ((1 << 4) & pci_read_config8(sm_dev, 0xAA))
		byte |= 2 << 0;	/* mode 2 */
	else
		byte |= 5 << 0;	/* mode 5 */
	pci_write_config8(ide_dev, 0x56, byte);
}
#endif

/*
 * justify the dev3 is exist or not
 * NOTE: This just copied from AMD Tilapia code.
 * It is completly unknown if it will work at all for this board.
 */
u8 is_dev3_present(void)
{
	u16 word;
	device_t sm_dev;

	/* access the smbus extended register */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));

	/* put the GPIO68 output to tristate */
	word = pci_read_config16(sm_dev, 0x7e);
	word |= 1 << 6;
	pci_write_config16(sm_dev, 0x7e,word);

	/* read the GPIO68 input status */
	word = pci_read_config16(sm_dev, 0x7e);

	if(word & (1 << 10)){
		/*not exist*/
		return 0;
	}else{
		/*exist*/
		return 1;
	}
}


/*************************************************
* enable the dedicated function in this board.
* This function called early than rs780_enable.
*************************************************/
static void m4a78em_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard enable. dev=0x%p\n", dev);

#if (CONFIG_GFXUMA == 1)
	msr_t msr, msr2;

	/* TOP_MEM: the top of DRAM below 4G */
	msr = rdmsr(TOP_MEM);
	printk(BIOS_INFO,
	    "%s, TOP MEM: msr.lo = 0x%08x, msr.hi = 0x%08x\n",
	     __func__, msr.lo, msr.hi);

	/* TOP_MEM2: the top of DRAM above 4G */
	msr2 = rdmsr(TOP_MEM2);
	printk(BIOS_INFO,
	    "%s, TOP MEM2: msr2.lo = 0x%08x, msr2.hi = 0x%08x\n",
	     __func__, msr2.lo, msr2.hi);

	switch (msr.lo) {
	case 0x10000000:	/* 256M system memory */
		uma_memory_size = 0x4000000;	/* 64M recommended UMA */
		break;

	case 0x20000000:	/* 512M system memory */
		uma_memory_size = 0x8000000;	/* 128M recommended UMA */
		break;

	default:		/* 1GB and above system memory */
		uma_memory_size = 0x10000000;	/* 256M recommended UMA */
		break;
	}

	uma_memory_base = msr.lo - uma_memory_size;	/* TOP_MEM1 */
	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
		    __func__, uma_memory_size, uma_memory_base);

	/* TODO: TOP_MEM2 */
#else
	uma_memory_size = 0x8000000;	/* 128M recommended UMA */
	uma_memory_base = 0x38000000;	/* 1GB  system memory supposed */
#endif

	set_pcie_dereset();
	/* get_ide_dma66(); */
	/* set_thermal_config(); */
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS M4A78-EM Mainboard")
	.enable_dev = m4a78em_enable,
};
