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
#include <southbridge/amd/pi/hudson/hudson.h>
#include <Fch/Fch.h>
#include <security/tpm/tspi.h>

#include "gpio_ftns.h"

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

	hudson_lpc_port80();

	if (!cpu_init_detectedx && boot_cpu()) {
		timestamp_init(timestamp_get());
		timestamp_add_now(TS_START_ROMSTAGE);

		post_code(0x30);
		early_lpc_init();

		hudson_clk_output_48Mhz();
		post_code(0x31);
		console_init();
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
	if (IS_ENABLED(CONFIG_BOARD_PCENGINES_APU2) ||
		IS_ENABLED(CONFIG_BOARD_PCENGINES_APU3) ||
		IS_ENABLED(CONFIG_BOARD_PCENGINES_APU4)) {
		configure_gpio(ACPI_MMIO_BASE,
			IOMUX_GPIO_32, Function0, GPIO_32, setting);
	}

	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_49, Function2, GPIO_49, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_50, Function2, GPIO_50, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_71, Function0, GPIO_71, setting);
	//
	// Configure output enabled, value low, pull up/down disabled
	//
	setting = 0x1 << GPIO_OUTPUT_ENABLE;
	if (IS_ENABLED(CONFIG_BOARD_PCENGINES_APU3) ||
		IS_ENABLED(CONFIG_BOARD_PCENGINES_APU4)) {
		configure_gpio(ACPI_MMIO_BASE,
			IOMUX_GPIO_33, Function0, GPIO_33, setting);
	}

	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_57, Function1, GPIO_57, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_58, Function1, GPIO_58, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_59, Function3, GPIO_59, setting);
	//
	// Configure output enabled, value high, pull up/down disabled
	//
	setting = 0x1 << GPIO_OUTPUT_ENABLE | 0x1 << GPIO_OUTPUT_VALUE;
	if (IS_ENABLED(CONFIG_BOARD_PCENGINES_APU5)) {
		configure_gpio(ACPI_MMIO_BASE,
			IOMUX_GPIO_32, Function0, GPIO_32, setting);
		configure_gpio(ACPI_MMIO_BASE,
			IOMUX_GPIO_33, Function0, GPIO_33, setting);
	}

	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_51, Function2, GPIO_51, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_55, Function3, GPIO_55, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_64, Function2, GPIO_64, setting);
	configure_gpio(ACPI_MMIO_BASE, IOMUX_GPIO_68, Function0, GPIO_68, setting);
}
