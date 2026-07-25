/* SPDX-License-Identifier: GPL-2.0-only */

#include <common/hda.h>
#include <option.h>
#include <soc/meminit.h>
#include <soc/romstage.h>
#include <types.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	const struct mb_cfg mem_config = {
		.type = MEM_TYPE_DDR5,
		.ect = true,
		.UserBd = BOARD_TYPE_ULT_ULX,
	};

	const bool half_populated = false;

	const struct mem_spd ddr5_spd_info = {
		.topo = MEM_TOPO_DIMM_MODULE,
		.smbus = {
			[0] = {
				.addr_dimm[0] = 0x50,
			},
			[1] = {
				.addr_dimm[0] = 0x52,
			},
		},
	};

	memcfg_init(mupd, &mem_config, &ddr5_spd_info, half_populated);

	mupd->FspmConfig.PchHdaDspEnable = get_uint_option("hda_dsp", 1);

	const uint8_t vtd = get_uint_option("vtd", 1);
	mupd->FspmConfig.VtdDisable = !vtd;

	/* Enable/Disable WiFi (RP09) based on CMOS settings */
	if (get_uint_option("wifi", 1) == 0)
		mupd->FspmConfig.PcieRpEnableMask &= ~(1 << 8);

	mupd->FspmConfig.PchHdaSubSystemIds =
		STARLABS_HDA_PCI_SUBSYSTEM_ID(STARLABS_HDA_POLICY_ALC269_VC3);
	mupd->FspmConfig.PsysPmax = 0;
};
