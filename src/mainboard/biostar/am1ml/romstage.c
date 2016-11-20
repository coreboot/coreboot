/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * Copyright (C) 2015 Sergej Ivanov <getinaks@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
#include <northbridge/amd/agesa/agesawrapper.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/lapic.h>
#include <southbridge/amd/agesa/hudson/hudson.h>
#include <cpu/amd/agesa/s3_resume.h>
#include "cbmem.h"
#include <superio/ite/common/ite.h>
#include <superio/ite/it8728f/it8728f.h>


#define ITE_CONFIG_REG_CC	0x02
#define SERIAL_DEV	PNP_DEV(0x2e, IT8728F_SP1)
#define GPIO_DEV	PNP_DEV(0x2e, IT8728F_GPIO)
#define ENVC_DEV	PNP_DEV(0x2e, IT8728F_EC)

#define MMIO_NON_POSTED_START	0xfed00000
#define MMIO_NON_POSTED_END	0xfedfffff
#define SB_MMIO	0xFED80000
#define SB_MMIO_MISC32(x)	*(volatile u32 *)(SB_MMIO + 0xE00 + (x))


static void it_sio_write(pnp_devfn_t dev, u8 reg, u8 value)
{
	pnp_set_logical_device(dev);
	pnp_write_config(dev, reg, value);
}

static void ite_enter_conf(pnp_devfn_t dev)
{
	u16 port = dev >> 8;

	outb(0x87, port);
	outb(0x01, port);
	outb(0x55, port);
	outb((port == 0x4e) ? 0xaa : 0x55, port);
}

static void ite_exit_conf(pnp_devfn_t dev)
{
	it_sio_write(dev, ITE_CONFIG_REG_CC, 0x02);
}

static void ite_evc_conf(pnp_devfn_t dev)
{
	ite_enter_conf(dev);
	it_sio_write(dev, 0xf1 , 0x40 );
	it_sio_write(dev, 0xf4 , 0x80 );
	it_sio_write(dev, 0xf5 , 0x00 );
	it_sio_write(dev, 0xf6 , 0xf0 );
	it_sio_write(dev, 0xf9 , 0x48 );
	it_sio_write(dev, 0xfa , 0x00 );
	it_sio_write(dev, 0xfb , 0x00 );
	ite_exit_conf(dev);
}

static void ite_gpio_conf(pnp_devfn_t dev)
{
	ite_enter_conf (dev);
	it_sio_write (dev, 0x25 , 0x80 );
	it_sio_write (dev, 0x26 , 0x07 );
	it_sio_write (dev, 0x28 , 0x81 );
	it_sio_write (dev, 0x2c , 0x06 );
	it_sio_write (dev, 0x72 , 0x00 );
	it_sio_write (dev, 0x73 , 0x00 );
	it_sio_write (dev, 0xb3 , 0x01 );
	it_sio_write (dev, 0xb8 , 0x00 );
	it_sio_write (dev, 0xc0 , 0x00 );
	it_sio_write (dev, 0xc3 , 0x00 );
	it_sio_write (dev, 0xc8 , 0x00 );
	it_sio_write (dev, 0xc9 , 0x07 );
	it_sio_write (dev, 0xcb , 0x01 );
	it_sio_write (dev, 0xf0 , 0x10 );
	it_sio_write (dev, 0xf4 , 0x27 );
	it_sio_write (dev, 0xf8 , 0x20 );
	it_sio_write (dev, 0xf9 , 0x01 );
	ite_exit_conf (dev);
}


void cache_as_ram_main(unsigned long bist, unsigned long cpu_init_detectedx)
{
	u32 val, t32;
	u8 byte;
	pci_devfn_t dev;
	u32 *addr32;

	/* Must come first to enable PCI MMCONF. */
	amd_initmmio();

	/* In Hudson RRG, PMIOxD2[5:4] is "Drive strength control for
	 * LpcClk[1:0]".  To be consistent with Parmer, setting to 4mA
	 * even though the register is not documented in the Kabini BKDG.
	 * Otherwise the serial output is bad code.
	 */
	outb(0xD2, 0xcd6);
	outb(0x00, 0xcd7);

	/* Set LPC decode enables. */
	pci_devfn_t dev2 = PCI_DEV(0, 0x14, 3);
	pci_write_config32(dev2, 0x44, 0xff03ffd5);

	hudson_lpc_port80();

	/* Enable the AcpiMmio space */
	outb(0x24, 0xcd6);
	outb(0x1, 0xcd7);

	/* Set auxiliary output clock frequency on OSCOUT1 pin to be 48MHz */
	addr32 = (u32 *)0xfed80e28;
	t32 = *addr32;
	t32 &= 0xfff8ffff;
	*addr32 = t32;

	/* Enable Auxiliary Clock1, disable FCH 14 MHz OscClk */
	addr32 = (u32 *)0xfed80e40;
	t32 = *addr32;
	t32 &= 0xffffbffb;
	*addr32 = t32;

	if (!cpu_init_detectedx && boot_cpu()) {
		/* enable SIO LPC decode */
		dev = PCI_DEV(0, 0x14, 3);
		byte = pci_read_config8(dev, 0x48);
		byte |= 3;	/* 2e, 2f */
		pci_write_config8(dev, 0x48, byte);

		/* enable serial decode */
		byte = pci_read_config8(dev, 0x44);
		byte |= (1 << 6);  /* 0x3f8 */
		pci_write_config8(dev, 0x44, byte);
		post_code(0x30);
		post_code(0x31);

		/* run ite */
		ite_kill_watchdog(GPIO_DEV);
		ite_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

		console_init();
	}
	printk(BIOS_DEBUG, "Console inited!\n");
	/* Halt if there was a built in self test failure */
	post_code(0x34);
	report_bist_failure(bist);


	/* Load MPB */
	val = cpuid_eax(1);
	printk(BIOS_DEBUG, "BSP Family_Model: %08x\n", val);
	printk(BIOS_DEBUG, "cpu_init_detectedx = %08lx\n", cpu_init_detectedx);

	/* On Larne, after LpcClkDrvSth is set, it needs some time to be stable, because of the buffer ICS551M */
	int i;
	for(i = 0; i < 200000; i++)
		val = inb(0xcd6);

	post_code(0x37);
	agesawrapper_amdinitreset();
	post_code(0x38);
	printk(BIOS_DEBUG, "Got past yangtze_early_setup\n");

	post_code(0x39);

	agesawrapper_amdinitearly();
	int s3resume = acpi_is_wakeup_s3();
	if (!s3resume) {
		post_code(0x40);
		agesawrapper_amdinitpost();
		post_code(0x41);
		agesawrapper_amdinitenv();
		/* TODO: Disable cache is not ok. */
		disable_cache_as_ram();
	} else { /* S3 detect */
		printk(BIOS_INFO, "S3 detected\n");

		post_code(0x60);
		agesawrapper_amdinitresume();

		amd_initcpuio();
		agesawrapper_amds3laterestore();

		post_code(0x61);
		prepare_for_resume();
	}

	outb(0xEA, 0xCD6);
	outb(0x1, 0xcd7);

	post_code(0x50);
	/* This functions configure SIO as it been done under vendor bios */
	printk(BIOS_DEBUG, "ITE CONFIG ENVC\n");
	ite_evc_conf(ENVC_DEV);
	printk(BIOS_DEBUG, "ITE CONFIG GPIO\n");
	ite_gpio_conf(GPIO_DEV);
	printk(BIOS_DEBUG, "ITE CONFIG DONE\n");

	copy_and_run();

	post_code(0x54);  /* Should never see this post code. */
}
