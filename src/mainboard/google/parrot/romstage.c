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
#include <arch/acpi.h>
#include <console/console.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <arch/cpu.h>
#include <halt.h>
#include <cbfs.h>
#include "ec/compal/ene932/ec.h"

void pch_enable_lpc(void)
{
	/* Parrot EC Decode Range Port60/64, Port62/66 */
	/* Enable EC, PS/2 Keyboard/Mouse */
	pci_write_config16(PCH_LPC_DEV, LPC_EN, KBC_LPC_EN | MC_LPC_EN);

	/* Map EC_IO decode to the LPC bus */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN1_DEC, (EC_IO & ~3) | 0x00040001);

	/* Map EC registers 68/6C decode to the LPC bus */
	pci_write_config32(PCH_LPC_DEV, LPC_GEN2_DEC, (68 & ~3) | 0x00040001);
}

void mainboard_rcba_config(void)
{
	u32 reg32;

	/*
	 *             GFX    INTA -> PIRQA (MSI)
	 * D28IP_P2IP  WLAN   INTA -> PIRQB
	 * D28IP_P3IP  ETH0   INTC -> PIRQD
	 * D29IP_E1P   EHCI1  INTA -> PIRQE
	 * D26IP_E2P   EHCI2  INTA -> PIRQE
	 * D31IP_SIP   SATA   INTA -> PIRQF (MSI)
	 * D31IP_SMIP  SMBUS  INTB -> PIRQG
	 * D31IP_TTIP  THRT   INTC -> PIRQH
	 * D27IP_ZIP   HDA    INTA -> PIRQG (MSI)
	 *
	 * Trackpad DVT PIRQA (16)
	 * Trackpad DVT PIRQE (20)
	 */

	/* Device interrupt pin register (board specific) */
	RCBA32(D31IP) = (INTC << D31IP_TTIP) | (NOINT << D31IP_SIP2) |
		(INTB << D31IP_SMIP) | (INTA << D31IP_SIP);
	RCBA32(D30IP) = (NOINT << D30IP_PIP);
	RCBA32(D29IP) = (INTA << D29IP_E1P);
	RCBA32(D28IP) = (NOINT << D28IP_P1IP) | (INTA << D28IP_P2IP) |
		(INTC << D28IP_P3IP) | (NOINT << D28IP_P4IP) |
		(NOINT << D28IP_P5IP) | (NOINT << D28IP_P6IP) |
		(NOINT << D28IP_P7IP) | (NOINT << D28IP_P8IP);
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
	RCBA16(OIC) = 0x0100;
	/* PCH BWG says to read back the IOAPIC enable register */
	(void) RCBA16(OIC);

	/* Disable unused devices (board specific) */
	reg32 = RCBA32(FD);
	/* Disable PCI bridge so MRC does not probe this bus */
	reg32 |= PCH_DISABLE_P2P;
	RCBA32(FD) = reg32;
}

void mainboard_early_init(int s3resume)
{
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
			/* Empty and onboard Ports 0-7, set to un-used pin OC3 */
			{ 0, 3, 0x0000 }, /* P0: Empty */
			{ 1, 0, 0x0040 }, /* P1: Left USB 1  (OC0) */
			{ 1, 1, 0x0040 }, /* P2: Left USB 2  (OC1) */
			{ 1, 1, 0x0040 }, /* P3: Left USB 3  (OC1) */
			{ 0, 3, 0x0000 }, /* P4: Empty */
			{ 0, 3, 0x0000 }, /* P5: Empty */
			{ 0, 3, 0x0000 }, /* P6: Empty */
			{ 0, 3, 0x0000 }, /* P7: Empty */
			/* Empty and onboard Ports 8-13, set to un-used pin OC4 */
			{ 1, 4, 0x0040 }, /* P8: MiniPCIe (WLAN) (no OC) */
			{ 0, 4, 0x0000 }, /* P9: Empty */
			{ 1, 4, 0x0040 }, /* P10: Camera (no OC) */
			{ 0, 4, 0x0000 }, /* P11: Empty */
			{ 0, 4, 0x0000 }, /* P12: Empty */
			{ 0, 4, 0x0000 }, /* P13: Empty */
		},
	};
	*pei_data = pei_data_template;
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* enabled power  usb oc pin  */
	{ 0, 0, -1 }, /* P0: Empty */
	{ 1, 0, 0 }, /* P1: Left USB 1  (OC0) */
	{ 1, 0, 1 }, /* P2: Left USB 2  (OC1) */
	{ 1, 0, 1 }, /* P3: Left USB 3  (OC1) */
	{ 0, 0, -1 }, /* P4: Empty */
	{ 0, 0, -1 }, /* P5: Empty */
	{ 0, 0, -1 }, /* P6: Empty */
	{ 0, 0, -1 }, /* P7: Empty */
	/* Empty and onboard Ports 8-13, set to un-used pin OC4 */
	{ 1, 0, -1 }, /* P8: MiniPCIe (WLAN) (no OC) */
	{ 0, 0, -1 }, /* P9: Empty */
	{ 1, 0, -1 }, /* P10: Camera (no OC) */
	{ 0, 0, -1 }, /* P11: Empty */
	{ 0, 0, -1 }, /* P12: Empty */
	{ 0, 0, -1 }, /* P13: Empty */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only) {
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}

void mainboard_config_superio(void)
{
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}
