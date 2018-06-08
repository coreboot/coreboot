/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2010 coresystems GmbH
 * Copyright (C) 2011 Google Inc.
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
#include <device/pci.h>
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
#include <southbridge/intel/common/gpio.h>
#include "ec/google/chromeec/ec.h"
#include <arch/cpu.h>
#include <cpu/x86/msr.h>
#include <halt.h>
#include <security/tpm/tspi.h>
#include <cbfs.h>

#include <southbridge/intel/bd82x6x/chip.h>

void pch_enable_lpc(void)
{
	const struct device *lpc;
	const struct southbridge_intel_bd82x6x_config *config = NULL;

	lpc = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	if (!lpc)
		return;
	if (lpc->chip_info)
		config = lpc->chip_info;
	if (!config)
		return;

	/* Set COM1/COM2 decode range */
	pci_write_config16(PCH_LPC_DEV, LPC_IO_DEC, 0x0010);

	/* Enable PS/2 Keyboard/Mouse, EC areas and COM1 */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN | MC_LPC_EN | \
			   GAMEL_LPC_EN | COMA_LPC_EN);

	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, config->gen1_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, config->gen2_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN3_DEC, config->gen3_dec);
	pci_write_config32(PCH_LPC_DEV, LPC_GEN4_DEC, config->gen4_dec);
}

void mainboard_rcba_config(void)
{
	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P3IP  WLAN   INTA -> PIRQB
	 * D29IP_E1P   EHCI1  INTA -> PIRQD
	 * D26IP_E2P   EHCI2  INTA -> PIRQF
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQH
	 * D31IP_TTIP  THRT   INTC -> PIRQA
	 * D27IP_ZIP   HDA    INTA -> PIRQA (MSI)
	 *
	 * TRACKPAD                -> PIRQE (Edge Triggered)
	 * TOUCHSCREEN             -> PIRQG (Edge Triggered)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (INTA << D28IP_P3IP);
	RCBA32(D27IP) = (INTA << D27IP_ZIP);
	RCBA32(D26IP) = (INTA << D26IP_E2P);
	RCBA32(D25IP) = (NOINT << D25IP_LIP);
	RCBA32(D22IP) = (NOINT << D22IP_MEI1IP);

	/* Device interrupt route registers */
	DIR_ROUTE(D31IR, PIRQB, PIRQH, PIRQA, PIRQC);
	DIR_ROUTE(D29IR, PIRQD, PIRQE, PIRQF, PIRQG);
	DIR_ROUTE(D28IR, PIRQB, PIRQC, PIRQD, PIRQE);
	DIR_ROUTE(D27IR, PIRQA, PIRQH, PIRQA, PIRQB);
	DIR_ROUTE(D26IR, PIRQF, PIRQE, PIRQG, PIRQH);
	DIR_ROUTE(D25IR, PIRQA, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D22IR, PIRQA, PIRQB, PIRQC, PIRQD);

	/* Enable IOAPIC (generic) */
	RCBA16(EOIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(EOIC);
}

static uint8_t *locate_spd(void)
{
	const int gpio_vector[] = {41, 42, 43, 10, -1};
	uint8_t *spd_file;
	size_t spd_file_len;
	int spd_index = get_gpios(gpio_vector);

	printk(BIOS_DEBUG, "spd index %d\n", spd_index);
	spd_file = cbfs_boot_map_with_leak("spd.bin", CBFS_TYPE_SPD,
						&spd_file_len);
	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < ((spd_index + 1) * 256)) {
		printk(BIOS_ERR, "spd index override to 0 - old hardware?\n");
		spd_index = 0;
	}

	if (spd_file_len < 256)
		die("Missing SPD data.");

	return spd_file + spd_index * 256;
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
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		.ddr3lv_support = 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1600,
		.usb_port_config = {
			/* Empty and onboard Ports 0-7, set to un-used pin OC3 */
			{ 0, 3, 0x0000 }, /* P0: Empty */
			{ 1, 0, 0x0040 }, /* P1: Left USB 1  (OC0) */
			{ 1, 1, 0x0040 }, /* P2: Left USB 2  (OC1) */
			{ 1, 3, 0x0040 }, /* P3: SDCARD      (no OC) */
			{ 0, 3, 0x0000 }, /* P4: Empty */
			{ 1, 3, 0x0040 }, /* P5: WWAN        (no OC) */
			{ 0, 3, 0x0000 }, /* P6: Empty */
			{ 0, 3, 0x0000 }, /* P7: Empty */
			/* Empty and onboard Ports 8-13, set to un-used pin OC4 */
			{ 1, 4, 0x0040 }, /* P8: Camera      (no OC) */
			{ 1, 4, 0x0040 }, /* P9: Bluetooth   (no OC) */
			{ 0, 4, 0x0000 }, /* P10: Empty */
			{ 0, 4, 0x0000 }, /* P11: Empty */
			{ 0, 4, 0x0000 }, /* P12: Empty */
			{ 0, 4, 0x0000 }, /* P13: Empty */
		},
	};
	*pei_data = pei_data_template;
	memcpy(pei_data->spd_data[0], locate_spd(),
	       sizeof(pei_data->spd_data[0]));
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  usb oc pin  */
	{ 0, 0, -1 }, /* P0: Empty */
	{ 1, 0, 0 }, /* P1: Left USB 1  (OC0) */
	{ 1, 0, 1 }, /* P2: Left USB 2  (OC1) */
	{ 1, 0, -1 }, /* P3: SDCARD      (no OC) */
	{ 0, 0, -1 }, /* P4: Empty */
	{ 1, 0, -1 }, /* P5: WWAN        (no OC) */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 0, 0, -1 }, /* P7: Empty */
	{ 1, 0, -1 }, /* P8: Camera      (no OC) */
	{ 1, 0, -1 }, /* P9: Bluetooth   (no OC) */
	{ 0, 0, -1 }, /* P10: Empty */
	{ 0, 0, -1 }, /* P11: Empty */
	{ 0, 0, -1 }, /* P12: Empty */
	{ 0, 0, -1 }, /* P13: Empty */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only) {
	memcpy(&spd[0], locate_spd(), 128);
}

void mainboard_early_init(int s3resume)
{
	if (!s3resume) {
		/* This is the fastest way to let users know
		 * the Intel CPU is now alive.
		 */
		google_chromeec_kbbacklight(100);
	}
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}

void mainboard_config_superio(void)
{
}
