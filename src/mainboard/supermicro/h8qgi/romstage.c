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

#include <lib.h>
#include <reset.h>
#include <stdint.h>
#include <arch/io.h>
#include <arch/romcc_io.h>
#include <arch/cpu.h>
#include <console/console.h>
#include <arch/stages.h>
#include "cpu/x86/bist.h"
#include "cpu/x86/lapic/boot_cpu.c"
#include "agesawrapper.h"
#include "northbridge/amd/agesa/family10/reset_test.h"
#include "southbridge/amd/sr5650/sr5650.h"
#include "southbridge/amd/sb700/sb700.h"
#include "superio/nuvoton/wpcm450/wpcm450.h"

extern void disable_cache_as_ram(void); /* cache_as_ram.inc */

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	agesawrapper_amdinitmmio();
	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);
		/* SR56x0 pcie bridges block pci_locate_device() before pcie training.
		 * disable all pcie bridges on SR56x0 to work around it
		 */
		sr5650_disable_pcie_bridge();
		post_code(0x31);
		sb7xx_51xx_lpc_port80();
		post_code(0x32);
	}

	/* Halt if there was a built in self test failure */
	post_code(0x33);
	report_bist_failure(bist);

	enable_sr5650_dev8();
	sb7xx_51xx_lpc_init();
	sb7xx_51xx_enable_wideio(0, 0x1600); /* though UARTs are on the NUVOTON BMC */
	wpcm450_enable_dev(WPCM450_SP1, CONFIG_SIO_PORT, CONFIG_TTYS0_BASE);
	sb7xx_51xx_disable_wideio(0);
	post_code(0x34);

	uart_init();
	post_code(0x35);
	console_init();

	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

	post_code(0x37);
	val = agesawrapper_amdinitreset();
	if (val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitreset failed: %x \n", val);
	} else {
		printk(BIOS_DEBUG, "agesawrapper_amdinitreset passed\n");
	}

	post_code(0x38);
	val = agesawrapper_amdinitearly();
	if(val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitearly failed: %x \n", val);
	} else {
		printk(BIOS_DEBUG, "agesawrapper_amdinitearly passed\n");
	}

	sr5650_early_setup();
	post_code(0x39);

	sb7xx_51xx_early_setup();
	sr5650_htinit();
	/* Reset for HT, FIDVID, PLL and errata changes to take affect. */
	if (!warm_reset_detect(0)) {
		print_info("...WARM RESET...\n\n\n");
		distinguish_cpu_resets(0);
		soft_reset();
		die("After soft_reset_x - shouldn't see this message!!!\n");
	}

	post_code(0x40);
	val = agesawrapper_amdinitpost();
	if (val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitpost failed: %x \n", val);
	}
	printk(BIOS_DEBUG, "agesawrapper_amdinitpost passed\n");

	post_code(0x41);
	val = agesawrapper_amdinitenv();
	if(val) {
		printk(BIOS_DEBUG, "agesawrapper_amdinitenv failed: %x \n", val);
	}
	printk(BIOS_DEBUG, "agesawrapper_amdinitenv passed\n");

	post_code(0x42);
	sr5650_before_pci_init();
	sb7xx_51xx_before_pci_init();

	post_code(0x50);
	print_debug("Disabling cache as ram ");
	disable_cache_as_ram();
	print_debug("done\n");

	post_code(0x51);
	copy_and_run(0);

	/* We will not return,  Should never see this message and post code. */
	print_debug("should not be here -\n");
	post_code(0x54);
}

