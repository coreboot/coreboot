/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 AMD
 * Written by Yinghai Lu <yinghailu@amd.com> for AMD.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#define K8_REV_F_SUPPORT_F0_F1_WORKAROUND 0

#include <stdint.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <device/pnp_def.h>
#include <arch/romcc_io.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include "pc80/serial.c"

#include "lib/uart8250.c"
#include "arch/i386/lib/printk_init.c"
#include "console/vtxprintf.c"
#include "console/console.c"

#include <cpu/amd/model_fxx_rev.h>
#include "northbridge/amd/amdk8/raminit.h"
#include "cpu/amd/model_fxx/apic_timer.c"

#include "lib/delay.c"

//#include "cpu/x86/lapic/boot_cpu.c"
#include "northbridge/amd/amdk8/reset_test.c"

#include "northbridge/amd/amdk8/debug.c"

#include "southbridge/nvidia/mcp55/mcp55_early_ctrl.c"

#include "northbridge/amd/amdk8/amdk8_f.h"

#include "cpu/x86/mtrr.h"
#include "cpu/amd/mtrr.h"
#include "cpu/x86/tsc.h"

#include "northbridge/amd/amdk8/amdk8_f_pci.c"
#include "northbridge/amd/amdk8/raminit_f_dqs.c"

#include "cpu/amd/dualcore/dualcore.c"

void hardwaremain(int ret_addr)
{
	struct sys_info *sysinfo = (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in CACHE
        struct sys_info *sysinfox = ((CONFIG_RAMTOP) - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE); // in RAM

	struct node_core_id id;

	id = get_node_core_id_x();

	//FIXME: for USBDEBUG you need to make sure dbg_info get assigned in AP
        print_debug("CODE IN CACHE ON NODE:"); print_debug_hex8(id.nodeid); print_debug("\n");

	train_ram(id.nodeid, sysinfo, sysinfox);

	/*
		go back, but can not use stack any more, because we only keep ret_addr and can not restore esp, and ebp
	*/

        __asm__ volatile (
                "movl  %0, %%edi\n\t"
                "jmp     *%%edi\n\t"
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


