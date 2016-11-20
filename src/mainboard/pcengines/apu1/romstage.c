/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Advanced Micro Devices, Inc.
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC
 * Copyright (C) 2014 Kyösti Mälkki <kyosti.malkki@gmail.com>
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
#include <cpu/x86/mtrr.h>
#include <cpu/amd/car.h>
#include <northbridge/amd/agesa/agesawrapper.h>
#include <southbridge/amd/cimx/cimx_util.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/cache.h>
#include <sb_cimx.h>
#include "SBPLATFORM.h"
#include "cbmem.h"
#include <cpu/amd/mtrr.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct5104d/nct5104d.h>
#include "gpio_ftns.h"

#define SIO_PORT 0x2e
#define SERIAL_DEV PNP_DEV(SIO_PORT, NCT5104D_SP1)

static void early_lpc_init(void);

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;

	/* Must come first to enable PCI MMCONF. */
	amd_initmmio();

	if (!cpu_init_detectedx && boot_cpu()) {
		post_code(0x30);
		sb_Poweron_Init();
		early_lpc_init();


		post_code(0x31);
		nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
		console_init();
	}

	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);

	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	post_code(0x37);
	agesawrapper_amdinitreset();

	post_code(0x39);
	agesawrapper_amdinitearly();

	int s3resume = acpi_is_wakeup_s3();
	if (!s3resume) {
		post_code(0x40);
		agesawrapper_amdinitpost();

		post_code(0x42);
		agesawrapper_amdinitenv();
		amd_initenv();

	} else { 			/* S3 detect */
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		agesawrapper_amdinitresume();

		agesawrapper_amds3laterestore();

		post_code(0x61);
		prepare_for_resume();
	}

	post_code(0x50);
	copy_and_run();
	printk(BIOS_ERR, "Error: copy_and_run() returned!\n");

	post_code(0x54);	/* Should never see this post code. */
}

static void early_lpc_init(void)
{
	u32 mmio_base;

	/* PC Engines requires system boot when power is applied. This feature is
	 * controlled in PM_REG 5Bh register. "Always Power On" works by writing a
	 * value of 05h.
	 */
	u8 bdata = pm_ioread(SB_PMIOA_REG5B);
	bdata &= 0xf8; //clear bits 0-2
	bdata |= 0x05; //set bits 0,2
	pm_iowrite(SB_PMIOA_REG5B, bdata);

	/* Multi-function pins switch to GPIO0-35, these pins are shared with PCI pins */
	bdata = pm_ioread(SB_PMIOA_REGEA);
	bdata &= 0xfe; //clear bit 0
	bdata |= 0x01; //set bit 0
	pm_iowrite(SB_PMIOA_REGEA, bdata);

	//configure required GPIOs
	mmio_base = find_gpio_base();
	configure_gpio(mmio_base, GPIO_10,  GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(mmio_base, GPIO_11,  GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_HIGH);
	configure_gpio(mmio_base, GPIO_15,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_16,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_17,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_18,  GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_187, GPIO_FTN_1, GPIO_INPUT);
	configure_gpio(mmio_base, GPIO_189, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(mmio_base, GPIO_190, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
	configure_gpio(mmio_base, GPIO_191, GPIO_FTN_1, GPIO_OUTPUT | GPIO_DATA_LOW);
}
