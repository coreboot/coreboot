/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>

#define DUMP_UPD(old, new, field) \
	fsp_display_upd_value(#field, sizeof(old->field), old->field, new->field)

/* Display the UPD parameters for MemoryInit */
void soc_display_fspm_upd_params(
	const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *new;
	const FSP_M_CONFIG *old;

	old = &fspm_old_upd->FspmConfig;
	new = &fspm_new_upd->FspmConfig;

	printk(BIOS_DEBUG, "UPD values for MemoryInit:\n");

	DUMP_UPD(old, new, PcdFspMrcDebugPrintErrorLevel);
	DUMP_UPD(old, new, PcdFspKtiDebugPrintErrorLevel);
	DUMP_UPD(old, new, PcdHsuartDevice);

	hexdump(fspm_new_upd, sizeof(*fspm_new_upd));
}

/* Display the UPD parameters for SiliconInit */
void soc_display_fsps_upd_params(
	const FSPS_UPD *fsps_old_upd,
	const FSPS_UPD *fsps_new_upd)
{
	const FSP_S_CONFIG *new;
	const FSP_S_CONFIG *old;

	old = &fsps_old_upd->FspsConfig;
	new = &fsps_new_upd->FspsConfig;

	printk(BIOS_DEBUG, "UPD values for SiliconInit:\n");

	DUMP_UPD(old, new, PcdBifurcationPcie0);
	DUMP_UPD(old, new, PcdBifurcationPcie1);
	DUMP_UPD(old, new, PcdActiveCoreCount);
	DUMP_UPD(old, new, PcdCpuMicrocodePatchBase);
	DUMP_UPD(old, new, PcdCpuMicrocodePatchSize);
	DUMP_UPD(old, new, PcdEnablePcie0);
	DUMP_UPD(old, new, PcdEnablePcie1);
	DUMP_UPD(old, new, PcdEnableEmmc);
	DUMP_UPD(old, new, PcdEnableGbE);
	DUMP_UPD(old, new, PcdFiaMuxConfigRequestPtr);
	DUMP_UPD(old, new, PcdPcieRootPort0DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort1DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort2DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort3DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort4DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort5DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort6DeEmphasis);
	DUMP_UPD(old, new, PcdPcieRootPort7DeEmphasis);
	DUMP_UPD(old, new, PcdEMMCDLLConfigPtr);

	hexdump(fsps_new_upd, sizeof(*fsps_new_upd));
}
