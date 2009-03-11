/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@lanl.gov>
 * Copyright (C) 2007 coresystems GmbH
 * (Written by Stefan Reinauer <stepan@coresystems.de> for coresystems GmbH)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

/* stage2 - coreboot RAM-based setup. */

#include <types.h>
#include <string.h>
#include <lar.h>
#include <console.h>
#include <device/device.h>
#include <tables.h>
#include <globalvars.h>
#include <lib.h>

/**
 * CPU init code which runs BEFORE any stage2 dev_phase1 is run. 
 * This code might, for example, init ECC on all cores. 
 *
 * @param coldboot Is this a power-on coldboot 
 * @param sysinfo sysinfo pointer
 * @returns 0 on success; error number otherwise
 */
unsigned int __attribute__((weak)) cpu_phase1(unsigned int coldboot,
			struct sys_info *sysinfo)
{
	printk(BIOS_SPEW, "cpu_phase1: %s: nothing to do.\n", coldboot? "Coldboot" : "Warmboot");
	return 0;
}

/**
 * CPU init code which runs AFTER ALL stage2 dev_phases are run. 
 * This code might, for example, install an SMI handler
 *
 * @param coldboot Is this a power-on coldboot 
 * @param sysinfo sysinfo pointer
 * @returns 0 on success; error number otherwise
 */
unsigned int __attribute__((weak)) cpu_phase2(unsigned int coldboot,
			struct sys_info *sysinfo)
{
	printk(BIOS_SPEW, "cpu_phase2: %s: nothing to do.\n", coldboot? "Coldboot" : "Warmboot");
	return 0;
}

/**
 * Main function of the DRAM part of coreboot.
 *
 * Coreboot is divided into pre-DRAM part and DRAM part. The stages before
 * this part are stage 0 and stage 1. This part contains stage 2, which
 * consists of phases 1 through 6.
 * printk has been set up in stage 1 and is working.
 *
 * Device Enumeration: in the dev_enumerate() phase.
 *
 * TODO:
 * - Check whether this documentation is still correct. Improve it.
 */
void *stage2(void)
{
	void *mbi;
	struct sys_info *sysinfo;
	struct global_vars *global_vars(void);
	post_code(POST_STAGE2_BEGIN);
	sysinfo = &(global_vars()->sys_info);
	cpu_phase1(is_coldboot(), sysinfo);
	dev_init();

	/* Phase 1 was console init and making printk work. Both functions are
	 * now performed by stage 1 code. Phase 1 is now without purpose.
	 */
	post_code(POST_STAGE2_PHASE1_START);
	dev_phase1();
	show_all_devs(BIOS_DEBUG, "After phase 1.");

	/* Here is where weird stuff like init_timer handling should be
	 * done. This is for ANYTHING that might have to happen before
	 * device enumeration but that needs a printk.
	 */
	post_code(POST_STAGE2_PHASE2_START);
	dev_phase2();
	show_all_devs(BIOS_DEBUG, "After phase 2.");

	/* Walk physical devices and add any dynamic devices to the
	 * device tree.
	 */
	post_code(POST_STAGE2_PHASE3_START);
	dev_root_phase3();
	show_all_devs_tree(BIOS_DEBUG, "After phase 3.");

	/* Compute and assign the bus resources. */
	post_code(POST_STAGE2_PHASE4_START);
	dev_phase4();
	show_all_devs(BIOS_DEBUG, "After phase 4.");

	/* Now actually enable devices on the bus. */
	post_code(POST_STAGE2_PHASE5_START);
	dev_root_phase5();
	show_all_devs(BIOS_DEBUG, "After phase 5.");

	/* Initialize devices on the bus. */
	post_code(POST_STAGE2_PHASE6_START);
	dev_phase6();
	show_all_devs(BIOS_DEBUG, "After phase 6.");

	cpu_phase2(is_coldboot(), sysinfo);

	/* Write tables to pass information to the payloads. */
	post_code(POST_STAGE2_WRITE_TABLES);
	mbi = write_tables();
	show_all_devs(BIOS_DEBUG, "After writing tables.");

	return mbi;
}

EXPORT_SYMBOL(stage2);
