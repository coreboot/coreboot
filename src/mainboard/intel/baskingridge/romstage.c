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
#include <arch/romcc_io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <cbmem.h>
#include <console/console.h>
#include "northbridge/intel/haswell/haswell.h"
#include "northbridge/intel/haswell/raminit.h"
#include "southbridge/intel/lynxpoint/pch.h"
#include "southbridge/intel/lynxpoint/me.h"
#include <arch/cpu.h>
#include <cpu/x86/bist.h>
#include <cpu/x86/msr.h>
#include "gpio.h"
#if CONFIG_CHROMEOS
#include <vendorcode/google/chromeos/chromeos.h>
#endif

const struct rcba_config_instruction rcba_config[] = {
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P1IP  WLAN   INTA -> PIRQB
	 * D28IP_P4IP  ETH0   INTB -> PIRQC
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQE
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQH
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA_SET_REG_32(D31IP, (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		               (INTB << D31IP_SMIP) | (INTA << D31IP_SIP)),
	RCBA_SET_REG_32(D30IP, (NOINT << D30IP_PIP)),
	RCBA_SET_REG_32(D29IP, (INTA << D29IP_E1P)),
	RCBA_SET_REG_32(D28IP, (INTA << D28IP_P1IP) | (INTC << D28IP_P3IP) |
	                       (INTB << D28IP_P4IP)),
	RCBA_SET_REG_32(D27IP, (INTA << D27IP_ZIP)),
	RCBA_SET_REG_32(D26IP, (INTA << D26IP_E2P)),
	RCBA_SET_REG_32(D25IP, (NOINT << D25IP_LIP)),
	RCBA_SET_REG_32(D22IP, (NOINT << D22IP_MEI1IP)),

	/* Device interrupt route registers */
	RCBA_SET_REG_32(D31IR, DIR_ROUTE(PIRQF, PIRQG, PIRQH, PIRQA)),
	RCBA_SET_REG_32(D29IR, DIR_ROUTE(PIRQD, PIRQE, PIRQF, PIRQG)),
	RCBA_SET_REG_32(D28IR, DIR_ROUTE(PIRQB, PIRQC, PIRQD, PIRQE)),
	RCBA_SET_REG_32(D27IR, DIR_ROUTE(PIRQG, PIRQH, PIRQA, PIRQB)),
	RCBA_SET_REG_32(D26IR, DIR_ROUTE(PIRQE, PIRQF, PIRQG, PIRQH)),
	RCBA_SET_REG_32(D25IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),
	RCBA_SET_REG_32(D22IR, DIR_ROUTE(PIRQA, PIRQB, PIRQC, PIRQD)),

	/* Disable unused devices (board specific) */
	RCBA_RMW_REG_32(FD, ~0, PCH_DISABLE_ALWAYS),

	RCBA_END_CONFIG,
};

void main(unsigned long bist)
{
	int boot_mode = 0;
	int wake_from_s3;
	int cbmem_was_initted;

#if CONFIG_COLLECT_TIMESTAMPS
	tsc_t start_romstage_time;
	tsc_t before_dram_time;
	tsc_t after_dram_time;
	tsc_t base_time = {
		.lo = pci_read_config32(PCI_DEV(0, 0x00, 0), 0xdc),
		.hi = pci_read_config32(PCI_DEV(0, 0x1f, 2), 0xd0)
	};
#endif
	struct pei_data pei_data = {
		pei_version: PEI_VERSION,
		mchbar: DEFAULT_MCHBAR,
		dmibar: DEFAULT_DMIBAR,
		epbar: DEFAULT_EPBAR,
		pciexbar: DEFAULT_PCIEXBAR,
		smbusbar: SMBUS_IO_BASE,
		wdbbar: 0x4000000,
		wdbsize: 0x1000,
		hpet_address: HPET_ADDR,
		rcba: DEFAULT_RCBA,
		pmbase: DEFAULT_PMBASE,
		gpiobase: DEFAULT_GPIOBASE,
		temp_mmio_base: 0xfed08000,
		system_type: 0, // 0 Mobile, 1 Desktop/Server
		tseg_size: CONFIG_SMM_TSEG_SIZE,
		spd_addresses: { 0xa0, 0xa2, 0xa4, 0xa6 },
		ec_present: 0,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		dimm_channel0_disabled: 0,
		dimm_channel1_disabled: 0,
		max_ddr3_freq: 1600,
		usb_port_config: {
			{ 1, 0, 0x0040 }, /* P0: Back USB3 port  (OC0) */
			{ 1, 0, 0x0040 }, /* P1: Back USB3 port  (OC0) */
			{ 1, 1, 0x0040 }, /* P2: Flex Port on bottom (OC1) */
			{ 1, 8, 0x0040 }, /* P3: Docking connector (no OC) */
			{ 1, 8, 0x0040 }, /* P4: Mini PCIE (no OC) */
			{ 1, 1, 0x0040 }, /* P5: USB eSATA header (OC1) */
			{ 1, 3, 0x0040 }, /* P6: Front Header J8H2 (OC3) */
			{ 1, 3, 0x0040 }, /* P7: Front Header J8H2 (OC3) */
			{ 1, 4, 0x0040 }, /* P8: USB/LAN Jack (OC4) */
			{ 1, 4, 0x0040 }, /* P9: USB/LAN Jack (OC4) */
			{ 1, 5, 0x0040 }, /* P10: Front Header J7H3 (OC5) */
			{ 1, 5, 0x0040 }, /* P11: Front Header J7H3 (OC5) */
			{ 1, 6, 0x0040 }, /* P12: USB/DP Jack (OC6) */
			{ 1, 6, 0x0040 }, /* P13: USB/DP Jack (OC6) */
		},
	};

#if CONFIG_COLLECT_TIMESTAMPS
	start_romstage_time = rdtsc();
#endif

	if (bist == 0)
		enable_lapic();

	wake_from_s3 = early_pch_init(&mainboard_gpio_map, &rcba_config[0]);

	/* Halt if there was a built in self test failure */
	report_bist_failure(bist);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization(HASWELL_MOBILE);
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if CONFIG_HAVE_ACPI_RESUME
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
		boot_mode = 2;
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
#endif
	}

	/* Prepare USB controller early in S3 resume */
	if (boot_mode == 2)
		enable_usb_bar();

	post_code(0x3a);
	pei_data.boot_mode = boot_mode;
#if CONFIG_COLLECT_TIMESTAMPS
	before_dram_time = rdtsc();
#endif

	report_platform_info();

	sdram_initialize(&pei_data);

#if CONFIG_COLLECT_TIMESTAMPS
	after_dram_time = rdtsc();
#endif
	post_code(0x3b);

	intel_early_me_status();

	quick_ram_check();
	post_code(0x3e);

	MCHBAR16(SSKPD) = 0xCAFE;
#if CONFIG_EARLY_CBMEM_INIT
	cbmem_was_initted = !cbmem_initialize();
#else
	cbmem_was_initted = cbmem_reinit((uint64_t) (get_top_of_ram()
						     - HIGH_MEMORY_SIZE));
#endif

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
		while (1) {
			hlt();
		}
	} else {
		pci_write_config32(PCI_DEV(0, 0x00, 0), SKPAD, 0xcafebabe);
	}
#endif
	post_code(0x3f);
#if CONFIG_CHROMEOS
	init_chromeos(boot_mode);
#endif
#if CONFIG_COLLECT_TIMESTAMPS
	timestamp_init(base_time);
	timestamp_add(TS_START_ROMSTAGE, start_romstage_time );
	timestamp_add(TS_BEFORE_INITRAM, before_dram_time );
	timestamp_add(TS_AFTER_INITRAM, after_dram_time );
	timestamp_add_now(TS_END_ROMSTAGE);
#endif
#if CONFIG_CONSOLE_CBMEM
	/* Keep this the last thing this function does. */
	cbmemc_reinit();
#endif
}
