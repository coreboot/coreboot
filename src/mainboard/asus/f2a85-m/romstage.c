/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
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

#include <northbridge/amd/agesa/agesawrapper.h>

#include <arch/acpi.h>
#include <arch/cpu.h>
#include <arch/io.h>
#include <arch/stages.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/agesa/s3_resume.h>
#include <cpu/amd/car.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pnp_def.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <southbridge/amd/agesa/hudson/smbus.h>
#include <stdint.h>
#include <string.h>
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>

#define MMIO_NON_POSTED_START 0xfed00000
#define MMIO_NON_POSTED_END   0xfedfffff
#define SB_MMIO 0xFED80000
#define SB_MMIO_MISC32(x) *(volatile u32 *)(SB_MMIO + 0xE00 + (x))

#define SERIAL_DEV PNP_DEV(0x2e, IT8728F_SP1)
#define GPIO_DEV PNP_DEV(0x2e, IT8728F_GPIO)

static void sbxxx_enable_48mhzout(void)
{
	/* most likely programming to 48MHz out signal */
	u32 reg32;
	reg32 = SB_MMIO_MISC32(0x28);
	reg32 &= 0xffc7ffff;
	reg32 |= 0x00100000;
	SB_MMIO_MISC32(0x28) = reg32;

	reg32 = SB_MMIO_MISC32(0x40);
	reg32 &= ~0x80u;
	SB_MMIO_MISC32(0x40) = reg32;
}

void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val;
	u8 byte;
	pci_devfn_t dev;

	amd_initmmio();

#if IS_ENABLED(CONFIG_POST_DEVICE_PCI_PCIE)
	hudson_pci_port80();
#endif
#if IS_ENABLED(CONFIG_POST_DEVICE_LPC)
	hudson_lpc_port80();
#endif

	if (!cpu_init_detectedx && boot_cpu()) {

		/* enable SIO LPC decode */
		dev = PCI_DEV(0, 0x14, 3);
		byte = pci_read_config8(dev, 0x48);
		byte |= 3;		/* 2e, 2f */
		pci_write_config8(dev, 0x48, byte);

		/* enable serial decode */
		byte = pci_read_config8(dev, 0x44);
		byte |= (1 << 6);  /* 0x3f8 */
		pci_write_config8(dev, 0x44, byte);

		post_code(0x30);

                /* enable SB MMIO space */
		outb(0x24, 0xcd6);
		outb(0x1, 0xcd7);

		/* enable SIO clock */
		sbxxx_enable_48mhzout();
		ite_kill_watchdog(GPIO_DEV);
		ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
		ite_enable_3vsbsw(GPIO_DEV);
		console_init();

		/* turn on secondary smbus at b20 */
		outb(0x28, 0xcd6);
		byte = inb(0xcd7);
		byte |= 1;
		outb(byte, 0xcd7);

		/* set DDR3 voltage */
		byte = CONFIG_BOARD_ASUS_F2A85_M_DDR3_VOLT_VAL;

		/* default is byte = 0x0, so no need to set it in this case */
		if (byte)
			do_smbus_write_byte(0xb20, 0x15, 0x3, byte);
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
		post_code(0x41);
		agesawrapper_amdinitenv();
		disable_cache_as_ram();
	} else {		/* S3 detect */
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		agesawrapper_amdinitresume();
		amd_initcpuio();
		agesawrapper_amds3laterestore();

		post_code(0x61);
		prepare_for_resume();
	}

	post_code(0x50);
	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}
