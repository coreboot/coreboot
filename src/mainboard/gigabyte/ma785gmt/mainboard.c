/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Wang Qing Pei <wangqingpei@gmail.com>
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
#include <southbridge/amd/sb700/sb700.h>
#include "chip.h"

#define ADT7461_ADDRESS 0x4C
#define ARA_ADDRESS     0x0C /* Alert Response Address */

extern int do_smbus_read_byte(u32 smbus_io_base, u32 device, u32 address);
extern int do_smbus_write_byte(u32 smbus_io_base, u32 device, u32 address,
			       u8 val);

#define ADT7461_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address)
#define ARA_read_byte(address) \
	do_smbus_read_byte(SMBUS_IO_BASE, ARA_ADDRESS, address)
#define ADT7461_write_byte(address, val) \
	do_smbus_write_byte(SMBUS_IO_BASE, ADT7461_ADDRESS, address, val)

#define SMBUS_IO_BASE 0x6000

uint64_t uma_memory_base, uma_memory_size;

void set_pcie_dereset(void);
void set_pcie_reset(void);
int is_dev3_present(void);

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
 */
int is_dev3_present(void)
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
 * set gpio40 gfx
 */
static void set_gpio40_gfx(void)
{
	u8 byte;
//	u16 word;
	u32 dword;
	device_t sm_dev;
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

	/*if the dev3 is present, set the gfx to 2x8 lanes*/
	/*otherwise set the gfx to 1x16 lanes*/
	if(is_dev3_present()){

		printk(BIOS_INFO, "Dev3 is present. GFX Configuration is Two x8 slots\n");
		/* when the gpio40 is configured as GPIO, this will enable the output */
		pci_write_config32(sm_dev, 0xf8, 0x4);
		dword = pci_read_config32(sm_dev, 0xfc);
		dword &= ~(1 << 10);

	        /* When the gpio40 is configured as GPIO, this will represent the output value*/
		/* 1 :enable two x8  , 0 : master slot enable only */
		dword |= (1 << 26);
		pci_write_config32(sm_dev, 0xfc, dword);

	}else{
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
* enable the dedicated function in ma785gmt board.
* This function called early than rs780_enable.
*************************************************/
static void ma785gmt_enable(device_t dev)
{
	/* Leave it for furture use. */
	/* struct mainboard_config *mainboard =
	   (struct mainboard_config *)dev->chip_info; */

	printk(BIOS_INFO, "Mainboard Gigabyte ma785gmt Enable. dev=0x%p\n", dev);

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
	set_gpio40_gfx();
}

int add_mainboard_resources(struct lb_memory *mem)
{
	/* UMA is removed from system memory in the northbridge code, but
	 * in some circumstances we want the memory mentioned as reserved.
 	 */
#if (CONFIG_GFXUMA == 1)
	printk(BIOS_INFO, "uma_memory_start=0x%llx, uma_memory_size=0x%llx \n",
		    uma_memory_base, uma_memory_size);
	lb_add_memory_range(mem, LB_MEM_RESERVED, uma_memory_base,
			    uma_memory_size);
#endif
	return 0;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("GIGABYTE MA785GMT   Mainboard")
	.enable_dev = ma785gmt_enable,
};
