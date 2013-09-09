/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 Sven Schnelle <svens@stackframe.org>
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

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <cbmem.h>
#include <timestamp.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include "northbridge/intel/i945/i945.h"
#include "northbridge/intel/i945/raminit.h"
#include "southbridge/intel/i82801gx/i82801gx.h"
#include "dock.h"

void setup_ich7_gpios(void)
{
	printk(BIOS_DEBUG, " GPIOS...");

	/* X60 GPIO:
	 * 1: HDD_PRESENCE#
	 * 6: Unknown (Pulled high by R215 to VCC3B)
	 * 7: BDC_PRESENCE#
	 * 8: H8_WAKE#
	 * 9: RTC_BAT_IN#
	 * 10: Unknown (Pulled high by R700 to VCC3M)
	 * 12: H8SCI#
	 * 13: SLICE_ON_3M#
	 * 14: Unknown (Pulled high by R321 to VCC3)
	 * 15: Unknown (Pulled high by R258 to VCC3)
	 * 19: Unknown (Pulled low  by R594)
	 * 21: Unknown (Pulled high by R145 to VCC3)
	 * 22: FWH_WP#
	 * 25: MDC_KILL#
	 * 33: HDD_PRESENCE_2#
	 * 35: CLKREQ_SATA#
	 * 36: PLANARID0
	 * 37: PLANARID1
	 * 38: PLANARID2
	 * 39: PLANARID3
	 * 48: FWH_TBL#
	 */

	outl(0x1f40f7c2, DEFAULT_GPIOBASE + 0x00);	/* GPIO_USE_SEL */
	outl(0xe0e8ffc3, DEFAULT_GPIOBASE + 0x04);	/* GP_IO_SEL */
	outl(0xfbf6ddfd, DEFAULT_GPIOBASE + 0x0c);	/* GP_LVL */
	/* Output Control Registers */
	outl(0x00040000, DEFAULT_GPIOBASE + 0x18);	/* GPO_BLINK */
	/* Input Control Registers */
	outl(0x000039ff, DEFAULT_GPIOBASE + 0x2c);	/* GPI_INV */
	outl(0x000100f2, DEFAULT_GPIOBASE + 0x30);	/* GPIO_USE_SEL2 */
	outl(0x000000f0, DEFAULT_GPIOBASE + 0x34);	/* GP_IO_SEL2 */
	outl(0x00030043, DEFAULT_GPIOBASE + 0x38);	/* GP_LVL */
}

static void ich7_enable_lpc(void)
{
	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), 0x64, 0xd0);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x80, 0x0210);
	// decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x82, 0x1f0d);

	/* range 0x1600 - 0x167f */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x84, 0x1601);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x86, 0x007c);

	/* range 0x15e0 - 0x10ef */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x88, 0x15e1);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x8a, 0x000c);

	/* range 0x1680 - 0x169f */
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x8c, 0x1681);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), 0x8e, 0x001c);
}

static void early_superio_config(void)
{
	int timeout = 100000;
	device_t dev = PNP_DEV(0x2e, 3);

	pnp_write_config(dev, 0x29, 0x06);

	while (!(pnp_read_config(dev, 0x29) & 0x08) && timeout--)
		udelay(1000);

	/* Enable COM1 */
	pnp_set_logical_device(dev);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3f8);
	pnp_set_enable(dev, 1);
}

static void rcba_config(void)
{
	/* Set up virtual channel 0 */
	RCBA32(0x0014) = 0x80000001;
	RCBA32(0x001c) = 0x03128010;

	/* Device 1f interrupt pin register */
	RCBA32(0x3100) = 0x00001230;
	RCBA32(0x3108) = 0x40004321;

	/* PCIe Interrupts */
	RCBA32(0x310c) = 0x00004321;
	/* HD Audio Interrupt */
	RCBA32(0x3110) = 0x00000002;

	/* dev irq route register */
	RCBA16(0x3140) = 0x1007;
	RCBA16(0x3142) = 0x0076;
	RCBA16(0x3144) = 0x3210;
	RCBA16(0x3146) = 0x7654;
	RCBA16(0x3148) = 0x0010;

	/* Enable IOAPIC */
	RCBA8(0x31ff) = 0x03;

	/* Enable upper 128bytes of CMOS */
	RCBA32(0x3400) = (1 << 2);

	/* Disable unused devices */
	RCBA32(0x3418) = FD_PCIE6 | FD_PCIE5 | FD_INTLAN | FD_ACMOD | FD_ACAUD;
	RCBA32(0x3418) |= (1 << 0);	// Required.

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

void main(unsigned long bist)
{
	u32 reg32;
	int boot_mode = 0;
	int cbmem_was_initted;
	const u8 spd_addrmap[2 * DIMM_SOCKETS] = { 0x50, 0x52, 0x51, 0x53 };


	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	/* Force PCIRST# */
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, SBR);
	udelay(200 * 1000);
	pci_write_config16(PCI_DEV(0, 0x1e, 0), BCTRL, 0);

	ich7_enable_lpc();

	dlpc_init();
	/* dock_init initializes the DLPC switch on
	 *  thinpad side, so this is required even
	 *  if we're undocked.
	 */
	if (dock_present()) {
		dock_connect();
		early_superio_config();
	}

	/* Set up the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG,
		       "Soft reset detected, rebooting properly.\n");
		outb(0x6, 0xcf9);
		while (1)
			asm("hlt");
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

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(boot_mode, spd_addrmap);
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
	i945_late_initialization();

#if !CONFIG_HAVE_ACPI_RESUME
#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
#if CONFIG_DEBUG_RAM_SETUP
	sdram_dump_mchbar_registers();

	{
		/* This will not work if TSEG is in place! */
		u32 tom = pci_read_config32(PCI_DEV(0, 2, 0), 0x5c);

		printk(BIOS_DEBUG, "TOM: 0x%08x\n", tom);
		ram_check(0x00000000, 0x000a0000);
		ram_check(0x00100000, tom);
	}
#endif
#endif
#endif

	MCHBAR16(SSKPD) = 0xCAFE;

	cbmem_was_initted = !cbmem_initialize();

#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */
	if ((boot_mode == 2) && cbmem_was_initted) {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);

		/* copy 1MB - 64K to high tables ram_base to prevent memory corruption
		 * through stage 2. We could keep stuff like stack and heap in high tables
		 * memory completely, but that's a wonderful clean up task for another
		 * day.
		 */
		if (resume_backup_memory)
			memcpy(resume_backup_memory, (void *)CONFIG_RAMBASE,
			       HIGH_MEMORY_SAVE);

		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD,
				   SKPAD_ACPI_S3_MAGIC);
	}
#endif

	timestamp_add_now(TS_END_ROMSTAGE);

}
