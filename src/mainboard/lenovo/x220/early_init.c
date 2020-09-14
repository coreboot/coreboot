/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <acpi/acpi.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	struct pei_data pei_data_template = {
		.pei_version = PEI_VERSION,
		.mchbar = (uintptr_t)DEFAULT_MCHBAR,
		.dmibar = (uintptr_t)DEFAULT_DMIBAR,
		.epbar = DEFAULT_EPBAR,
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
		.spd_addresses = { 0xa0, 0x00,0xa2,0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 1,
		.gbe_enable = 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1333,
		.usb_port_config = {
			{ 1, 0, 0x0040 },
			{ 1, 1, 0x0080 },
			{ 1, 3, 0x0080 },
			{ 1, 3, 0x0080 },
			{ 1, 0, 0x0080 },
			{ 1, 0, 0x0080 },
			{ 1, 2, 0x0040 },
			{ 1, 2, 0x0040 },
			{ 1, 6, 0x0080 },
			{ 1, 5, 0x0080 },
			{ 1, 6, 0x0080 },
			{ 1, 6, 0x0080 },
			{ 1, 7, 0x0080 },
			{ 1, 6, 0x0080 },
		},
	};
	*pei_data = pei_data_template;
}

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd (&spd[0], 0x50, id_only);
	read_spd (&spd[2], 0x51, id_only);
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}
