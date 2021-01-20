/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#if CONFIG(USE_NATIVE_RAMINIT)
#include <northbridge/intel/sandybridge/raminit_native.h>
#else
#include <northbridge/intel/sandybridge/raminit.h>
#endif
#include <southbridge/intel/bd82x6x/pch.h>

#if !CONFIG(USE_NATIVE_RAMINIT)
void mainboard_fill_pei_data(struct pei_data *pei_data)
{
	struct pei_data pei_data_template = {
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
		.spd_addresses = { 0xa0, 0x00, 0xa2, 0x00 },
		.ts_addresses = { 0x00, 0x00, 0x00, 0x00 },
		.ec_present = 0,
		.gbe_enable = 1,
		// 0 = leave channel enabled
		// 1 = disable dimm 0 on channel
		// 2 = disable dimm 1 on channel
		// 3 = disable dimm 0+1 on channel
		.dimm_channel0_disabled = 2,
		.dimm_channel1_disabled = 2,
		.max_ddr3_freq = 1333,
		.usb_port_config = {
#define USB_CONFIG(enabled, current, ocpin) { enabled, ocpin, 0x040 * current }
#include "usb.h"
		},
	};
	*pei_data = pei_data_template;
}

int mainboard_should_reset_usb(int s3resume)
{
	return !s3resume;
}
#endif
