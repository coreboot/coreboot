/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
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
#include "SBPLATFORM.h" 	/* Platfrom Specific Definitions */
#include "chip.h"


void set_pcie_reset(void);
void set_pcie_dereset(void);

/**
 * TODO
 * SB CIMx callback
 */
void set_pcie_reset(void)
{
}

/**
 * TODO
 * mainboard specific SB CIMx callback
 */
void set_pcie_dereset(void)
{
}

/**
 * Southstation using SB GPIO 17/18 to control the Red/Green LED
 * These two LEDs can be used to show the OS booting status.
 */
static void southstation_led_init(void)
{
#define GPIO_FUNCTION	2  //GPIO function
#define SB_GPIO_REG17	17 //Red  Light
#define SB_GPIO_REG18	18 //Green Light

	/* multi-function pins switch to GPIO0-35 */
	RWMEM(ACPI_MMIO_BASE + PMIO_BASE + SB_PMIOA_REGEA, AccWidthUint8, ~BIT0, 1);

	/* select IOMux to function2, corresponds to GPIO */
	RWMEM(ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG17, AccWidthUint8, ~(BIT0 | BIT1), GPIO_FUNCTION);
	RWMEM(ACPI_MMIO_BASE + IOMUX_BASE + SB_GPIO_REG18, AccWidthUint8, ~(BIT0 | BIT1), GPIO_FUNCTION);

	/* Lighting test */
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG17, AccWidthUint8, ~(0xFF), 0x08); //output high
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG18, AccWidthUint8, ~(0xFF), 0x08);
	mdelay(100);
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG17, AccWidthUint8, ~(0xFF), 0x48); //output low
	RWMEM(ACPI_MMIO_BASE + GPIO_BASE + SB_GPIO_REG18, AccWidthUint8, ~(0xFF), 0x48);
}


/*************************************************
* enable the dedicated function in southstation board.
*************************************************/
static void southstation_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

#if CONFIG_GFXUMA
	msr_t msr, msr2;
	uint32_t sys_mem;

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

	/* refer to UMA Size Consideration in Family14h BKDG. */
	sys_mem = msr.lo + 0x1000000; // Ignore 16MB allocated for C6 when finding UMA size, refer MemNGetUmaSizeON()
	if ((msr.hi & 0x0000000F) || (sys_mem >= 0x80000000)) {
		uma_memory_size = 0x18000000;	/* >= 2G memory, 384M recommended UMA */
	}
	else {
	  if (sys_mem >= 0x40000000) {
		  uma_memory_size = 0x10000000;	/* >= 1G memory, 256M recommended UMA */
	  }
	  else {
		  uma_memory_size = 0x4000000;	/* <1G memory, 64M recommended UMA */
	  }
	}

	uma_memory_base = msr.lo - uma_memory_size;	/* TOP_MEM1 */
	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
		    __func__, uma_memory_size, uma_memory_base);

	/* TODO: TOP_MEM2 */
#else
	uma_memory_size = 0x10000000;	/* 256M recommended UMA */
	uma_memory_base = 0x30000000;	/* 1GB  system memory supported */
#endif
	southstation_led_init();
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
	CHIP_NAME(CONFIG_MAINBOARD_VENDOR " " CONFIG_MAINBOARD_PART_NUMBER " Mainboard")
	.enable_dev = southstation_enable,
};
