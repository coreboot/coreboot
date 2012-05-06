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
#include <boot/tables.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <device/pci_def.h>
#include "southbridge/amd/sb700/sb700.h"
#include "southbridge/amd/sb700/smbus.h"
#include "chip.h"

uint64_t uma_memory_base, uma_memory_size;

void set_pcie_dereset(void);
void set_pcie_reset(void);
u8 is_dev3_present(void);

static void pcie_rst_toggle(u8 val) {
	u8 byte;

	byte = pm_ioread(0x8d);
	byte &= ~(3 << 1);
	pm_iowrite(0x8d, byte);

	byte = pm_ioread(0x94);
	/* Output enable */
	byte &= ~(3 << 2);
	/* Toggle GPM8, GPM9 */
	byte &= ~(3 << 0);
	byte |= val;
	pm_iowrite(0x94, byte);
}

void set_pcie_dereset()
{
    pcie_rst_toggle(0x3);
}

void set_pcie_reset()
{
    pcie_rst_toggle(0x0);
}

#if 0	     /* not tested yet */
/********************************************************
* mahogany uses SB700 GPIO9 to detect IDE_DMA66.
* IDE_DMA66 is routed to GPIO 9. So we read Gpio 9 to
* get the cable type, 40 pin or 80 pin?
********************************************************/
static void get_ide_dma66(void)
{
	u8 byte;
	/*u32 sm_dev, ide_dev; */
	device_t sm_dev, ide_dev;

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
#endif	/* get_ide_dma66 */

u8 is_dev3_present(void)
{
	return 0;
}

/*************************************************
* enable the dedicated function in mahogany board.
* This function called early than rs780_enable.
*************************************************/
static void mb_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard 939A785GMH/128M Enable. dev=0x%p\n", dev);

#if CONFIG_GFXUMA
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

	/* refer to UMA Size Consideration in 780 BDG. */
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
}

int add_mainboard_resources(struct lb_memory *mem)
{
	/* UMA is removed from system memory in the northbridge code, but
	 * in some circumstances we want the memory mentioned as reserved.
 	 */
#if CONFIG_GFXUMA
	printk(BIOS_INFO, "uma_memory_start=0x%llx, uma_memory_size=0x%llx \n",
		    uma_memory_base, uma_memory_size);
	lb_add_memory_range(mem, LB_MEM_RESERVED, uma_memory_base,
			    uma_memory_size);
#endif
	return 0;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("Asrock 939A785GMH/128M Mainboard")
	.enable_dev = mb_enable,
};

/* override the default SATA PHY setup */
void sb7xx_51xx_setup_sata_phys(struct device *dev) {
	/* RPR7.6.1 Program the PHY Global Control to 0x2C00 */
	pci_write_config16(dev, 0x86, 0x2c00);

	/* RPR7.6.2 SATA GENI PHY ports setting */
	pci_write_config32(dev, 0x88, 0x01B48016);
	pci_write_config32(dev, 0x8c, 0x01B48016);
	pci_write_config32(dev, 0x90, 0x01B48016);
	pci_write_config32(dev, 0x94, 0x01B48016);
	pci_write_config32(dev, 0x98, 0x01B48016);
	pci_write_config32(dev, 0x9C, 0x01B48016);

	/* RPR7.6.3 SATA GEN II PHY port setting for port [0~5]. */
	pci_write_config16(dev, 0xA0, 0xA07A);
	pci_write_config16(dev, 0xA2, 0xA07A);
	pci_write_config16(dev, 0xA4, 0xA07A);
	pci_write_config16(dev, 0xA6, 0xA07A);
	pci_write_config16(dev, 0xA8, 0xA07A);
	pci_write_config16(dev, 0xAA, 0xA0FF);
}
