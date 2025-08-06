/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg mem_config = {
	.type = MEM_TYPE_DDR5,
	.user_bd = BOARD_TYPE_ULT_ULX,
	.ect = true,

	.rcomp = {
		.resistor = 100,
	},

	.ddr_config = {
		.dq_pins_interleaved = false,
	}
};

static const struct mem_spd spd_info = {
	.topo = MEM_TOPO_DIMM_MODULE,
	.smbus = {
		[0] = {
			.addr_dimm[0] = 0x50,
			.addr_dimm[1] = 0x0,
		},

		[1] = {
			.addr_dimm[0] = 0x50,
			.addr_dimm[1] = 0x0,
		},

		[2] = {
			.addr_dimm[0] = 0x52,
			.addr_dimm[1] = 0x0,
		},

		[3] = {
			.addr_dimm[0] = 0x52,
			.addr_dimm[1] = 0x0,
		},
	},
};

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const uint8_t channel_to_ckd_qck[] = { 0, 0, 1, 0, 0, 0, 1, 0 };
	memcpy(mupd->FspmConfig.ChannelToCkdQckMapping,
		channel_to_ckd_qck, sizeof(channel_to_ckd_qck));

	const uint8_t phy_clock_to_ckd_dimm[] = { 0, 0, 0, 0, 8, 0, 8, 0 };
	memcpy(mupd->FspmConfig.PhyClockToCkdDimm,
		phy_clock_to_ckd_dimm, sizeof(phy_clock_to_ckd_dimm));


	// Raise FSP loglevel for verbose debugging. Requires debug build (NDA).
	//	mupd->FspmConfig.PcdSerialDebugLevel = 4;

	// GpioOverride needs to be disabled, messes with PCIe CLKREQs
	mupd->FspmConfig.GpioOverride = 0;
	mupd->FspmConfig.EnableAbove4GBMmio = 1;

	memcfg_init(mupd, &mem_config, &spd_info, false);
}
