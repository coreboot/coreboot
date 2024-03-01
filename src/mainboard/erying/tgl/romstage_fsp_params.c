/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/meminit.h>
#include <soc/romstage.h>

static const struct mb_cfg mem_config = {
	.type = MEM_TYPE_DDR4,
	.ddr4_config = { .dq_pins_interleaved = true },
};

static const struct mem_spd spd_info = {
	.topo = MEM_TOPO_DIMM_MODULE,
	.smbus = {
		[0] = { .addr_dimm[0] = 0x50, },
		[1] = { .addr_dimm[0] = 0x52, },
	},
};

const bool half_populated = false;

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	// Performance settings
	mupd->FspmConfig.EnableAbove4GBMmio = 1;

	// iGPU
	mupd->FspmConfig.GttSize = 3; // 8MB
	mupd->FspmConfig.ApertureSize = 3; // 512MB
	mupd->FspmConfig.GtPsmiSupport = 0;
	mupd->FspmConfig.IgdDvmt50PreAlloc = 2; // 64MB

	// DMI (SoC - PCH) Link settings
	mupd->FspmConfig.DmiMaxLinkSpeed = 3;
	mupd->FspmConfig.DmiAspm = 0;
	mupd->FspmConfig.DmiAspmCtrl = 0;

	// Memory settings/training - based on stock
	mupd->FspmConfig.SpdProfileSelected = 0; // Default profile
	mupd->FspmConfig.RefClk = 0; // 133MHz
	mupd->FspmConfig.VddVoltage = 1350; // 1.35V
	mupd->FspmConfig.McPllVoltageOffset = 6; // Bump MC VCC by offset of 6
	mupd->FspmConfig.Ratio = 0; // Auto
	mupd->FspmConfig.RingDownBin = 1;
	mupd->FspmConfig.GearRatio = 1; // Gear 1
	mupd->FspmConfig.ECT = 1;
	mupd->FspmConfig.LCT = 1;
	mupd->FspmConfig.SOT = 1;
	mupd->FspmConfig.ERDMPRTC2D = 0;
	mupd->FspmConfig.RDMPRT = 1;
	mupd->FspmConfig.RCVET = 1;
	mupd->FspmConfig.JWRL = 1;
	mupd->FspmConfig.EWRTC2D = 1;
	mupd->FspmConfig.ERDTC2D = 1;
	mupd->FspmConfig.WRTC2D = 1;
	mupd->FspmConfig.WRVC1D = 1;
	mupd->FspmConfig.DIMMODTT = 1;
	mupd->FspmConfig.DIMMRONT = 1;
	mupd->FspmConfig.WRDSEQT = 1;
	mupd->FspmConfig.WRSRT = 0;
	mupd->FspmConfig.RDODTT = 1;
	mupd->FspmConfig.RDEQT = 1;
	mupd->FspmConfig.RDAPT = 1;
	mupd->FspmConfig.RDTC2D = 1;
	mupd->FspmConfig.WRVC2D = 1;
	mupd->FspmConfig.RDVC2D = 1;
	mupd->FspmConfig.CMDVC = 1;
	mupd->FspmConfig.MrcSafeConfig = 0;
	mupd->FspmConfig.LpDdrDqDqsReTraining = 1;
	mupd->FspmConfig.SafeMode = 0;
	mupd->FspmConfig.MemTestOnWarmBoot = 1;
	mupd->FspmConfig.DdrFreqLimit = 3200; // Maximum tested speed.

	memcfg_init(mupd, &mem_config, &spd_info, half_populated);
}
