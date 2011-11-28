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
#include "southbridge/amd/sb700/sb700.h"
#include "southbridge/amd/sb700/smbus.h"
#include "chip.h"

#define ADT7461_ADDRESS 0x4C
#define ARA_ADDRESS     0x0C /* Alert Response Address */

#define ADT7461_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address)
#define ARA_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ARA_ADDRESS, address)
#define ADT7461_write_byte(address, val) \
	do_smbus_write_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address, val)

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

/*
 * justify the dev3 is exist or not
 * NOTE: This just copied from AMD Tilapia code.
 * It is completly unknown it it will work at all for ASUS M4A785-M.
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

/*
 * set thermal config
 */
static void set_thermal_config(void)
{
	u8 byte;
	u16 word;
	device_t sm_dev;

	/* set ADT 7461 */
	ADT7461_write_byte(0x0B, 0x50);	/* Local Temperature Hight limit */
	ADT7461_write_byte(0x0C, 0x00);	/* Local Temperature Low limit */
	ADT7461_write_byte(0x0D, 0x50);	/* External Temperature Hight limit  High Byte */
	ADT7461_write_byte(0x0E, 0x00);	/* External Temperature Low limit High Byte */

	ADT7461_write_byte(0x19, 0x55);	/* External THERM limit */
	ADT7461_write_byte(0x20, 0x55);	/* Local THERM limit */

	byte = ADT7461_read_byte(0x02);	/* read status register to clear it */
	ARA_read_byte(0x05); /* A hardware alert can only be cleared by the master sending an ARA as a read command */
	printk(BIOS_INFO, "Init adt7461 end , status 0x02 %02x\n", byte);

	/* sb700 settings for thermal config */
	/* set SB700 GPIO 64 to GPIO with pull-up */
	byte = pm2_ioread(0x42);
	byte &= 0x3f;
	pm2_iowrite(0x42, byte);

	/* set GPIO 64 to input */
	sm_dev = dev_find_slot(0, PCI_DEVFN(0x14, 0));
	word = pci_read_config16(sm_dev, 0x56);
	word |= 1 << 7;
	pci_write_config16(sm_dev, 0x56, word);

	/* set GPIO 64 internal pull-up */
	byte = pm2_ioread(0xf0);
	byte &= 0xee;
	pm2_iowrite(0xf0, byte);

	/* set Talert to be active low */
	byte = pm_ioread(0x67);
	byte &= ~(1 << 5);
	pm_iowrite(0x67, byte);

	/* set Talert to generate ACPI event */
	byte = pm_ioread(0x3c);
	byte &= 0xf3;
	pm_iowrite(0x3c, byte);

	/* THERMTRIP pin */
	/* byte = pm_ioread(0x68);
	 * byte |= 1 << 3;
	 * pm_iowrite(0x68, byte);
	 *
	 * byte = pm_ioread(0x55);
	 * byte |= 1 << 0;
	 * pm_iowrite(0x55, byte);
	 *
	 * byte = pm_ioread(0x67);
	 * byte &= ~( 1 << 6);
	 * pm_iowrite(0x67, byte);
	 */
}

/*************************************************
* enable the dedicated function in this board.
* This function called early than rs780_enable.
*************************************************/
static void m4a785m_enable(device_t dev)
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
	set_thermal_config();
}

struct chip_operations mainboard_ops = {
#ifdef CONFIG_BOARD_ASUS_M4A785TM
	CHIP_NAME("ASUS M4A785T-M Mainboard")
#else
	CHIP_NAME("ASUS M4A785-M Mainboard")
#endif
	.enable_dev = m4a785m_enable,
};
