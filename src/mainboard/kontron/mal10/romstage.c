/* SPDX-License-Identifier: GPL-2.0-only */

#include <string.h>
#include <soc/romstage.h>
#include <FspmUpd.h>

void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	mupd->FspmConfig.Package               = 0;
	mupd->FspmConfig.Profile               = 0x19;
	mupd->FspmConfig.MemoryDown            = 0;
	mupd->FspmConfig.DDR3LPageSize         = 2;
	mupd->FspmConfig.DualRankSupportEnable = 0;
	mupd->FspmConfig.RmtMode               = 0;
	mupd->FspmConfig.MemorySizeLimit       = 0;
	mupd->FspmConfig.DIMM0SPDAddress       = 0xA0;
	mupd->FspmConfig.DIMM1SPDAddress       = 0xA4;

	mupd->FspmConfig.RmtCheckRun = 1;
	mupd->FspmConfig.RmtMarginCheckScaleHighThreshold = 0;
	mupd->FspmConfig.EnhancePort8xhDecoding = 1;

	mupd->FspmConfig.MsgLevelMask  = 0;
	mupd->FspmConfig.MrcDataSaving = 0;
	mupd->FspmConfig.MrcFastBoot   = 1;

	mupd->FspmConfig.PrimaryVideoAdaptor = 2;
}
