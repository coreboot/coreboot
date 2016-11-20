/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2012 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2016 Renze Nicolai <renze@rnplus.nl>
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

#include <superio/fintek/common/fintek.h>
#include <superio/fintek/f71869ad/f71869ad.h>

#include <stdint.h>
#include <string.h>

#define MMIO_NON_POSTED_START 0xfed00000
#define MMIO_NON_POSTED_END   0xfedfffff
#define SB_MMIO 0xFED80000
#define SB_MMIO_MISC32(x) *(volatile u32 *)(SB_MMIO + 0xE00 + (x))

/* Ensure Super I/O config address (i.e., 0x2e or 0x4e) matches that of devicetree.cb */
#define SUPERIO_ADDRESS 0x4e

#define SERIAL_DEV PNP_DEV(SUPERIO_ADDRESS, F71869AD_SP1)
#define GPIO_DEV PNP_DEV(SUPERIO_ADDRESS, F71869AD_GPIO)


/* GPIO configuration */
#define FINTEK_ENTRY_KEY 0x87
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(FINTEK_ENTRY_KEY, port);
	outb(FINTEK_ENTRY_KEY, port);
}

#define FINTEK_EXIT_KEY 0xAA
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(FINTEK_EXIT_KEY, port);
}

static void gpio_init(pnp_devfn_t dev)
{
	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_write_config(dev, 0x60, 0x0a); //Base addr high
	pnp_write_config(dev, 0x61, 0x00); //Base addr low
	pnp_write_config(dev, 0xe0, 0x04); //GPIO1 output enable
	pnp_write_config(dev, 0xe1, 0xff); //GPIO1 output data
	pnp_write_config(dev, 0xe3, 0x04); //GPIO1 drive enable
	pnp_write_config(dev, 0xe4, 0x00); //GPIO1 PME enable
	pnp_write_config(dev, 0xe5, 0x00); //GPIO1 input detect select
	pnp_write_config(dev, 0xe6, 0x40); //GPIO1 event status
	pnp_write_config(dev, 0xd0, 0x00); //GPIO2 output enable
	pnp_write_config(dev, 0xd1, 0xff); //GPIO2 output data
	pnp_write_config(dev, 0xd3, 0x00); //GPIO2 drive enable
	pnp_write_config(dev, 0xc0, 0x00); //GPIO3 output enable
	pnp_write_config(dev, 0xc1, 0xff); //GPIO3 output data
	pnp_write_config(dev, 0xb0, 0x04); //GPIO4 output enable
	pnp_write_config(dev, 0xb1, 0x04); //GPIO4 output data
	pnp_write_config(dev, 0xb3, 0x04); //GPIO4 drive enable
	pnp_write_config(dev, 0xb4, 0x00); //GPIO4 PME enable
	pnp_write_config(dev, 0xb5, 0x00); //GPIO4 input detect select
	pnp_write_config(dev, 0xb6, 0x00); //GPIO4 event status
	pnp_write_config(dev, 0xa0, 0x00); //GPIO5 output enable
	pnp_write_config(dev, 0xa1, 0x1f); //GPIO5 output data
	pnp_write_config(dev, 0xa3, 0x00); //GPIO5 drive enable
	pnp_write_config(dev, 0xa4, 0x00); //GPIO5 PME enable
	pnp_write_config(dev, 0xa5, 0xff); //GPIO5 input detect select
	pnp_write_config(dev, 0xa6, 0xe0); //GPIO5 event status
	pnp_write_config(dev, 0x90, 0x00); //GPIO6 output enable
	pnp_write_config(dev, 0x91, 0xff); //GPIO6 output data
	pnp_write_config(dev, 0x93, 0x00); //GPIO6 drive enable
	pnp_write_config(dev, 0x80, 0x00); //GPIO7 output enable
	pnp_write_config(dev, 0x81, 0xff); //GPIO7 output data
	pnp_write_config(dev, 0x83, 0x00); //GPIO7 drive enable
	pnp_set_enable(dev, 1);
	pnp_exit_conf_state(dev);
}


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

	/* Must come first to enable PCI MMCONF. */
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

		/* Initialize GPIO registers */
		gpio_init(GPIO_DEV);

		/* Enable serial console */
		fintek_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
		console_init();

		/* turn on secondary smbus at b20 */
		outb(0x28, 0xcd6);
		byte = inb(0xcd7);
		byte |= 1;
		outb(byte, 0xcd7);
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
