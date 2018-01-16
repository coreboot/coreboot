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
 */

#include <stdint.h>
#include <string.h>
#include <lib.h>
#include <timestamp.h>
#include <arch/io.h>
#include <device/pci_def.h>
#include <device/pnp_def.h>
#include <cpu/x86/lapic.h>
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <cbmem.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <bootmode.h>
#include <security/tpm/tis.h>
#include <cbfs.h>
#include <ec/quanta/it8518/ec.h>
#include "ec.h"
#include "onboard.h"

void pch_enable_lpc(void)
{
	/*
	 * Enable:
	 *  EC Decode Range Port62/66
	 *  SuperIO Port2E/2F
	 *  PS/2 Keyboard/Mouse Port60/64
	 *  FDD Port3F0h-3F5h and Port3F7h
	 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN | MC_LPC_EN |
			CNF1_LPC_EN | FDD_LPC_EN);

	/* Stout EC Decode Range Port68/6C */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, (0x68 | 0x40001));
}

void mainboard_rcba_config(void)
{
	u32 reg32;

	/*
	 *             GFX         INTA -> PIRQA (MSI)
	 * D20IP_XHCI  XHCI        INTA -> PIRQD (MSI)
	 * D26IP_E2P   EHCI #2     INTA -> PIRQF
	 * D27IP_ZIP   HDA         INTA -> PIRQA (MSI)
	 * D28IP_P2IP  WLAN        INTA -> PIRQD
	 * D28IP_P3IP  Card Reader INTB -> PIRQE
	 * D28IP_P6IP  LAN         INTC -> PIRQB
	 * D29IP_E1P   EHCI #1     INTA -> PIRQD
	 * D31IP_SIP   SATA        INTA -> PIRQB (MSI)
	 * D31IP_SMIP  SMBUS       INTB -> PIRQH
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (NOINT << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (NOINT << D28IP_P1IP) | (INTA << D28IP_P2IP) |
		(INTB << D28IP_P3IP) | (NOINT << D28IP_P4IP) |
		(NOINT << D28IP_P5IP) | (INTC << D28IP_P6IP) |
		(NOINT << D28IP_P7IP) | (NOINT << D28IP_P8IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);
	RCBA32(D20IP) = (INTA << D20IP_XHCIIP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQD, PIRQE, PIRQB, PIRQC);
	DIR_ROUTE(D27IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQF, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D20IR, PIRQD, PIRQE, PIRQF, PIRQG);

	/* Enable IOAPIC (generic) */
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	/* Disable PCI bridge so MRC does not probe this bus */
	reg32 |= PCH_DISABLE_P2P;
	RCBA32(FD) = reg32;
}

	/*
	 * The Stout EC needs to be reset to RW mode. It is important that
	 * the RTC_PWR_STS is not set until ramstage EC init.
	 */
static void early_ec_init(void)
{
	u8 ec_status = ec_read(EC_STATUS_REG);
	int rec_mode = get_recovery_mode_switch();

	if (((ec_status & 0x3) == EC_IN_RO_MODE) ||
	    ((ec_status & 0x3) == EC_IN_RECOVERY_MODE)) {

		printk(BIOS_DEBUG, "EC Cold Boot Detected\n");
		if (!rec_mode) {
			/*
			 * Tell EC to exit RO mode
			 */
			printk(BIOS_DEBUG, "EC will exit RO mode and boot normally\n");
			ec_write_cmd(EC_CMD_EXIT_BOOT_BLOCK);
			die("wait for ec to reset");
		}
	} else {
		printk(BIOS_DEBUG, "EC Warm Boot Detected\n");
		ec_write_cmd(EC_CMD_WARM_RESET);
	}
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = (uintptr_t)DEFAULT_MCHBAR,
		.dmibar = (uintptr_t)DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBABASE,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.system_type = 0, // 0 Mobile, 1 Desktop/Server
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.spd_addresses = { 0xA0, 0x00,0xA4,0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			/* enabled   usb oc pin    length */
			{ 1, 0, 0x0040 }, /* P0: USB 3.0 1  (OC0) */
			{ 1, 0, 0x0040 }, /* P1: USB 3.0 2  (OC0) */
			{ 0, 1, 0x0000 }, /* P2: Empty */
			{ 1, 1, 0x0040 }, /* P3: Camera (no OC) */
			{ 1, 1, 0x0040 }, /* P4: WLAN   (no OC) */
			{ 1, 1, 0x0040 }, /* P5: WWAN   (no OC) */
			{ 0, 1, 0x0000 }, /* P6: Empty */
			{ 0, 1, 0x0000 }, /* P7: Empty */
			{ 0, 5, 0x0000 }, /* P8: Empty */
			{ 1, 4, 0x0040 }, /* P9: USB 2.0 (AUO4) (OC4) */
			{ 0, 5, 0x0000 }, /* P10: Empty */
			{ 0, 5, 0x0000 }, /* P11: Empty */
			{ 0, 5, 0x0000 }, /* P12: Empty */
			{ 1, 5, 0x0040 }, /* P13: Bluetooth (no OC) */
		},
		.usb3 = {
			.mode = XHCI_MODE,
			.hs_port_switch_mask = XHCI_PORTS,
			.preboot_support = XHCI_PREBOOT,
			.xhci_streams = XHCI_STREAMS,
		},
	};
	*pei_data = pei_data_template;
}

void mainboard_early_init(int s3resume)
{
	/* Do ec reset as early as possible, but skip it on S3 resume */
	if (!s3resume) {
		early_ec_init();
	}
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}

void mainboard_config_superio(void)
{
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled   usb oc pin    length */
	{1, 0, 0},  /* P0: USB 3.0 1  (OC0) */
	{1, 0, 0},  /* P1: USB 3.0 2  (OC0) */
	{0, 0, 0},  /* P2: Empty */
	{1, 0, -1}, /* P3: Camera (no OC) */
	{1, 0, -1}, /* P4: WLAN   (no OC) */
	{1, 0, -1}, /* P5: WWAN   (no OC) */
	{0, 0, 0},  /* P6: Empty */
	{0, 0, 0},  /* P7: Empty */
	{0, 0, 0},  /* P8: Empty */
	{1, 0, 4},  /* P9: USB 2.0 (AUO4) (OC4) */
	{0, 0, 0},  /* P10: Empty */
	{0, 0, 0},  /* P11: Empty */
	{0, 0, 0},  /* P12: Empty */
	{1, 0, -1}, /* P13: Bluetooth (no OC) */
};
