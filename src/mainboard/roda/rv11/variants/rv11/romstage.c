/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 secunet Security Networks AG
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
#include <device/pci.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/bd82x6x/pch.h>

void pch_enable_lpc(void)
{
	/* Enable KBC on 0x60/0x64 (KBC),
		  EC on 0x62/0x66 (MC) */
	pci_write_config16(PCH_LPC_DEV, LPC_EN,
			   KBC_LPC_EN | MC_LPC_EN);
}

void mainboard_config_superio(void)
{
}

void mainboard_fill_pei_data(struct pei_data *const pei_data)
{
	const struct pei_data pei_data_template = {
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
		.spd_addresses = { 0xA0, 0x00, 0xA4, 0x00 },
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
			/* Enabled / OC PIN / Length */
			{ 1, 0, 0x0040 }, /* P00: 1st            USB3 (OC #0) */
			{ 1, 4, 0x0040 }, /* P01: 2nd            USB3 (OC #4) */
			{ 1, 1, 0x0080 }, /* P02: 1st Multibay   USB3 (OC #1) */
			{ 1, 2, 0x0080 }, /* P03: 2nd Multibay   USB3 (OC #2) */
			{ 1, 8, 0x0040 }, /* P04: MiniPCIe 1     USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P05: MiniPCIe 2     USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P06: MiniPCIe 3     USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P07: GPS            USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P08: MiniPCIe 4     USB2 (no OC) */
			{ 1, 3, 0x0040 }, /* P09: Express Card   USB2 (OC #3) */
			{ 1, 8, 0x0040 }, /* P10: SD card reader USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P11: Sensors Hub?   USB2 (no OC) */
			{ 1, 8, 0x0040 }, /* P12: Touch Screen   USB2 (no OC) */
			{ 1, 5, 0x0040 }, /* P13: reserved?      USB2 (OC #5) */
		},
		.usb3 = {
			.mode =			3,	/* Smart Auto? */
			.hs_port_switch_mask =	0xf,	/* All four ports. */
			.preboot_support =	1,	/* preOS driver? */
			.xhci_streams =		1,	/* Enable. */
		},
		.pcie_init = 1,
	};
	*pei_data = pei_data_template;
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* Enabled / Power / OC PIN */
	{ 1, 0, 0 }, /* P00: 1st            USB3 (OC #0) */
	{ 1, 0, 4 }, /* P01: 2nd            USB3 (OC #4) */
	{ 1, 1, 1 }, /* P02: 1st Multibay   USB3 (OC #1) */
	{ 1, 1, 2 }, /* P03: 2nd Multibay   USB3 (OC #2) */
	{ 1, 0, 8 }, /* P04: MiniPCIe 1     USB2 (no OC) */
	{ 1, 0, 8 }, /* P05: MiniPCIe 2     USB2 (no OC) */
	{ 1, 0, 8 }, /* P06: MiniPCIe 3     USB2 (no OC) */
	{ 1, 0, 8 }, /* P07: GPS            USB2 (no OC) */
	{ 1, 0, 8 }, /* P08: MiniPCIe 4     USB2 (no OC) */
	{ 1, 0, 3 }, /* P09: Express Card   USB2 (OC #3) */
	{ 1, 0, 8 }, /* P10: SD card reader USB2 (no OC) */
	{ 1, 0, 8 }, /* P11: Sensors Hub?   USB2 (no OC) */
	{ 1, 0, 8 }, /* P12: Touch Screen   USB2 (no OC) */
	{ 1, 0, 5 }, /* P13: reserved?      USB2 (OC #5) */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
