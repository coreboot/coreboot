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
#include <arch/io.h>
#include <arch/stages.h>
#include <device/pnp_def.h>
#include <arch/cpu.h>
#include <cpu/x86/lapic.h>
#include <console/console.h>
#include <commonlib/loglevel.h>
#include <timestamp.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/agesa/state_machine.h>
#include <northbridge/amd/pi/agesawrapper.h>
#include <northbridge/amd/pi/agesawrapper_call.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <cpu/amd/microcode.h>
#include <southbridge/amd/pi/hudson/hudson.h>
#include <Fch/Fch.h>
#include <security/tpm/tspi.h>

#include "gpio_ftns.h"
#include <build.h>
#include "bios_knobs.h"

static void early_lpc_init(void);
static void print_sign_of_life(void);
extern char coreboot_dmi_date[];
extern char coreboot_version[];

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

	hudson_lpc_port80();

	if (!cpu_init_detectedx && boot_cpu()) {
		u32 data, *memptr;
		timestamp_init(timestamp_get());
		timestamp_add_now(TS_START_ROMSTAGE);

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

		bool scon = check_console();

		if(scon) {
			print_sign_of_life();
		}
		//
		// Configure clock request
		//
		data = *((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG00));

		data &= 0xFFFF0000;
		data |= (0 + 1) << (0 * 4);	// CLKREQ 0 to CLK0
		data |= (1 + 1) << (1 * 4);	// CLKREQ 1 to CLK1
#if IS_ENABLED(CONFIG_BOARD_PCENGINES_APU2) || IS_ENABLED(CONFIG_BOARD_PCENGINES_APU3) || IS_ENABLED(CONFIG_BOARD_PCENGINES_APU4)
		data |= (2 + 1) << (2 * 4);	// CLKREQ 2 to CLK2 disabled on APU5
#endif
		// make CLK3 to ignore CLKREQ# input
		// force it to be always on
		data |= ( 0xf ) << (3 * 4);	// CLKREQ 3 to CLK3

		*((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG00)) = data;

		data = *((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG04));

		data &= 0xFFFFFF0F;
#if IS_ENABLED(CONFIG_FORCE_MPCIE2_CLK)
		// make GFXCLK to ignore CLKREQ# input
		// force it to be always on
		data |= 0xF << (1 * 4); // CLKREQ GFX to GFXCLK
#else
		bool mpcie2_clk = check_mpcie2_clk();
		if (mpcie2_clk) {
			// make GFXCLK to ignore CLKREQ# input
			// force it to be always on
			data |= 0xF << (1 * 4); // CLKREQ GFX to GFXCLK
			printk(BIOS_DEBUG, "mPCIe clock enabled\n");
		}
		else {
			data |= 0xA << (1 * 4);	// CLKREQ GFX to GFXCLK
			printk(BIOS_DEBUG, "mPCIe clock disabled\n");
		}
#endif

		*((u32 *)(ACPI_MMIO_BASE + MISC_BASE+FCH_MISC_REG04)) = data;
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	update_microcode(val);

	post_code(0x37);
	AGESAWRAPPER(amdinitreset);

	post_code(0x38);
	printk(BIOS_DEBUG, "Got past avalon_early_setup\n");

	post_code(0x39);
	AGESAWRAPPER(amdinitearly);

	timestamp_add_now(TS_BEFORE_INITRAM);

	post_code(0x40);
	AGESAWRAPPER(amdinitpost);

	/* FIXME: Detect if TSC frequency changed during raminit? */
	timestamp_rescale_table(1, 4);

	timestamp_add_now(TS_AFTER_INITRAM);
}

void agesa_postcar(struct sysinfo *cb)
{
	//PspMboxBiosCmdDramInfo();
	post_code(0x41);
	AGESAWRAPPER(amdinitenv);

	if (IS_ENABLED(CONFIG_TPM1) || IS_ENABLED(CONFIG_TPM2))
		tpm_setup(false);

	outb(0xEA, 0xCD6);
	outb(0x1, 0xcd7);
}

static void early_lpc_init(void)
{
	u32 setting = 0x0;

	//
	// Configure output disabled, value low, pull up/down disabled
	//
#if IS_ENABLED(CONFIG_BOARD_PCENGINES_APU5)
	configure_gpio(IOMUX_GPIO_22, Function0, GPIO_22, setting);
#endif

#if IS_ENABLED(CONFIG_BOARD_PCENGINES_APU2) || IS_ENABLED(CONFIG_BOARD_PCENGINES_APU3) || IS_ENABLED(CONFIG_BOARD_PCENGINES_APU4)
	configure_gpio(IOMUX_GPIO_32, Function0, GPIO_32, setting);
#endif
	configure_gpio(IOMUX_GPIO_49, Function2, GPIO_49, setting);
	configure_gpio(IOMUX_GPIO_50, Function2, GPIO_50, setting);
	configure_gpio(IOMUX_GPIO_71, Function0, GPIO_71, setting);
	//
	// Configure output enabled, value low, pull up/down disabled
	//
	setting = GPIO_OUTPUT_ENABLE;
#if IS_ENABLED(CONFIG_BOARD_PCENGINES_APU3) || IS_ENABLED(CONFIG_BOARD_PCENGINES_APU4)
	configure_gpio(IOMUX_GPIO_33, Function0, GPIO_33, setting);
#endif
	configure_gpio(IOMUX_GPIO_57, Function1, GPIO_57, setting);
	configure_gpio(IOMUX_GPIO_58, Function1, GPIO_58, setting);
	configure_gpio(IOMUX_GPIO_59, Function3, GPIO_59, setting);
	//
	// Configure output enabled, value high, pull up/down disabled
	//
	setting = GPIO_OUTPUT_ENABLE | GPIO_OUTPUT_VALUE;
#if IS_ENABLED(CONFIG_BOARD_PCENGINES_APU5)
	configure_gpio(IOMUX_GPIO_32, Function0, GPIO_32, setting);
	configure_gpio(IOMUX_GPIO_33, Function0, GPIO_33, setting);
#endif
	configure_gpio(IOMUX_GPIO_51, Function2, GPIO_51, setting);
	configure_gpio(IOMUX_GPIO_55, Function3, GPIO_55, setting);
	configure_gpio(IOMUX_GPIO_64, Function2, GPIO_64, setting);
	configure_gpio(IOMUX_GPIO_68, Function0, GPIO_68, setting);
}

static const char *mainboard_bios_version(void)
{
	if (strlen(CONFIG_LOCALVERSION))
		return CONFIG_LOCALVERSION;
	else
		return coreboot_version;
}

static void print_sign_of_life()
{
	char tmp[9];
	strncpy(tmp,   coreboot_dmi_date+6, 4);
	strncpy(tmp+4, coreboot_dmi_date+3, 2);
	strncpy(tmp+6, coreboot_dmi_date,   2);
	tmp[8] = '\0';
	printk(BIOS_ALERT, CONFIG_MAINBOARD_VENDOR " "
	                   CONFIG_MAINBOARD_PART_NUMBER "\n");
	printk(BIOS_ALERT, "coreboot build %s\n", tmp);
	printk(BIOS_ALERT, "BIOS version %s\n", mainboard_bios_version());
}
