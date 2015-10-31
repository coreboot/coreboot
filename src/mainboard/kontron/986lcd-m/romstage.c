/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
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

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include "option_table.h"
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <halt.h>
#include <superio/winbond/w83627thg/w83627thg.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627THG_SP1)

void setup_ich7_gpios(void)
{
	printk(BIOS_DEBUG, " GPIOS...");
	/* General Registers */
	outl(0x1f1ff7c0, DEFAULT_GPIOBASE + 0x00);	/* GPIO_USE_SEL */
	outl(0xe0e8efc3, DEFAULT_GPIOBASE + 0x04);	/* GP_IO_SEL */
	outl(0xebffeeff, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
	/* Output Control Registers */
	outl(0x00000000, DEFAULT_GPIOBASE + 0x18);	/* GPO_BLINK */
	/* Input Control Registers */
	outl(0x00002180, DEFAULT_GPIOBASE + 0x2c);	/* GPI_INV */
	outl(0x000100ff, DEFAULT_GPIOBASE + 0x30);	/* GPIO_USE_SEL2 */
	outl(0x00000030, DEFAULT_GPIOBASE + 0x34);	/* GP_IO_SEL2 */
	outl(0x00010035, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL */
}

static void ich7_enable_lpc(void)
{
	int lpt_en = 0;
	if (read_option(lpt, 0) != 0) {
		lpt_en = 1<<2; // enable LPT
	}
	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x64, 0xd0);
	// Set COM1/COM2 decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0010);
	// Enable COM1/COM2/KBD/SuperIO1+2
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x340b | lpt_en);
	// Enable HWM at 0xa00
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x00fc0a01);
	// COM3 decode
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x88, 0x000403e9);
	// COM4 decode
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x8c, 0x000402e9);
	// io 0x300 decode
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x90, 0x00000301);
}

/* TODO: superio code should really not be in mainboard */
static void pnp_enter_func_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0x87, port);
	outb(0x87, port);
}

static void pnp_exit_func_mode(device_t dev)
{
	u16 port = dev >> 8;
	outb(0xaa, port);
}

/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
static void early_superio_config_w83627thg(void)
{
	device_t dev;

	dev=PNP_DEV(0x2e, W83627THG_SP1);
	pnp_enter_func_mode(dev);

	pnp_write_config(dev, 0x24, 0xc6); // PNPCSV

	pnp_write_config(dev, 0x29, 0x43); // GPIO settings
	pnp_write_config(dev, 0x2a, 0x40); // GPIO settings

	dev=PNP_DEV(0x2e, W83627THG_SP1);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3f8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 4);
	pnp_set_enable(dev, 1);

	dev=PNP_DEV(0x2e, W83627THG_SP2);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x2f8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 3);
	// pnp_write_config(dev, 0xf1, 4); // IRMODE0
	pnp_set_enable(dev, 1);

	dev=PNP_DEV(0x2e, W83627THG_KBC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x60);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x64);
	// pnp_write_config(dev, 0xf0, 0x82);
	pnp_set_enable(dev, 1);

	dev=PNP_DEV(0x2e, W83627THG_GAME_MIDI_GPIO1);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_write_config(dev, 0xf5, 0xff); // invert all GPIOs
	pnp_set_enable(dev, 1);

	dev=PNP_DEV(0x2e, W83627THG_GPIO2);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1); // Just enable it

	dev=PNP_DEV(0x2e, W83627THG_GPIO3);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_write_config(dev, 0xf0, 0xfb); // GPIO bit 2 is output
	pnp_write_config(dev, 0xf1, 0x00); // GPIO bit 2 is 0
	pnp_write_config(dev, 0x30, 0x03); // Enable GPIO3+4. pnp_set_enable is not sufficient

	dev=PNP_DEV(0x2e, W83627THG_FDC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	dev=PNP_DEV(0x2e, W83627THG_PP);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	/* Enable HWM */
	dev=PNP_DEV(0x2e, W83627THG_HWM);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0xa00);
	pnp_set_enable(dev, 1);

	pnp_exit_func_mode(dev);

	dev=PNP_DEV(0x4e, W83627THG_SP1);
	pnp_enter_func_mode(dev);

	pnp_set_logical_device(dev); // Set COM3 to sane non-conflicting values
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3e8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 11);
	pnp_set_enable(dev, 1);

	dev=PNP_DEV(0x4e, W83627THG_SP2);
	pnp_set_logical_device(dev); // Set COM4 to sane non-conflicting values
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x2e8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 10);
	pnp_set_enable(dev, 1);

	dev=PNP_DEV(0x4e, W83627THG_FDC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	dev=PNP_DEV(0x4e, W83627THG_PP);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	dev=PNP_DEV(0x4e, W83627THG_KBC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x00);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x00);

	pnp_exit_func_mode(dev);
}

static void rcba_config(void)
{
	u32 reg32;

	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;
	//RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042210;
	/* Device 1d interrupt pin register */
	RCBA32(0x310c) = 0x00214321;

	/* dev irq route register */
	RCBA16(0x3140) = 0x0132;
	RCBA16(0x3142) = 0x3241;
	RCBA16(0x3144) = 0x0237;
	RCBA16(0x3146) = 0x3210;
	RCBA16(0x3148) = 0x3210;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;

	/* Now, this is a bit ugly. As per PCI specification, function 0 of a
	 * device always has to be implemented. So disabling ethernet port 1
	 * would essentially disable all three ethernet ports of the mainboard.
	 * It's possible to rename the ports to achieve compatibility to the
	 * PCI spec but this will confuse all (static!) tables containing
	 * interrupt routing information.
	 * To avoid this, we enable (unused) port 6 and swap it with port 1
	 * in the case that ethernet port 1 is disabled. Since no devices
	 * are connected to that port, we don't have to worry about interrupt
	 * routing.
	 */
	int port_shuffle = 0;

	/* Disable unused devices */
	reg32 = FD_ACMOD|FD_ACAUD|FD_PATA;
	reg32 |= FD_PCIE6|FD_PCIE5|FD_PCIE4;

	if (read_option(ethernet1, 0) != 0) {
		printk(BIOS_DEBUG, "Disabling ethernet adapter 1.\n");
		reg32 |= FD_PCIE1;
	}
	if (read_option(ethernet2, 0) != 0) {
		printk(BIOS_DEBUG, "Disabling ethernet adapter 2.\n");
		reg32 |= FD_PCIE2;
	} else {
		if (reg32 & FD_PCIE1)
			port_shuffle = 1;
	}
	if (read_option(ethernet3, 0) != 0) {
		printk(BIOS_DEBUG, "Disabling ethernet adapter 3.\n");
		reg32 |= FD_PCIE3;
	} else {
		if (reg32 & FD_PCIE1)
			port_shuffle = 1;
	}

	if (port_shuffle) {
		/* Enable PCIE6 again */
		reg32 &= ~FD_PCIE6;
		/* Swap PCIE6 and PCIE1 */
		RCBA32(RPFN) = 0x00043215;
	}

	reg32 |= 1;

	RCBA32(0x3418) = reg32;

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;
}

static void early_ich7_init(void)
{
	uint8_t reg8;
	uint32_t reg32;

	// program secondary mlt XXX byte?
	pci_write_config8(PCI_DEV(0, 0x1e, 0), 0x1b, 0x20);

	// reset rtc power status
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, reg8);

	// usb transient disconnect
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xad);
	reg8 |= (3 << 0);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xad, reg8);

	reg32 = pci_read_config32(PCI_DEV(0, 0x1d, 7), 0xfc);
	reg32 |= (1 << 29) | (1 << 17);
	pci_write_config32(PCI_DEV(0, 0x1d, 7), 0xfc, reg32);

	reg32 = pci_read_config32(PCI_DEV(0, 0x1d, 7), 0xdc);
	reg32 |= (1 << 31) | (1 << 27);
	pci_write_config32(PCI_DEV(0, 0x1d, 7), 0xdc, reg32);

	RCBA32(0x0088) = 0x0011d000;
	RCBA16(0x01fc) = 0x060f;
	RCBA32(0x01f4) = 0x86000040;
	RCBA32(0x0214) = 0x10030549;
	RCBA32(0x0218) = 0x00020504;
	RCBA8(0x0220) = 0xc5;
	reg32 = RCBA32(0x3410);
	reg32 |= (1 << 6);
	RCBA32(0x3410) = reg32;
	reg32 = RCBA32(0x3430);
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	RCBA32(0x3430) = reg32;
	RCBA32(0x3418) |= (1 << 0);
	RCBA16(0x0200) = 0x2008;
	RCBA8(0x2027) = 0x0d;
	RCBA16(0x3e08) |= (1 << 7);
	RCBA16(0x3e48) |= (1 << 7);
	RCBA32(0x3e0e) |= (1 << 7);
	RCBA32(0x3e4e) |= (1 << 7);

	// next step only on ich7m b0 and later:
	reg32 = RCBA32(0x2034);
	reg32 &= ~(0x0f << 16);
	reg32 |= (5 << 16);
	RCBA32(0x2034) = reg32;
}

#include <cpu/intel/romstage.h>
void main(unsigned long bist)
{
	int s3resume = 0;

	if (bist == 0)
		enable_lapic();

	/* Force PCIRST# */
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, SBR);
	udelay(200 * 1000);
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, 0);

	ich7_enable_lpc();
	early_superio_config_w83627thg();

	/* Set up the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		outb(0x6, 0xcf9);
		halt();
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	sdram_initialize(s3resume ? 2 : 0, NULL);

	/* Perform some initialization that must run before stage2 */
	early_ich7_init();

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization(s3resume);
}
