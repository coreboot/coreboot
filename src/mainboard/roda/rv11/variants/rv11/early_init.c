/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_fill_pei_data(struct pei_data *const pei_data)
{
	const struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar = CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar = CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar = CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar = CONFIG_FIXED_SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = CONFIG_HPET_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBA,
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
