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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


//#define SYSTEM_TYPE 0	/* SERVER */
#define SYSTEM_TYPE 1	/* DESKTOP */
//#define SYSTEM_TYPE 2	/* MOBILE */

//used by incoherent_ht
#define FAM10_SCAN_PCI_BUS 0
#define FAM10_ALLOCATE_IO_RANGE 0

#include <lib.h>
#include <reset.h>
#include <stdint.h>
#include <arch/io.h>
#include <arch/cpu.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <arch/stages.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/lapic/boot_cpu.c"
#include "agesawrapper.h"
#include "northbridge/amd/agesa/family10/reset_test.h"
#include "superio/ite/it8721f/early_serial.c"
#include "drivers/pc80/i8254.c"
#include "drivers/pc80/i8259.c"
#include "southbridge/amd/rs780/early_setup.c"
#include <sb_cimx.h>
#include <SBPLATFORM.h> /* SB OEM constants */

extern void disable_cache_as_ram(void); /* cache_as_ram.inc */

#define SERIAL_DEV PNP_DEV(0x4e, IT8721F_SP1)
void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;


	post_code(0x30);

    agesawrapper_amdinitmmio();

	post_code(0x32);
	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

    enable_rs780_dev8();
	sb800_clk_output_48Mhz();

	it8721f_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
	uart_init();
	console_init();
	printk(BIOS_DEBUG, "\n");

//	dump_mem(CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x200, CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	// Load MPB
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

 	post_code(0x37);
	printk(BIOS_DEBUG, "agesawrapper_amdinitreset ");
	val = agesawrapper_amdinitreset();
	if (val)
		printk(BIOS_DEBUG, "error level: %x \n", val);
	else
		printk(BIOS_DEBUG, "passed.\n");
                                           
    if (!cpu_init_detectedx && boot_cpu()) {
	    post_code(0x38);

		//enable port80 decoding and southbridge poweron init
		post_code(0x3A);
		sb_Poweron_Init(); 
	}

	post_code(0x3B);
    rs780_early_setup();
	printk(BIOS_DEBUG, "agesawrapper_amdinitearly ");
	val = agesawrapper_amdinitearly();
	if (val)
		printk(BIOS_DEBUG, "error level: %x \n", val);
	else
		printk(BIOS_DEBUG, "passed.\n");    	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */


    rs780_htinit();

	if (!warm_reset_detect(0)) {
		print_info("...WARM RESET...\n\n\n");
		distinguish_cpu_resets(0);
		soft_reset();
		die("After soft_reset_x - shouldn't see this message!!!\n");
	}
	post_code(0x39);

	post_code(0x40);
	printk(BIOS_DEBUG, "agesawrapper_amdinitpost ");
	val = agesawrapper_amdinitpost();
	if (val)
		printk(BIOS_DEBUG, "error level: %x \n", val);
	else
		printk(BIOS_DEBUG, "passed.\n");

	post_code(0x41);
	printk(BIOS_DEBUG, "agesawrapper_amdinitenv ");
	val = agesawrapper_amdinitenv();
	if (val)
		printk(BIOS_DEBUG, "error level: %x \n", val);
	else
		printk(BIOS_DEBUG, "passed.\n"); 

	/* Initialize i8259 pic */
	post_code(0x41);
	setup_i8259();

	/* Initialize i8254 timers */
	post_code(0x42);
	setup_i8254();  

    rs780_before_pci_init();

	post_code(0x50);
	print_debug("Disabling cache as ram ");
	disable_cache_as_ram();
	print_debug("done\n");

	post_code(0x51);
	copy_and_run();

	/* We will not return,  Should never see this message and post code. */
	print_debug("should not be here -\n");
	post_code(0x54);  
}

