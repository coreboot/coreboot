/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
 * Copyright (C) 2014 Vladimir Serbinenko
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/byteorder.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <cbmem.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/bd82x6x/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include <cbfs.h>

static void pch_enable_lpc(void)
{
	/* T520 EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   CNF2_LPC_EN | CNF1_LPC_EN | MC_LPC_EN | KBC_LPC_EN |
			   COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0x7c1601);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, 0xc15e1);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, 0x0c06a1);

	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x10);

	pci_write_config32(PCH_LPC_DEV, 0xac,
			   0x80010000);
}

static void rcba_config(void)
{
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  WLAN   INTA -> PIRQB
	 * D28IP_P2IP  ETH0   INTB -> PIRQF
	 * D28IP_P3IP  SDCARD INTC -> PIRQD
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQF
	 * D31IP_SIP   SATA   INTA -> PIRQB (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQH
	 * D31IP_TTIP  THRT   INTC -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQA (MSI)
	 *
	 * Trackpad interrupt is edge triggered and cannot be shared.
	 * TRACKPAD                -> PIRQG

	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
			(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P1IP) | (INTB << D28IP_P2IP) |
			(INTC << D28IP_P3IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQB, PIRQF, PIRQD, PIRQE);
	DIR_ROUTE(D27IR, PIRQA, PIRQH, PIRQA, PIRQB);
	DIR_ROUTE(D26IR, PIRQF, PIRQE, PIRQG, PIRQH);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	RCBA32(FD) = 0x17f81fe3;
	RCBA32(BUC) = 0;
}

void main(unsigned long bist)
{
	int boot_mode = 0;
	int cbmem_was_initted;
	u32 pm1_cnt;
	u16 pm1_sts;

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		outb(0x6, 0xcf9);
		hlt ();
	}

	struct pei_data pei_data = {
		.pei_version = PEI_VERSION,
		.mchbar = DEFAULT_MCHBAR,
		.dmibar = DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = DEFAULT_RCBABASE,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, // 0 Mobile, 1 Desktop/Server
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xA0, 0x00,0xA2,0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		.gbe_enable = 1,
		.ddr3lv_support = 0,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			 /* enabled   usb oc pin    length */
			{ 1, 0, 0x0080 }, /* P0 (left, fan side), OC 0 */
			{ 1, 1, 0x0080 }, /* P1 (left touchpad side), OC 1 */
			{ 1, 0, 0x0040 }, /* P2: dock, OC 3 */
			{ 1, 0, 0x0040 }, /* P3: wwan, no OC */
			{ 1, 0, 0x0080 }, /* P4: Wacom tablet on X230t, otherwise empty */
			{ 1, 0, 0x0080 }, /* P5: Expresscard, no OC */
			{ 0, 0, 0x0000 }, /* P6: Empty */
			{ 1, 0, 0x0080 }, /* P7: dock, no OC */
			{ 1, 4, 0x0080 }, /* P8: Empty */
			{ 1, 5, 0x0080 }, /* P9: Right (EHCI debug), OC 5 */
			{ 1, 0, 0x0040 }, /* P10: fingerprint reader, no OC */
			{ 1, 0, 0x0040 }, /* P11: bluetooth, no OC. */
			{ 1, 0, 0x0040 }, /* P12: wlan, no OC */
			{ 1, 0, 0x0080 }, /* P13: webcam, no OC */
		},
		.ddr_refresh_rate_config = 2, /* Force double refresh rate */
	};

	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);

	outl(0x3962a5ff, DEFAULT_GPIOBASE);
	outl(0x8ebf6aff, DEFAULT_GPIOBASE + 4);
	outl(0x66957f3b, DEFAULT_GPIOBASE + 0xc);
	outl(0x00080000, DEFAULT_GPIOBASE + 0x20);
	outl(0x00002082, DEFAULT_GPIOBASE + 0x2c);
	outl(0x02ff06de, DEFAULT_GPIOBASE + 0x30);
	outl(0x1f47f9f5, DEFAULT_GPIOBASE + 0x34);
	outl(0xbdbffd47, DEFAULT_GPIOBASE + 0x38);
	outl(0x000000f0, DEFAULT_GPIOBASE + 0x40);
	outl(0x00000ff0, DEFAULT_GPIOBASE + 0x44);
	outl(0x00000f4f, DEFAULT_GPIOBASE + 0x48);
	outl(0x01000000, DEFAULT_GPIOBASE + 0x60);

	/* Initialize console device(s) */
	console_init();

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	sandybridge_early_initialization(SANDYBRIDGE_MOBILE);
	printk(BIOS_DEBUG, "Back from sandybridge_early_initialization()\n");

	/* Check PM1_STS[15] to see if we are waking from Sx */
	pm1_sts = inw(DEFAULT_PMBASE + PM1_STS);

	/* Read PM1_CNT[12:10] to determine which Sx state */
	pm1_cnt = inl(DEFAULT_PMBASE + PM1_CNT);

	if ((pm1_sts & WAK_STS) && ((pm1_cnt >> 10) & 7) == 5) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		boot_mode = 2;
		/* Clear SLP_TYPE. This will break stage2 but
		 * we care for that when we get there.
		 */
		outl(pm1_cnt & ~(7 << 10), DEFAULT_PMBASE + PM1_CNT);
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	post_code(0x38);
	/* Enable SPD ROMs and DDR-III DRAM */
	enable_smbus();

	/* Prepare USB controller early in S3 resume */
	if (boot_mode == 2)
		enable_usb_bar();

	post_code(0x39);
	post_code(0x3a);

	pei_data.boot_mode = boot_mode;
	timestamp_add_now(TS_BEFORE_INITRAM);

	/* MRC.bin has a bug and sometimes halts (instead of reboot?).
	 */
	if (boot_mode != 2) {
		RCBA32(GCS) = RCBA32(GCS) & ~(1 << 5);	/* reset */
		outw((0 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* let timer go */
	}

	sdram_initialize(&pei_data);

	if (boot_mode != 2) {
		RCBA32(GCS) = RCBA32(GCS) | (1 << 5);	/* No reset */
		outw((1 << 11), DEFAULT_PMBASE | 0x60 | 0x08);	/* halt timer */
	}

	timestamp_add_now(TS_AFTER_INITRAM);
	post_code(0x3c);

	rcba_config();
	post_code(0x3d);

	quick_ram_check();
	post_code(0x3e);

	MCHBAR16(SSKPD) = 0xCAFE;
	cbmem_was_initted = !cbmem_recovery(boot_mode == 2);
	if (boot_mode != 2) {
		save_mrc_data(&pei_data);
	}

#if CONFIG_HAVE_ACPI_RESUME
	/* If there is no high memory area, we didn't boot before, so
	 * this is not a resume. In that case we just create the cbmem toc.
	 */

	*(u32 *)CBMEM_BOOT_MODE = 0;
	*(u32 *)CBMEM_RESUME_BACKUP = 0;

	if ((boot_mode == 2) && cbmem_was_initted) {
		void *resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
		if (resume_backup_memory) {
			*(u32 *)CBMEM_BOOT_MODE = boot_mode;
			*(u32 *)CBMEM_RESUME_BACKUP = (u32)resume_backup_memory;
		}
		/* Magic for S3 resume */
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafed00d);
	} else if (boot_mode == 2) {
		/* Failed S3 resume, reset to come up cleanly */
		outb(0x6, 0xcf9);
		hlt();
	} else {
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafebabe);
	}
#endif
	post_code(0x3f);
	timestamp_add_now(TS_END_ROMSTAGE);
}
