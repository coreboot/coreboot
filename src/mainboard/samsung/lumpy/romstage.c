/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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
#include <arch/io.h>
#include <arch/byteorder.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <cbfs.h>
#include <cbmem.h>
#include <console/console.h>
#include "northbridge/intel/sandybridge/sandybridge.h"
#include "northbridge/intel/sandybridge/raminit.h"
#include "southbridge/intel/bd82x6x/pch.h"
#include "southbridge/intel/bd82x6x/gpio.h"
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include "option_table.h"
#include "gpio.h"
#if CONFIG_CONSOLE_SERIAL8250
#include "superio/smsc/lpc47n207/lpc47n207.h"
#include "superio/smsc/lpc47n207/early_serial.c"
#endif
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif

static void pch_enable_lpc(void)
{
	/* Set COM1/COM2 decode range */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);

#if CONFIG_CONSOLE_SERIAL8250
	/* Enable SuperIO + EC + KBC + COM1 + lpc47n207 config*/
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | MC_LPC_EN |
		KBC_LPC_EN | CNF2_LPC_EN | COMA_LPC_EN);

	/* map full 256 bytes at 0x1600 to the LPC bus */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, 0xfc1601);

	try_enabling_LPC47N207_uart();
#else
	/* Enable SuperIO + EC + KBC */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, CNF1_LPC_EN | MC_LPC_EN |
		KBC_LPC_EN);
#endif
}

static void rcba_config(void)
{
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
	RCBA32(D31IP) = (INTB << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTC << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
		(INTB << D28IP_P4IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);

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
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	reg32 |= PCH_DISABLE_ALWAYS;
	RCBA32(FD) = reg32;
}

static void early_pch_init(void)
{
	u8 reg8;

	// reset rtc power status
	reg8 = pci_read_config8(PCH_LPC_DEV, 0xa4);
	reg8 &= ~(1 << 2);
	pci_write_config8(PCH_LPC_DEV, 0xa4, reg8);
}

void main(unsigned long bist)
{
	int boot_mode = 0;
	int cbmem_was_initted;
	u32 pm1_cnt;
	u16 pm1_sts;

	struct pei_data pei_data = {
		pei_version: PEI_VERSION,
		mchbar: DEFAULT_MCHBAR,
		dmibar: DEFAULT_DMIBAR,
		epbar: DEFAULT_EPBAR,
		pciexbar: CONFIG_MMCONF_BASE_ADDRESS,
		smbusbar: SMBUS_IO_BASE,
		wdbbar: 0x4000000,
		wdbsize: 0x1000,
		hpet_address: CONFIG_HPET_ADDRESS,
		rcba: DEFAULT_RCBABASE,
		pmbase: DEFAULT_PMBASE,
		gpiobase: DEFAULT_GPIOBASE,
		thermalbase: 0xfed08000,
		system_type: 0, // 0 Mobile, 1 Desktop/Server
		tseg_size: CONFIG_SMM_TSEG_SIZE,
		spd_addresses: { 0xa0, 0x00,0x00,0x00 },
		ts_addresses: { 0x30, 0x00, 0x00, 0x00 },
		ec_present: 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		dimm_channel0_disabled: 2,
		dimm_channel1_disabled: 2,
		max_ddr3_freq: 1333,
		usb_port_config: {
			{ 1, 0, 0x0080 }, /* P0: Port 0      (OC0) */
			{ 1, 1, 0x0080 }, /* P1: Port 1      (OC1) */
			{ 1, 0, 0x0040 }, /* P2: MINIPCIE1   (no OC) */
			{ 1, 0, 0x0040 }, /* P3: MMC         (no OC) */
			{ 0, 0, 0x0000 }, /* P4: Empty */
			{ 0, 0, 0x0000 }, /* P5: Empty */
			{ 0, 0, 0x0000 }, /* P6: Empty */
			{ 0, 0, 0x0000 }, /* P7: Empty */
			{ 1, 4, 0x0040 }, /* P8: MINIPCIE2   (no OC) */
			{ 0, 4, 0x0000 }, /* P9: Empty */
			{ 0, 4, 0x0000 }, /* P10: Empty */
			{ 1, 4, 0x0040 }, /* P11: Camera     (no OC) */
			{ 0, 4, 0x0000 }, /* P12: Empty */
			{ 0, 4, 0x0000 }, /* P13: Empty */
		},
	};

	typedef const uint8_t spd_blob[256];
	struct cbfs_file *spd_file;
	spd_blob *spd_data;


	timestamp_init(get_initial_timestamp());
	timestamp_add_now(TS_START_ROMSTAGE);

	if (bist == 0)
		enable_lapic();

	pch_enable_lpc();

	/* Enable GPIOs */
	pci_write_config32(PCH_LPC_DEV, GPIO_BASE, DEFAULT_GPIOBASE|1);
	pci_write_config8(PCH_LPC_DEV, GPIO_CNTL, 0x10);
	setup_pch_gpios(&lumpy_gpio_map);

	console_init();

#if CONFIG_CHROMEOS
	save_chromeos_gpios();
#endif

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	if (MCHBAR16(SSKPD) == 0xCAFE) {
		printk(BIOS_DEBUG, "soft reset detected\n");
		boot_mode = 1;

		/* System is not happy after keyboard reset... */
		printk(BIOS_DEBUG, "Issuing CF9 warm reset\n");
		outb(0x6, 0xcf9);
		hlt();
	}

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

	u32 gp_lvl2 = inl(DEFAULT_GPIOBASE + 0x38);
	u8 gpio33, gpio41, gpio49;
	gpio33 = (gp_lvl2 >> (33-32)) & 1;
	gpio41 = (gp_lvl2 >> (41-32)) & 1;
	gpio49 = (gp_lvl2 >> (49-32)) & 1;
	printk(BIOS_DEBUG, "Memory Straps:\n");
	printk(BIOS_DEBUG, " - memory capacity %dGB\n",
		gpio33 ? 2 : 1);
	printk(BIOS_DEBUG, " - die revision %d\n",
		gpio41 ? 2 : 1);
	printk(BIOS_DEBUG, " - vendor %s\n",
		gpio49 ? "Samsung" : "Other");

	int spd_index = 0;

	switch ((gpio49 << 2) | (gpio41 << 1) | gpio33) {
	case 0: // Other 1G Rev 1
		spd_index = 0;
		break;
	case 2: // Other 1G Rev 2
		spd_index = 1;
		break;
	case 1: // Other 2G Rev 1
	case 3: // Other 2G Rev 2
		spd_index = 2;
		break;
	case 4: // Samsung 1G Rev 1
		spd_index = 3;
		break;
	case 6: // Samsung 1G Rev 2
		spd_index = 4;
		break;
	case 5: // Samsung 2G Rev 1
	case 7: // Samsung 2G Rev 2
		spd_index = 5;
		break;
	}

	spd_file = cbfs_get_file(CBFS_DEFAULT_MEDIA, "spd.bin");
	if (!spd_file)
		die("SPD data not found.");
	if (spd_file->len < (spd_index + 1) * 256)
		die("Missing SPD data.");
	spd_data = (spd_blob *)CBFS_SUBHEADER(spd_file);
	// leave onboard dimm address at f0, and copy spd data there.
	memcpy(pei_data.spd_data[0], spd_data[spd_index], 256);

	post_code(0x39);
	pei_data.boot_mode = boot_mode;
	timestamp_add_now(TS_BEFORE_INITRAM);
	sdram_initialize(&pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);
	post_code(0x3a);
	/* Perform some initialization that must run before stage2 */
	early_pch_init();
	post_code(0x3b);

	rcba_config();
	post_code(0x3c);

	quick_ram_check();
	post_code(0x3e);

	MCHBAR16(SSKPD) = 0xCAFE;

	cbmem_was_initted = !cbmem_initialize();

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
#if CONFIG_CHROMEOS
	init_chromeos(boot_mode);
#endif
	timestamp_add_now(TS_END_ROMSTAGE);
}
