/* SPDX-License-Identifier: GPL-2.0-only */

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

	DUMP_UPD(old, new, DebugPrintLevel);
	DUMP_UPD(old, new, PchAdrEn);

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

	DUMP_UPD(old, new, BifurcationPcie0);
	DUMP_UPD(old, new, BifurcationPcie1);
	DUMP_UPD(old, new, ActiveCoreCount);
	DUMP_UPD(old, new, CpuMicrocodePatchBase);
	DUMP_UPD(old, new, CpuMicrocodePatchSize);
	DUMP_UPD(old, new, EnablePcie0);
	DUMP_UPD(old, new, EnablePcie1);
	DUMP_UPD(old, new, EnableEmmc);
	DUMP_UPD(old, new, EnableGbE);
	DUMP_UPD(old, new, FiaMuxConfigRequestPtr);
	DUMP_UPD(old, new, PcieRootPort0DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort1DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort2DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort3DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort4DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort5DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort6DeEmphasis);
	DUMP_UPD(old, new, PcieRootPort7DeEmphasis);
	DUMP_UPD(old, new, EMMCDLLConfigPtr);

	hexdump(fsps_new_upd, sizeof(*fsps_new_upd));
}
