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
//#include <southbridge/amd/sb900/sb900.h>
#include "chip.h"

#define ONE_MB  0x100000
//#define SMBUS_IO_BASE 0x6000

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


/*************************************************
* enable the dedicated function in torpedo board.
*************************************************/
static void torpedo_enable(device_t dev)
{
  printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable. dev=0x%p\n", dev);
  setup_uma_memory();
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
  .enable_dev = torpedo_enable,
};
