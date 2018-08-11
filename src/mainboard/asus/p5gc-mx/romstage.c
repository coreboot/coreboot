/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2008 coresystems GmbH
 * Copyright (C) 2016 Arthur Heymans arthur@aheymans.xyz
 * Copyright (C) 2016 Kyösti Mälkki <kyosti.malkki@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

// __PRE_RAM__ means: use "unsigned" for device, not a struct.

#include <stdint.h>
#include <string.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <lib.h>
#include <arch/acpi.h>
#include <timestamp.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627dhg/w83627dhg.h>
#include <console/console.h>
#include <cpu/x86/bist.h>
#include <cpu/intel/romstage.h>
#include <halt.h>
#include <northbridge/intel/i945/i945.h>
#include <northbridge/intel/i945/raminit.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <cpu/x86/msr.h>
#include <cpu/intel/speedstep.h>
#include <arch/cpu.h>

#define SERIAL_DEV PNP_DEV(0x2e, W83627DHG_SP1)
#define GPIO_DEV PNP_DEV(0x2e, W83627DHG_GPIO2345_V)

/*
 * BSEL0 is connected with GPIO32
 * BSEL1 is connected with GPIO33 with inversed logic
 * BSEL2 is connected with GPIO55
 */
static int setup_sio_gpio(u8 bsel)
{
	int need_reset = 0;
	u8 reg, old_reg;

	pnp_enter_ext_func_mode(GPIO_DEV);
	pnp_set_logical_device(GPIO_DEV);

	reg = 0x9a;
	old_reg = pnp_read_config(GPIO_DEV, 0x2c);
	pnp_write_config(GPIO_DEV, 0x2c, reg);
	need_reset = (reg != old_reg);

	pnp_write_config(GPIO_DEV, 0x30, 0x0e);
	pnp_write_config(GPIO_DEV, 0xe0, 0xde);
	pnp_write_config(GPIO_DEV, 0xf0, 0xf3);
	pnp_write_config(GPIO_DEV, 0xf4, 0x80);
	pnp_write_config(GPIO_DEV, 0xf5, 0x80);

	/* Invert GPIO33 */
	pnp_write_config(GPIO_DEV, 0xf2, 0x08);

	reg = (bsel & 3) << 2;
	old_reg = pnp_read_config(GPIO_DEV, 0xf1);
	pnp_write_config(GPIO_DEV, 0xf1, reg);
	need_reset += ((reg & 0xc) != (old_reg & 0xc));

	reg = (bsel >> 2) << 5;
	old_reg = pnp_read_config(GPIO_DEV, 0xe1);
	pnp_write_config(GPIO_DEV, 0xe1, reg);
	need_reset += ((reg & 0x20) != (old_reg & 0x20));

	pnp_exit_ext_func_mode(GPIO_DEV);

	return need_reset;
}

static u8 msr_get_fsb(void)
{
	u8 fsbcfg;
	msr_t msr;
	const u32 eax = cpuid_eax(1);

	/* Netburst */
	if (((eax >> 8) & 0xf) == 0xf) {
		msr = rdmsr(MSR_EBC_FREQUENCY_ID);
		fsbcfg = (msr.lo >> 16) & 0x7;
	} else { /* Intel Core 2 */
		msr = rdmsr(MSR_FSB_FREQ);
		fsbcfg = msr.lo & 0x7;
	}

	return fsbcfg;
}

static void ich7_enable_lpc(void)
{
	// Enable Serial IRQ
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0xd0);
	// Set COM1/COM2 decode range
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0010);
	// Enable COM1
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF1_LPC_EN
			| KBC_LPC_EN | FDD_LPC_EN | LPT_LPC_EN | COMB_LPC_EN
			| COMA_LPC_EN);
	// Enable SuperIO Power Management Events
	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN1_DEC, 0x00040291);
}

static void rcba_config(void)
{
	/* Enable IOAPIC */
	RCBA8(OIC) = 0x03;

	/* Disable unused devices */
	RCBA32(FD) = FD_PCIE6 | FD_PCIE5 | FD_PCIE4 | FD_PCIE3 | FD_ACMOD
		| FD_ACAUD | 1;

	/* Enable PCIe Root Port Clock Gate */
	RCBA32(CG) = 0x00000001;
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
	RCBA32(0x0214) = 0x10030509;
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

	// next step only on ich7m b0 and later:
	reg32 = RCBA32(0x2034);
	reg32 &= ~(0x0f << 16);
	reg32 |= (5 << 16);
	RCBA32(0x2034) = reg32;
}

void mainboard_romstage_entry(unsigned long bist)
{
	int s3resume = 0, boot_mode = 0;

	u8 c_bsel = msr_get_fsb();

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	ich7_enable_lpc();

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Set up the console */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected.\n");
		boot_mode = 1;
	}

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i945_early_initialization();

	s3resume = southbridge_detect_s3_resume();

	/*
	 * Result is that FSB is incorrect on s3 resume (fixed at 800MHz).
	 * Some CPU accept this others don't.
	 */
	if (!s3resume && setup_sio_gpio(c_bsel)) {
		printk(BIOS_DEBUG,
			"Needs reset to configure CPU BSEL straps\n");
		outb(0xe, 0xcf9);
		halt();
	}

	/* Enable SPD ROMs and DDR-II DRAM */
	enable_smbus();

#if CONFIG_DEFAULT_CONSOLE_LOGLEVEL > 8
	dump_spd_registers();
#endif

	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(s3resume ? 2 : boot_mode, NULL);
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
