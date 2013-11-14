/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
 * Copyright (C) 2013 Vladimir Serbinenko <phcoder@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <console/console.h>
#include <device/device.h>
#include <arch/io.h>
#include <delay.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/pci_ids.h>
#include <arch/io.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include <northbridge/intel/nehalem/nehalem.h>
#include <southbridge/intel/bd82x6x/pch.h>

#include <pc80/mc146818rtc.h>
#include "dock.h"
#include <arch/x86/include/arch/acpigen.h>
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
#include <x86emu/regs.h>
#include <arch/interrupt.h>
#endif
#include <pc80/keyboard.h>
#include <cpu/x86/lapic.h>
#include <device/pci.h>
#include <smbios.h>

static acpi_cstate_t cst_entries[] = {
	{1, 1, 1000, {0x7f, 1, 2, {0}, 1, 0}},
	{2, 1, 500, {0x01, 8, 0, {0}, DEFAULT_PMBASE + LV2, 0}},
	{2, 17, 250, {0x01, 8, 0, {0}, DEFAULT_PMBASE + LV3, 0}},
};

int get_cst_entries(acpi_cstate_t ** entries)
{
	*entries = cst_entries;
	return ARRAY_SIZE(cst_entries);
}

#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE

static int int15_handler(void)
{
	switch ((X86_EAX & 0xffff)) {
		/* Get boot display.  */
	case 0x5f35:
		X86_EAX = 0x5f;
#if 0
		/* DisplayPort */
		X86_ECX = 0x4;
		/* VGA */
		X86_ECX = 0x1;
#endif
		/* LCD */
		X86_ECX = 0x8;

		return 1;
	case 0x5f40:
		X86_EAX = 0x5f;
		X86_ECX = 0x2;
		return 1;
	default:
		printk(BIOS_WARNING, "Unknown INT15 function %04x!\n",
		       X86_EAX & 0xffff);
		return 0;
	}
}
#endif

static void rcba_config(void)
{
#if 0
#if 0
	u32 reg32;

	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  WLAN   INTA -> PIRQB
	 * D28IP_P4IP  ETH0   INTB -> PIRQC (MSI)
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQB
	 * D31IP_SIP   SATA   INTA -> PIRQA (MSI)
	 * D31IP_SMIP  SMBUS  INTC -> PIRQH
	 * D31IP_TTIP  THRT   INTB -> PIRQG
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 *
	 * LIGHTSENSOR             -> PIRQE (Edge Triggered)
	 * TRACKPAD                -> PIRQF (Edge Triggered)
	 */
	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTD << D31IP_TTIP) | (INTC << D31IP_SIP2) |
	    (INTD << D31IP_UNKIP) | (INTA << D31IP_SMIP) | (INTB << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = 0x40043214;	//(INTA << D29IP_E1P);
	RCBA32(D28IP) = 0x00014321;	//(INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
	//              (INTB << D28IP_P4IP);
	RCBA32(D27IP) = 0x00000002;	//(INTA << D27IP_ZIP);
	RCBA32(D26IP) = 0x30003214;	//(INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);
#endif
	RCBA32(0x30fc) = 0x00000000;
	(void)RCBA32(0x30fc);
	RCBA32(0x3100) = 0x04341200;
	(void)RCBA32(0x3100);
	RCBA32(0x3104) = 0x00000000;
	(void)RCBA32(0x3104);
	RCBA32(0x3108) = 0x40043214;
	(void)RCBA32(0x3108);
	RCBA32(0x310c) = 0x00014321;
	(void)RCBA32(0x310c);
	RCBA32(0x3110) = 0x00000002;
	(void)RCBA32(0x3110);
	RCBA32(0x3114) = 0x30003214;
	(void)RCBA32(0x3114);
	RCBA32(0x311c) = 0x00000002;
	(void)RCBA32(0x311c);
	RCBA32(0x3120) = 0x00000000;
	(void)RCBA32(0x3120);
	RCBA32(0x3124) = 0x00002321;
	(void)RCBA32(0x3124);
	RCBA32(0x313c) = 0x00000000;
	(void)RCBA32(0x313c);
	RCBA32(0x3140) = 0x00003107;
	(void)RCBA32(0x3140);
	RCBA32(0x3144) = 0x76543210;
	(void)RCBA32(0x3144);
	RCBA32(0x3148) = 0x00000010;
	(void)RCBA32(0x3148);
	RCBA32(0x314c) = 0x00007654;
	(void)RCBA32(0x314c);
	RCBA32(0x3150) = 0x00000004;
	(void)RCBA32(0x3150);
	RCBA32(0x3158) = 0x00000000;
	(void)RCBA32(0x3158);
	RCBA32(0x315c) = 0x00003210;
	(void)RCBA32(0x315c);
	RCBA32(0x31fc) = 0x03000000;
	(void)RCBA32(0x31fc);

#if 0
	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQA, PIRQG, PIRQH, PIRQB);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQG, PIRQH);
	DIR_ROUTE(D28IR, PIRQB, PIRQC, PIRQD, PIRQE);
	DIR_ROUTE(D27IR, PIRQG, PIRQH, PIRQA, PIRQB);
	DIR_ROUTE(D26IR, PIRQB, PIRQC, PIRQD, PIRQA);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);
	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void)RCBA16(OIC);
	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_ALWAYS;
	RCBA32(FD) = reg32;
#endif

#if 0
	RCBA32(0x3418) = 0x16e61fe1;
	(void)RCBA32(0x3418);
#endif
#endif
}

const char *smbios_mainboard_version(void)
{
	return "Lenovo X201";
}

static void mainboard_enable(device_t dev)
{
	device_t dev0;
	u8 defaults_loaded = 0;
	u16 pmbase;

	printk(BIOS_SPEW, "starting SPI configure\n");

	/* Configure SPI.  */
	RCBA32(0x3800) = 0x07ff0500;
	RCBA32(0x3804) = 0x3f046008;
	RCBA32(0x3808) = 0x0058efc0;
	RCBA32(0x384c) = 0x92000000;
	RCBA32(0x3850) = 0x00000a0b;
	RCBA32(0x3858) = 0x07ff0500;
	RCBA32(0x385c) = 0x04ff0003;
	RCBA32(0x3860) = 0x00020001;
	RCBA32(0x3864) = 0x00000fff;
	RCBA32(0x3874) = 0;
	RCBA32(0x3890) = 0xf8400000;
	RCBA32(0x3894) = 0x143b5006;
	RCBA32(0x3898) = 0x05200302;
	RCBA32(0x389c) = 0x0601209f;
	RCBA32(0x38b0) = 0x00000004;
	RCBA32(0x38b4) = 0x03040002;
	RCBA32(0x38c0) = 0x00000007;
	RCBA32(0x38c4) = 0x00802005;
	RCBA32(0x38c8) = 0x00002005;
	RCBA32(0x3804) = 0x3f04e008;

	printk(BIOS_SPEW, "SPI configured\n");

	pmbase = pci_read_config32(dev_find_slot(0, PCI_DEVFN(0x1f, 0)),
				   PMBASE) & 0xff80;

	printk(BIOS_SPEW, " ... pmbase = 0x%04x\n", pmbase);

	outl(0, pmbase + SMI_EN);

	enable_lapic();
	pci_write_config32(dev_find_slot(0, PCI_DEVFN(0x1f, 0)), GPIO_BASE,
			   DEFAULT_GPIOBASE | 1);
	pci_write_config8(dev_find_slot(0, PCI_DEVFN(0x1f, 0)), GPIO_CNTL,
			  0x10);

	rcba_config();

#ifdef DISABLED
	ec_clr_bit(0x03, 2);

	if (inb(0x164c) & 0x08) {
		ec_set_bit(0x03, 2);
		ec_write(0x0c, 0x88);
	}
#endif
	/* If we're resuming from suspend, blink suspend LED */
	dev0 = dev_find_slot(0, PCI_DEVFN(0, 0));
	if (dev0 && pci_read_config32(dev0, SKPAD) == SKPAD_ACPI_S3_MAGIC)
		ec_write(0x0c, 0xc7);

#ifdef DISABLED
	idedev = dev_find_slot(0, PCI_DEVFN(0x1f, 1));
	if (idedev && idedev->chip_info && dock_ultrabay_device_present()) {
		struct southbridge_intel_i82801gx_config *config =
		    idedev->chip_info;
		config->ide_enable_primary = 1;
		/* enable Ultrabay power */
		outb(inb(0x1628) | 0x01, 0x1628);
		ec_write(0x0c, 0x84);
	} else {
		/* disable Ultrabay power */
		outb(inb(0x1628) & ~0x01, 0x1628);
		ec_write(0x0c, 0x04);
	}
#endif

	if (get_option(&defaults_loaded, "cmos_defaults_loaded") < 0) {
		printk(BIOS_INFO, "failed to get cmos_defaults_loaded");
		defaults_loaded = 0;
	}

	if (!defaults_loaded) {
		printk(BIOS_INFO, "Restoring CMOS defaults\n");
		set_option("tft_brightness", &(u8[]) {
			   0xff}
		);
		set_option("volume", &(u8[]) {
			   0x03}
		);
		/* set baudrate to 115200 baud */
		set_option("baud_rate", &(u8[]) {
			   0x00}
		);
		/* set default debug_level (DEFAULT_CONSOLE_LOGLEVEL starts at 1) */
		set_option("debug_level", &(u8[]) {
			   CONFIG_DEFAULT_CONSOLE_LOGLEVEL + 1}
		);
		set_option("cmos_defaults_loaded", &(u8[]) {
			   0x01}
		);
	}
#if CONFIG_PCI_OPTION_ROM_RUN_YABEL || CONFIG_PCI_OPTION_ROM_RUN_REALMODE
	/* Install custom int15 handler for VGA OPROM */
	mainboard_interrupt_handlers(0x15, &int15_handler);
#endif

	/* This sneaked in here, because X201 SuperIO chip isn't really
	   connected to anything and hence we don't init it.
	 */
	pc_keyboard_init(0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
