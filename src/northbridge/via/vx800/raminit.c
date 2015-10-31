/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#include <spd.h>
#include <delay.h>

#ifdef __clang__
/* Silence clang warnings via pragmas to avoid the problems in this file
   blocking analyzes for the rest of the tree.  */
#pragma clang diagnostic ignored "-Wsometimes-uninitialized"
#pragma clang diagnostic ignored "-Wconstant-logical-operand"
#pragma clang diagnostic ignored "-Warray-bounds"
#endif

#if CONFIG_DEBUG_RAM_SETUP
#define PRINT_DEBUG_MEM(x)		printk(BIOS_DEBUG, x)
#define PRINT_DEBUG_MEM_HEX8(x)		printk(BIOS_DEBUG, "%02x", x)
#define PRINT_DEBUG_MEM_HEX16(x)	printk(BIOS_DEBUG, "%04x", x)
#define PRINT_DEBUG_MEM_HEX32(x)	printk(BIOS_DEBUG, "%08x", x)
#define DUMPNORTH()			dump_pci_device(PCI_DEV(0, 0, 0))
#else
#define PRINT_DEBUG_MEM(x)
#define PRINT_DEBUG_MEM_HEX8(x)
#define PRINT_DEBUG_MEM_HEX16(x)
#define PRINT_DEBUG_MEM_HEX32(x)
#define DUMPNORTH()
#endif
#include "northbridge/via/vx800/translator_ddr2_init.c"
#include "northbridge/via/vx800/dram_init.h"
#include "northbridge/via/vx800/early_smbus.c"
#include "northbridge/via/vx800/early_serial.c"
#include "northbridge/via/vx800/dram_util.h"
#include "northbridge/via/vx800/dram_util.c"
#include "northbridge/via/vx800/detection.c"
#include "northbridge/via/vx800/freq_setting.c"
#include "northbridge/via/vx800/timing_setting.c"
#include "northbridge/via/vx800/drdy_bl.c"
#include "northbridge/via/vx800/driving_setting.c"
#include "northbridge/via/vx800/clk_ctrl.c"
#include "northbridge/via/vx800/dev_init.c"
#include "northbridge/via/vx800/rank_map.c"
#include "northbridge/via/vx800/dqs_search.c"
#include "northbridge/via/vx800/final_setting.c"
#include "northbridge/via/vx800/uma_ram_setting.c"
#include "northbridge/via/vx800/dram_init.c"
/*
 * Support one dimm with up to 2 ranks
 */

static void ddr2_ram_setup(void)
{
	CB_STATUS Status;
	PRINT_DEBUG_MEM("In ddr2_ram_setup\r");

	Status = DDR2_DRAM_INIT();
	if (CB_SUCCESS != Status) {
		PRINT_DEBUG_MEM("Dram init error. Status = %x\r");
	}

}
