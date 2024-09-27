/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <fsp/debug.h>
#include <lib.h>

#define DISPLAY_UPD(field) \
	fsp_display_upd_value(#field, sizeof(old->field), old->field, new->field)

void soc_display_fspm_upd_params(const FSPM_UPD *fspm_old_upd, const FSPM_UPD *fspm_new_upd)
{
	const FSP_M_CONFIG *old = &fspm_old_upd->FspmConfig;
	const FSP_M_CONFIG *new = &fspm_new_upd->FspmConfig;

	printk(BIOS_SPEW, "UPD values for FspMemoryInit:\n");

	DISPLAY_UPD(PcdEnableBiosSsaRMT);
	DISPLAY_UPD(PcdEnableBiosSsaRMTonFCB);
	DISPLAY_UPD(PcdBiosSsaPerBitMargining);
	DISPLAY_UPD(PcdBiosSsaDisplayTables);
	DISPLAY_UPD(PcdBiosSsaPerDisplayPlots);
	DISPLAY_UPD(PcdBiosSsaLoopCount);
	DISPLAY_UPD(PcdBiosSsaBacksideMargining);
	DISPLAY_UPD(PcdBiosSsaEarlyReadIdMargining);
	DISPLAY_UPD(PcdBiosSsaStepSizeOverride);
	DISPLAY_UPD(PcdBiosSsaRxDqs);
	DISPLAY_UPD(PcdBiosSsaRxVref);
	DISPLAY_UPD(PcdBiosSsaTxDq);
	DISPLAY_UPD(PcdBiosSsaTxVref);
	DISPLAY_UPD(PcdBiosSsaCmdAll);
	DISPLAY_UPD(PcdBiosSsaCmdVref);
	DISPLAY_UPD(PcdBiosSsaCtlAll);
	DISPLAY_UPD(PcdBiosSsaEridDelay);
	DISPLAY_UPD(PcdBiosSsaEridVref);
	DISPLAY_UPD(PcdBiosSsaDebugMessages);
	DISPLAY_UPD(PcdEccSupport);
	DISPLAY_UPD(PcdFastBoot);
	DISPLAY_UPD(PcdMemTest);
	DISPLAY_UPD(PcdMemTurnaroundOpt);
	DISPLAY_UPD(PcdDdrFreq);
	DISPLAY_UPD(PcdCommandTiming);
	DISPLAY_UPD(PcdCustomRefreshRate);
	DISPLAY_UPD(PcdTsegSize);
	DISPLAY_UPD(PcdHsuartDevice);
	DISPLAY_UPD(PcdHeciCommunication);
	DISPLAY_UPD(PcdVtdSupport);
	DISPLAY_UPD(PcdPchUsb3Port);
	DISPLAY_UPD(PcdPchUsb2Port);
	DISPLAY_UPD(PcdPchUsb3PortOc);
	DISPLAY_UPD(PcdPchUsb2PortOc);
	DISPLAY_UPD(PcdUsb2PeTxiSet);
	DISPLAY_UPD(PcdUsb2TxiSet);
	DISPLAY_UPD(PcdUsb2PreDeEmp);
	DISPLAY_UPD(PcdUsb2PreEmpHalfBit);
	DISPLAY_UPD(PcdIIOPciePortBifurcation);
	DISPLAY_UPD(PcdIIoPcieRLinkDeEmphasis);
	DISPLAY_UPD(PcdIIoPciePort1ADeEmphasis);
	DISPLAY_UPD(PcdIIoPciePort1BDeEmphasis);
	DISPLAY_UPD(PcdIIoPciePort1CDeEmphasis);
	DISPLAY_UPD(PcdIIoPciePort1DDeEmphasis);
	DISPLAY_UPD(PcdIIoPcieLinkSpeedRLink);
	DISPLAY_UPD(PcdIIoPciePort1ALinkSpeed);
	DISPLAY_UPD(PcdIIoPciePort1BLinkSpeed);
	DISPLAY_UPD(PcdIIoPciePort1CLinkSpeed);
	DISPLAY_UPD(PcdIIoPciePort1DLinkSpeed);
	DISPLAY_UPD(PcdIIoPcieRLinkAspm);
	DISPLAY_UPD(PcdIIoPciePort1AAspm);
	DISPLAY_UPD(PcdIIoPciePort1BAspm);
	DISPLAY_UPD(PcdIIoPciePort1CAspm);
	DISPLAY_UPD(PcdIIoPciePort1DAspm);
	DISPLAY_UPD(PcdBifurcationPcie0);
	DISPLAY_UPD(PcdBifurcationPcie2);
	DISPLAY_UPD(PcdMemoryThermalThrottling);
	DISPLAY_UPD(PcdFiaMuxOverride);
	DISPLAY_UPD(FiaMuxCfgInvalidate);
	DISPLAY_UPD(PcdPchTraceHubMode);
	DISPLAY_UPD(PcdPchTraceHubMemReg0Size);
	DISPLAY_UPD(PcdPchTraceHubMemReg1Size);
	DISPLAY_UPD(PcdFiaLaneConfigPtr);
	DISPLAY_UPD(PcdKtiBufferPtr);
	DISPLAY_UPD(PcdMemSpdPtr);

	hexdump(fspm_new_upd, sizeof(*fspm_new_upd));
}

void soc_display_fsps_upd_params(const FSPS_UPD *fsps_old_upd, const FSPS_UPD *fsps_new_upd)
{
	const FSP_S_CONFIG *old = &fsps_old_upd->FspsConfig;
	const FSP_S_CONFIG *new = &fsps_new_upd->FspsConfig;

	printk(BIOS_SPEW, "UPD values for FspSiliconInit:\n");

	DISPLAY_UPD(PcdCpuMicrocodePatchBase);
	DISPLAY_UPD(PcdCpuMicrocodePatchSize);
	DISPLAY_UPD(PcdEnableSATA);
	DISPLAY_UPD(PcdEmmc);
	DISPLAY_UPD(PcdEmmcHS400Support);
	DISPLAY_UPD(PcdPcieRootPort0LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort1LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort2LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort3LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort8LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort9LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort10LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort11LinkSpeed);
	DISPLAY_UPD(PcdPcieRootPort0Aspm);
	DISPLAY_UPD(PcdPcieRootPort1Aspm);
	DISPLAY_UPD(PcdPcieRootPort2Aspm);
	DISPLAY_UPD(PcdPcieRootPort3Aspm);
	DISPLAY_UPD(PcdPcieRootPort8Aspm);
	DISPLAY_UPD(PcdPcieRootPort9Aspm);
	DISPLAY_UPD(PcdPcieRootPort10Aspm);
	DISPLAY_UPD(PcdPcieRootPort11Aspm);
	DISPLAY_UPD(PcdPcieRootPort0ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort1ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort2ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort3ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort8ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort9ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort10ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort11ConnectionType);
	DISPLAY_UPD(PcdPcieRootPort0HotPlug);
	DISPLAY_UPD(PcdPcieRootPort1HotPlug);
	DISPLAY_UPD(PcdPcieRootPort2HotPlug);
	DISPLAY_UPD(PcdPcieRootPort3HotPlug);
	DISPLAY_UPD(PcdPcieRootPort8HotPlug);
	DISPLAY_UPD(PcdPcieRootPort9HotPlug);
	DISPLAY_UPD(PcdPcieRootPort10HotPlug);
	DISPLAY_UPD(PcdPcieRootPort11HotPlug);
	DISPLAY_UPD(PcdEMMCDLLConfigPtr);

	hexdump(fsps_new_upd, sizeof(*fsps_new_upd));
}
