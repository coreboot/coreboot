/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#define ASSEMBLY 1
#define __PRE_RAM__

#define RAMINIT_SYSINFO 1
#define CACHE_AS_RAM_ADDRESS_DEBUG 0

#define SET_NB_CFG_54 1

//used by raminit
#define QRANK_DIMM_SUPPORT 1

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include "option_table.h"
#include "pc80/mc146818rtc_early.c"
#include "pc80/serial.c"
#if CONFIG_USE_INIT == 0
	#include "lib/memcpy.c"
#endif
#include "arch/i386/lib/console.c"

#include <cpu/amd/model_10xxx_rev.h>
#include "northbridge/amd/amdfam10/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"

#include "lib/delay.c"

#if NODE_NUMS == 64
	 #define NODE_PCI(x,fn) ((x<32)?PCI_DEV(CONFIG_CBB,CONFIG_CDB+x,fn):PCI_DEV(CONFIG_CBB-1, CONFIG_CDB+x-32, fn))
#else
	 #define NODE_PCI(x, fn) PCI_DEV(CONFIG_CBB,CONFIG_CDB+x,fn)
#endif

//#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdfam10/reset_test.c"
#include "northbridge/amd/amdfam10/debug.c"
#include "southbridge/amd/amd8111/amd8111_early_ctrl.c"
#include "northbridge/amd/amdfam10/amdfam10.h"

#include "cpu/x86/mtrr.h"
#include "cpu/amd/mtrr.h"
#include "cpu/x86/tsc.h"

#include "northbridge/amd/amdfam10/amdfam10_pci.c"
#include "northbridge/amd/amdfam10/amdfam10_conf.c"
#include "northbridge/amd/amdfam10/raminit_ddr2_dqs.c"

#include "cpu/amd/quadcore/quadcore.c"

void hardwaremain(int ret_addr)
{
	struct sys_info *sysinfo = (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in CACHE
	struct sys_info *sysinfox = ((CONFIG_RAMTOP) - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in RAM

	struct node_core_id id;

	id = get_node_core_id_x();

	printk_debug("CODE IN CACHE ON NODE: %02x\n");

	train_ram(id.nodeid, sysinfo, sysinfox);

	/* go back, but can not use stack any more, because we only keep
	ret_addr and can not restore esp, and ebp */

	__asm__ volatile (
		"movl	%0, %%edi\n\t"
		"jmp	  *%%edi\n\t"
		:: "a"(ret_addr)
	);



}

#include <arch/registers.h>

void x86_exception(struct eregs *info)
{
	do {
		hlt();
	} while(1);
}


