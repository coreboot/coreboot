/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
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
 */

/* __PRE_RAM__ means: use "unsigned" for device, not a struct. */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <arch/acpi.h>
#include <timestamp.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <halt.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include "option_table.h"

static void ich7_enable_lpc(void)
{
	int lpt_en = 0;
	if (read_option(lpt, 0) != 0)
		lpt_en = LPT_LPC_EN; /* enable LPT */

	/* Enable Serial IRQ */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0xd0);
	/* decode range */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0007);
	/* decode range */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF2_LPC_EN | CNF1_LPC_EN
			| MC_LPC_EN | KBC_LPC_EN | GAMEH_LPC_EN | GAMEL_LPC_EN
			| FDD_LPC_EN | lpt_en | COMB_LPC_EN | COMA_LPC_EN);
	/* COM3 and COM4 decode? */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN1_DEC, 0x1c02e1);
	/* ??decode?? */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x00fc0601);
	/* EC decode? */
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN3_DEC, 0x00040069);
}

/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
static inline void pnp_enter_ext_func_mode(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_ext_func_mode(pnp_devfn_t dev)
{
	unsigned int port = dev >> 8;
	outb(0xaa, port);
}

static void pnp_write_register(pnp_devfn_t dev, int reg, int val)
{
	unsigned int port = dev >> 8;
	outb(reg, port);
	outb(val, port+1);
}

static void early_superio_config(void)
{
	pnp_devfn_t dev;

	dev = PNP_DEV(0x2e, 0x00);

	pnp_enter_ext_func_mode(dev);
	pnp_write_register(dev, 0x01, 0x94); /* Extended Parport modes */
	pnp_write_register(dev, 0x02, 0x88); /* UART power on */
	pnp_write_register(dev, 0x03, 0x72); /* Floppy */
	pnp_write_register(dev, 0x04, 0x01); /* EPP + SPP */
	pnp_write_register(dev, 0x14, 0x03); /* Floppy */
	pnp_write_register(dev, 0x20, (0x3f0 >> 2)); /* Floppy */
	pnp_write_register(dev, 0x23, (0x378 >> 2)); /* PP base */
	pnp_write_register(dev, 0x24, (0x3f8 >> 2)); /* UART1 base */
	pnp_write_register(dev, 0x25, (0x2f8 >> 2)); /* UART2 base */
	pnp_write_register(dev, 0x26, (2 << 4) | 0); /* FDC + PP DMA */
	pnp_write_register(dev, 0x27, (6 << 4) | 7); /* FDC + PP DMA */
	pnp_write_register(dev, 0x28, (4 << 4) | 3); /* UART1,2 IRQ */
	/* These are the SMI status registers in the SIO: */
	pnp_write_register(dev, 0x30, (0x600 >> 4)); /* Runtime Register Block Base */

	pnp_write_register(dev, 0x31, 0x00); /* GPIO1 DIR */
	pnp_write_register(dev, 0x32, 0x00); /* GPIO1 POL */
	pnp_write_register(dev, 0x33, 0x40); /* GPIO2 DIR */
	pnp_write_register(dev, 0x34, 0x00); /* GPIO2 POL */
	pnp_write_register(dev, 0x35, 0xff); /* GPIO3 DIR */
	pnp_write_register(dev, 0x36, 0x00); /* GPIO3 POL */
	pnp_write_register(dev, 0x37, 0xe0); /* GPIO4 DIR */
	pnp_write_register(dev, 0x38, 0x00); /* GPIO4 POL */
	pnp_write_register(dev, 0x39, 0x80); /* GPIO4 POL */

	pnp_exit_ext_func_mode(dev);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */

	/* Device 1f interrupt pin register */
	RCBA32(D31IP) = 0x00042220;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0232;
	RCBA16(D30IR) = 0x3246;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x3216;

	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;

	/* Disable unused devices */
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_PCIE3 | FD_PCIE2 |
			 FD_INTLAN | FD_ACMOD | FD_HDAUD | FD_PATA;
	RCBA32(FD) |= (1 << 0); /* Required. */

	/* This should probably go into the ACPI OS Init trap */

	/* Set up I/O Trap #0 for 0xfe00 (SMIC) */
	RCBA32(0x1e84) = 0x00020001;
	RCBA32(0x1e80) = 0x0000fe01;

	/* Set up I/O Trap #3 for 0x800-0x80c (Trap) */
	RCBA32(0x1e9c) = 0x000200f0;
	RCBA32(0x1e98) = 0x000c0801;
}

static void early_ich7_init(void)
{
	uint8_t reg8;
	uint32_t reg32;

	/* program secondary mlt XXX byte? */
	pci_write_config8(PCI_DEV(0, 0x1e, 0), 0x1b, 0x20);

	/* reset rtc power status */
	reg8 = pci_read_config8(PCI_DEV(0, 0x1f, 0), 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0xa4, reg8);

	/* usb transient disconnect */
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
	reg32 = RCBA32(GCS);
	reg32 |= (1 << 6);
	RCBA32(GCS) = reg32;
	reg32 = RCBA32(0x3430);
	reg32 &= ~(3 << 0);
	reg32 |= (1 << 0);
	RCBA32(0x3430) = reg32;
	RCBA32(FD) |= (1 << 0);
	RCBA16(0x0200) = 0x2008;
	RCBA8(0x2027) = 0x0d;
	RCBA16(0x3e08) |= (1 << 7);
	RCBA16(0x3e48) |= (1 << 7);
	RCBA32(0x3e0e) |= (1 << 7);
	RCBA32(0x3e4e) |= (1 << 7);

	/* next step only on ich7m b0 and later: */
	reg32 = RCBA32(0x2034);
	reg32 &= ~(0x0f << 16);
	reg32 |= (5 << 16);
	RCBA32(0x2034) = reg32;
}

static void init_artec_dongle(void)
{
	/* Enable 4MB decoding */
	outb(0xf1, 0x88);
	outb(0xf4, 0x88);
}

void mainboard_romstage_entry(unsigned long bist)
{
	int s3resume = 0;


	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	/* Force PCIRST# */
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, SBR);
	udelay(200 * 1000);
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, 0);

	ich7_enable_lpc();
	early_superio_config();

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

	/* This has to happen after i945_early_initialization() */
	init_artec_dongle();

	s3resume = southbridge_detect_s3_resume();

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(s3resume ? 2 : 0, NULL);
	timestamp_add_now(TS_AFTER_INITRAM);

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
