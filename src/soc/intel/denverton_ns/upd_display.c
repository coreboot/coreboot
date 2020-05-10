/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <fsp/util.h>
#include <lib.h>

/* Display the UPD parameters for MemoryInit */
void soc_display_fspm_upd_params(
	const FSPM_UPD *fspm_old_upd,
	const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *new;
	const FSP_M_CONFIG *old;

	old = &fspm_old_upd->FspmConfig;
	new = &fspm_new_upd->FspmConfig;

	printk(BIOS_SPEW, "UPD values for MemoryInit:\n");

	#define DISPLAY_UPD(field) \
		fsp_display_upd_value(#field, sizeof(old->field), \
			old->field, new->field)

	DISPLAY_UPD(PcdSmmTsegSize);
	DISPLAY_UPD(PcdFspDebugPrintErrorLevel);
	DISPLAY_UPD(PcdSpdSmbusAddress_0_0);
	DISPLAY_UPD(PcdSpdSmbusAddress_0_1);
	DISPLAY_UPD(PcdSpdSmbusAddress_1_0);
	DISPLAY_UPD(PcdSpdSmbusAddress_1_1);
	DISPLAY_UPD(PcdMrcRmtSupport);
	DISPLAY_UPD(PcdMrcRmtCpgcExpLoopCntValue);
	DISPLAY_UPD(PcdMrcRmtCpgcNumBursts);
	DISPLAY_UPD(PcdMemoryPreservation);
	DISPLAY_UPD(PcdFastBoot);
	DISPLAY_UPD(PcdEccSupport);
	DISPLAY_UPD(PcdHsuartDevice);
	DISPLAY_UPD(PcdMemoryDown);
	DISPLAY_UPD(PcdEnableSATA0);
	DISPLAY_UPD(PcdEnableSATA1);
	DISPLAY_UPD(PcdEnableIQAT);
	DISPLAY_UPD(PcdSmbusSpdWriteDisable);
	DISPLAY_UPD(PcdEnableMeShutdown);
	DISPLAY_UPD(PcdEnableXhci);
	DISPLAY_UPD(PcdDdrFreq);
	DISPLAY_UPD(PcdMmioSize);
	DISPLAY_UPD(PcdMeHeciCommunication);
	DISPLAY_UPD(PcdHsioLanesNumber);
	DISPLAY_UPD(PcdFiaMuxConfigPtr);
	DISPLAY_UPD(PcdHalfWidthEnable);
	DISPLAY_UPD(PcdTclIdle);
	DISPLAY_UPD(PcdInterleaveMode);
	DISPLAY_UPD(PcdMemoryThermalThrottling);
	DISPLAY_UPD(PcdSkipMemoryTest);
	DISPLAY_UPD(PcdUsb2Port1Pin);
	DISPLAY_UPD(PcdUsb2Port2Pin);
	DISPLAY_UPD(PcdUsb2Port3Pin);
	DISPLAY_UPD(PcdUsb2Port4Pin);
	DISPLAY_UPD(PcdUsb3Port1Pin);
	DISPLAY_UPD(PcdUsb3Port2Pin);
	DISPLAY_UPD(PcdUsb3Port3Pin);
	DISPLAY_UPD(PcdUsb3Port4Pin);
	DISPLAY_UPD(PcdIOxAPIC0_199);
	DISPLAY_UPD(PcdDmapX16);

	#undef DISPLAY_UPD

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

	printk(BIOS_SPEW, "UPD values for SiliconInit:\n");

	#define DISPLAY_UPD(field) \
		fsp_display_upd_value(#field, sizeof(old->field), \
			old->field, new->field)

	DISPLAY_UPD(PcdBifurcationPcie0);
	DISPLAY_UPD(PcdBifurcationPcie1);
	DISPLAY_UPD(PcdActiveCoreCount);
	DISPLAY_UPD(PcdCpuMicrocodePatchBase);
	DISPLAY_UPD(PcdCpuMicrocodePatchSize);
	DISPLAY_UPD(PcdEnablePcie0);
	DISPLAY_UPD(PcdEnablePcie1);
	DISPLAY_UPD(PcdEnableEmmc);
	DISPLAY_UPD(PcdEnableGbE);
	DISPLAY_UPD(PcdFiaMuxConfigRequestPtr);
	DISPLAY_UPD(PcdPcieRootPort0DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort1DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort2DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort3DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort4DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort5DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort6DeEmphasis);
	DISPLAY_UPD(PcdPcieRootPort7DeEmphasis);
	DISPLAY_UPD(PcdEMMCDLLConfigPtr);
	DISPLAY_UPD(PcdPcieRootPort0LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort1LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort2LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort3LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort4LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort5LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort6LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort7LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort0Aspm);
	DISPLAY_UPD(PcdPcieRootPort1Aspm);
	DISPLAY_UPD(PcdPcieRootPort2Aspm);
	DISPLAY_UPD(PcdPcieRootPort3Aspm);
	DISPLAY_UPD(PcdPcieRootPort4Aspm);
	DISPLAY_UPD(PcdPcieRootPort5Aspm);
	DISPLAY_UPD(PcdPcieRootPort6Aspm);
	DISPLAY_UPD(PcdPcieRootPort7Aspm);

	#undef DISPLAY_UPD

	hexdump(fsps_new_upd, sizeof(*fsps_new_upd));
}
