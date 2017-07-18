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
 */

#include <stdint.h>
#include <string.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <commonlib/loglevel.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/pi/agesawrapper_call.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include <Fch/Fch.h>
#include "gpio_ftns.h"
#include <build.h>

static void early_lpc_init(void);

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/*
	 *  In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 *  LpcClk[1:0]".  This following register setting has been
	 *  replicated in every reference design since Parmer, so it is
	 *  believed to be required even though it is not documented in
	 *  the SoC BKDGs.  Without this setting, there is no serial
	 *  output.
	 */
	outb(0xD2, 0xcd6);
	outb(0x00, 0xcd7);

	amd_initmmio();

	hudson_lpc_port80();

	if (!cpu_init_detectedx && boot_cpu()) {
		u32 data, *memptr;

		post_code(0x30);
		early_lpc_init();

		hudson_clk_output_48Mhz();
		post_code(0x31);
		console_init();


		printk(BIOS_INFO, "14-25-48Mhz Clock settings\n");

		memptr = (u32 *)(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG28 );
		data = *memptr;
		printk(BIOS_INFO, "FCH_MISC_REG28 is 0x%08x \n", data);

		memptr = (u32 *)(ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG40 );
		data = *memptr;
		printk(BIOS_INFO, "FCH_MISC_REG40 is 0x%08x \n", data);

		// sign of life strings
		printk(BIOS_ALERT, CONFIG_MAINBOARD_PART_NUMBER "\n");
		printk(BIOS_ALERT, "coreboot build %s\n", COREBOOT_DMI_DATE);
		printk(BIOS_ALERT, "BIOS version %s\n", COREBOOT_ORIGIN_GIT_TAG);

		//
		// Configure clock request
		//
		data = *((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG00));

		data &= 0xFFFF0000;
		data |= (0 + 1) << (0 * 4);	// CLKREQ 0 to CLK0
		data |= (1 + 1) << (1 * 4);	// CLKREQ 1 to CLK1
		data |= (2 + 1) << (2 * 4);	// CLKREQ 2 to CLK2
		// make CLK3 to ignore CLKREQ# input
		// force it to be always on
		data |= ( 0xf ) << (3 * 4);	// CLKREQ 3 to CLK3

		*((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG00)) = data;

		data = *((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG04));

		data &= 0xFFFFFF0F;
		data |= 0xA << (1 * 4);	// CLKREQ GFX to GFXCLK

		*((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG04)) = data;
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x \n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx \n", cpu_init_detectedx);

	post_code(0x37);
	AGESAWRAPPER(amdinitreset);

	post_code(0x38);
	printk(BIOS_DEBUG, "Got past avalon_early_setup\n");

	post_code(0x39);
	AGESAWRAPPER(amdinitearly);

	post_code(0x40);
	AGESAWRAPPER(amdinitpost);

	//PspMboxBiosCmdDramInfo();
	post_code(0x41);
	AGESAWRAPPER(amdinitenv);
	/*
	  If code hangs here, please check cahaltasm.S
	*/
	disable_cache_as_ram();

	outb(0xEA, 0xCD6);
	outb(0x1, 0xcd7);

	post_code(0x50);
	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}


static void early_lpc_init(void)
{
	u32 setting = 0x0;

	//
	// Configure output disabled, value low, pull up/down disabled
	//
	configure_gpio(IOMUX_GPIO_32, Function0, GPIO_32, setting);
	configure_gpio(IOMUX_GPIO_49, Function2, GPIO_49, setting);
	configure_gpio(IOMUX_GPIO_50, Function2, GPIO_50, setting);
	configure_gpio(IOMUX_GPIO_71, Function0, GPIO_71, setting);
	//
	// Configure output enabled, value low, pull up/down disabled
	//
	setting = GPIO_OUTPUT_ENABLE;
	configure_gpio(IOMUX_GPIO_33, Function0, GPIO_33, setting);
	configure_gpio(IOMUX_GPIO_57, Function1, GPIO_57, setting);
	configure_gpio(IOMUX_GPIO_58, Function1, GPIO_58, setting);
	configure_gpio(IOMUX_GPIO_59, Function3, GPIO_59, setting);
	//
	// Configure output enabled, value high, pull up/down disabled
	//
	setting = GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE;
	configure_gpio(IOMUX_GPIO_51, Function2, GPIO_51, setting);
	configure_gpio(IOMUX_GPIO_55, Function3, GPIO_55, setting);
	configure_gpio(IOMUX_GPIO_64, Function2, GPIO_64, setting);
	configure_gpio(IOMUX_GPIO_68, Function0, GPIO_68, setting);
}
