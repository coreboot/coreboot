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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include "northbridge/intel/i945/i945.h"
#include "northbridge/intel/i945/raminit.h"
#include "southbridge/intel/i82801gx/i82801gx.h"
#include "option_table.h"

void setup_ich7_gpios(void)
{
	u32 gpios;

	printk(BIOS_DEBUG, " GPIOS...");
	/* General Registers */
	outl(0x1f28f7c2, DEFAULT_GPIOBASE + 0x00);	/* GPIO_USE_SEL */
	outl(0xe0e809c3, DEFAULT_GPIOBASE + 0x04);	/* GP_IO_SEL */
	// Power On value is eede1fbf, we set: (TODO explain why)
	//   -- [21] = 1
	//   -- [20] = 0
	//   -- [18] = 0
	//   -- [17] = 0
	//   -- [13] = 1
	//   -- [05] = 0
	//   -- [04] = 0
	//   -- [03] = 0
	//   -- [02] = 0
	//   We should probably do this explicitly bitwise, see below.
	outl(0xeee83f83, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
	/* Output Control Registers */
	outl(0x00000000, DEFAULT_GPIOBASE + 0x18);	/* GPO_BLINK */
	/* Input Control Registers */
	outl(0x00000180, DEFAULT_GPIOBASE + 0x2c);	/* GPI_INV */
	outl(0x000000e6, DEFAULT_GPIOBASE + 0x30);	/* GPIO_USE_SEL2 */
	outl(0x000000d0, DEFAULT_GPIOBASE + 0x34);	/* GP_IO_SEL2 */
	outl(0x00000034, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL2 */

	printk(BIOS_SPEW, "\n  Initializing drive bay...\n");
	gpios = inl(DEFAULT_GPIOBASE + 0x38); // GPIO Level 2
	gpios |= (1 << 0); // GPIO33 = ODD
	gpios |= (1 << 1); // GPIO34 = IDE_RST#
	outl(gpios, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL2 */

	gpios = inl(DEFAULT_GPIOBASE + 0x0c); // GPIO Level
	gpios &= ~(1 << 13);	// ??
	outl(gpios, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */

	printk(BIOS_SPEW, "\n  Initializing Ethernet NIC...\n");
	gpios = inl(DEFAULT_GPIOBASE + 0x0c); // GPIO Level
	gpios &= ~(1 << 24);	// Enable LAN Power
	outl(gpios, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
}

static void ich7_enable_lpc(void)
{
	int lpt_en = 0;
	if (read_option(lpt, 0) != 0) {
	       lpt_en = 1<<2; // enable LPT
	}
	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x64, 0xd0);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0007);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x3f0b | lpt_en);
	// Enable 0x02e0 - 0x2ff
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x84, 0x001c02e1);
	// Enable 0x600 - 0x6ff
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x88, 0x00fc0601);
	// Enable 0x68 - 0x6f
	pci_write_config32(PCI_DEV(0, 0x1f, 0), 0x8c, 0x00040069);
}

/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
static void pnp_enter_ext_func_mode(device_t dev)
{
	unsigned int port = dev >> 8;
	outb(0x55, port);
}

static void pnp_exit_ext_func_mode(device_t dev)
{
	unsigned int port = dev >> 8;
	outb(0xaa, port);
}

static void pnp_write_register(device_t dev, int reg, int val)
{
	unsigned int port = dev >> 8;
	outb(reg, port);
	outb(val, port+1);
}

static void early_superio_config(void)
{
	device_t dev;

	dev=PNP_DEV(0x4e, 0x00);

	pnp_enter_ext_func_mode(dev);
	pnp_write_register(dev, 0x02, 0x0e); // UART power
	pnp_write_register(dev, 0x1b, (0x3e8 >> 2)); // UART3 base
	pnp_write_register(dev, 0x1c, (0x2e8 >> 2)); // UART4 base
	pnp_write_register(dev, 0x1d, (5 << 4) | 11); // UART3,4 IRQ
	pnp_write_register(dev, 0x1e, 1); // no 32khz clock
	pnp_write_register(dev, 0x24, (0x3f8 >> 2)); // UART1 base
	pnp_write_register(dev, 0x28, (4 << 4) | 0); // UART1,2 IRQ
	pnp_write_register(dev, 0x2c, 0); // DMA0 FIR
	pnp_write_register(dev, 0x30, (0x600 >> 4)); // Runtime Register Block Base

	pnp_write_register(dev, 0x31, 0xce); // GPIO1 DIR
	pnp_write_register(dev, 0x32, 0x00); // GPIO1 POL
	pnp_write_register(dev, 0x33, 0x0f); // GPIO2 DIR
	pnp_write_register(dev, 0x34, 0x00); // GPIO2 POL
	pnp_write_register(dev, 0x35, 0xa8); // GPIO3 DIR
	pnp_write_register(dev, 0x36, 0x00); // GPIO3 POL
	pnp_write_register(dev, 0x37, 0xa8); // GPIO4 DIR
	pnp_write_register(dev, 0x38, 0x00); // GPIO4 POL

	pnp_write_register(dev, 0x39, 0x00); // GPIO1 OUT
	pnp_write_register(dev, 0x40, 0x80); // GPIO2/MISC OUT
	pnp_write_register(dev, 0x41, 0x00); // GPIO5 OUT
	pnp_write_register(dev, 0x42, 0xa8); // GPIO5 DIR
	pnp_write_register(dev, 0x43, 0x00); // GPIO5 POL
	pnp_write_register(dev, 0x44, 0x00); // GPIO ALT1
	pnp_write_register(dev, 0x45, 0x50); // GPIO ALT2
	pnp_write_register(dev, 0x46, 0x00); // GPIO ALT3

	pnp_write_register(dev, 0x48, 0x55); // GPIO ALT5
	pnp_write_register(dev, 0x49, 0x55); // GPIO ALT6
	pnp_write_register(dev, 0x4a, 0x55); // GPIO ALT7
	pnp_write_register(dev, 0x4b, 0x55); // GPIO ALT8
	pnp_write_register(dev, 0x4c, 0x55); // GPIO ALT9
	pnp_write_register(dev, 0x4d, 0x55); // GPIO ALT10

	pnp_exit_ext_func_mode(dev);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	//RCBA32(0x0014) = 0x80000001;
	//RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00042220;
	/* Device 1d interrupt pin register */
	RCBA32(0x310c) = 0x00214321;

	/* dev irq route register */
	RCBA16(0x3140) = 0x0232;
	RCBA16(0x3142) = 0x3246;
	RCBA16(0x3144) = 0x0237;
	RCBA16(0x3146) = 0x3201;
	RCBA16(0x3148) = 0x3216;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Disable unused devices */
	RCBA32(0x3418) = FD_PCIE6 | FD_PCIE5 | FD_INTLAN | FD_ACMOD | FD_ACAUD | FD_PATA;
	RCBA32(0x3418) |= (1 << 0); // Required.

	/* Enable PCIe Root Port Clock Gate */
	// RCBA32(0x341c) = 0x00000001;


	/* This should probably go into the ACPI enable trap */
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

#include <cbmem.h>

void main(unsigned long bist)
{
	u32 reg32;
	int boot_mode = 0;

	if (bist == 0)
		enable_lapic();

#if 0
	/* Force PCIRST# */
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, SBR);
	udelay(200 * 1000);
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, 0);
#endif

	ich7_enable_lpc();
	early_superio_config();

	/* Set up the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected, rebooting properly\n");
		outb(0x6, 0xcf9);
		while (1) asm("hlt");
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	/* Read PM1_CNT */
	reg32 = inl(DEFAULT_PMBASE + 0x04);
	printk(BIOS_DEBUG, "PM1_CNT: %08x\n", reg32);
	if (((reg32 >> 10) & 7) == 5) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		boot_mode = 2;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(reg32 & ~(7 << 10), DEFAULT_PMBASE + 0x04);

#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	sdram_initialize(boot_mode, NULL);

	/* Perform some initialization that must run before stage2 */
	early_ich7_init();

	/* This should probably go away. Until now it is required
	 * and mainboard specific
	 */
	rcba_config();

	/* Chipset Errata! */
	fixup_i945_errata();

	/* Initialize the internal PCIe links before we go into stage2 */
	i945_late_initialization();

#if !CONFIG_HAVE_ACPI_RESUME
	/* When doing resume, we must not overwrite RAM */
#if CONFIG_DEBUG_RAM_SETUP
	sdram_dump_mchbar_registers();

	{
		/* This will not work if TSEG is in place! */
		u32 tom = pci_read_config32(PCI_DEV(0,2,0), 0x5c);

		printk(BIOS_DEBUG, "TOM: 0x%08x\n", tom);
		ram_check(0x00000000, 0x000a0000);
		ram_check(0x00100000, tom);
	}
#endif
#endif
	MCHBAR16(SSKPD) = 0xCAFE;

#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */
	if ((boot_mode == 2) && cbmem_reinit()) {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);

		/* copy 1MB - 64K to high tables ram_base to prevent memory corruption
		 * through stage 2. We could keep stuff like stack and heap in high tables
		 * memory completely, but that's a wonderful clean up task for another
		 * day.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)CONFIG_RAMBASE, HIGH_MEMORY_SAVE);

		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, SKPAD_ACPI_S3_MAGIC);
	}
#endif
}
