/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include <NbPlatform.h>
#include "chip.h"

#define ONE_MB  0x100000
//#define SMBUS_IO_BASE 0x6000

void set_pcie_reset(void *nbconfig);
void set_pcie_dereset(void *nbconfig);

/**
 * TODO
 * SB CIMx callback
 */
void set_pcie_reset(void *nbconfig)
{
}

/**
 * Mainboard specific RD890 CIMx callback
 * Release Resets to PCIe Links
 * SR5690 PCIE_RESET_GPIO1,2,3,4 to reset pcie
 */
void set_pcie_dereset(void *nbconfig)
{
	//u32 nb_dev = MAKE_SBDFO(0, 0x0, 0x0, 0x0, 0x0);
	u32 i;
	u32 val;
	u32 nb_addr;

	val = 0x00000007UL;
	AMD_NB_CONFIG_BLOCK *pConfig = (AMD_NB_CONFIG_BLOCK*)nbconfig;
	for (i = 0; i < MAX_NB_COUNT; i ++) {
		nb_addr = pConfig->Northbridges[i].NbPciAddress.AddressValue | NB_HTIU_INDEX;
		LibNbPciIndexRMW(nb_addr,
				NB_HTIU_REGA8,
				AccessS3SaveWidth32,
				~val,
				val,
				&(pConfig->Northbridges[i]));
	}
}


/*************************************************
 * enable the dedicated function in dinar board.
 *************************************************/
static void dinar_enable(device_t dev)
{
	printk(BIOS_INFO, "Mainboard Dinar Enable. dev=0x%p\n", dev);
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
	printk (BIOS_INFO, "%s, TOP MEM2: msr2.lo = 0x%08x, msr2.hi = 0x%08x\n",
			__func__, msr2.lo, msr2.hi);

	/* refer to UMA Size Consideration in Family15h BKDG. */
	/* Please reference MemNGetUmaSizeOR () */
	/*
	 *     Total system memory   UMASize
	 *     >= 2G                 512M
	 *     >=1G                  256M
	 *     <1G                    64M
	 */
	sys_mem = msr.lo + 16 * ONE_MB;   // Ignore 16MB allocated for C6 when finding UMA size
	if ((msr2.hi & 0x0000000F) || (sys_mem >= 2048 * ONE_MB)) {
		uma_memory_size = 512 * ONE_MB;
	} else if (sys_mem >= 1024 * ONE_MB) {
		uma_memory_size = 256 * ONE_MB;
	} else {
		uma_memory_size = 64 * ONE_MB;
	}
	uma_memory_base = msr.lo - uma_memory_size; /* TOP_MEM1 */

	printk(BIOS_INFO, "%s: uma size 0x%08llx, memory start 0x%08llx\n",
			__func__, uma_memory_size, uma_memory_base);

	/* TODO: TOP_MEM2 */
#else
	uma_memory_size = 256 * ONE_MB; /* 256M recommended UMA */
	uma_memory_base = 768 * ONE_MB; /* 1GB  system memory supported */
#endif

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
	CHIP_NAME("AMD DINAR Mainboard")
		.enable_dev = dinar_enable,
};
