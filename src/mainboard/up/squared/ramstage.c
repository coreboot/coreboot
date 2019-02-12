/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2019 Felix Singer <felix.singer@9elements.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include <soc/ramstage.h>
#include <fsp/api.h>
#include <FspsUpd.h>
#include <console/console.h>

void mainboard_silicon_init_params(FSP_S_CONFIG *silconfig)
{
	printk(BIOS_DEBUG, "MAINBOARD: %s/%s called\n", __FILE__, __func__);

	silconfig->C1e = 0x1;			// 0x0
	silconfig->PkgCStateLimit = 0xFE;	// 0x2
	silconfig->CStateAutoDemotion = 0x3;	// 0x0
	silconfig->CStateUnDemotion = 0x3;	// 0x0
	silconfig->PkgCStateDemotion = 0x1;	// 0x0
	silconfig->PkgCStateUnDemotion = 0x1;	// 0x0
	silconfig->IpuEn = 0x0;			// 0x1
	silconfig->Pme = 0x1;				// 0x0
	silconfig->HdAudioIoBufferOwnership = 0x3;	// 0x0
	silconfig->DspEndpointDmic = 0x0;		// 0x1
	silconfig->DspEndpointBluetooth = 0x0;		// 0x1
	silconfig->DspEndpointI2sSkp = 0x1;		// 0x0
	silconfig->DspEndpointI2sHp = 0x1;		// 0x0
	silconfig->HDAudioPwrGate = 0x1;		// 0x0
	silconfig->HDAudioClkGate = 0x1;		// 0x0
	silconfig->DspFeatureMask = 0x2A;		// 0x0
	silconfig->HpetBdfValid = 0x1;			// 0x0
	silconfig->HpetDeviceNumber = 0xF;		// 0x1f
	silconfig->IoApicBdfValid = 0x1;		// 0x0
	silconfig->IoApicDeviceNumber = 0x1F;		// 0xf
	silconfig->IshEnable = 0x0;			// 0x1
	silconfig->SpiEiss = 0x0;			// 0x1
	silconfig->LPSS_S0ixEnable = 0x1;		// 0x0
	silconfig->SdcardEnabled = 0x0;			// 0x1
	silconfig->eMMCHostMaxSpeed = 0x2;		// 0x0
	silconfig->Usb30Mode = 0x1;			// 0x0
	silconfig->VtdEnable = 0x1;			// 0x0
	silconfig->MonitorMwaitEnable = 0x0;			// 0x1
	silconfig->HdAudioDspUaaCompliance = 0x1;		// 0x0
	silconfig->InitS3Cpu = 0x1;				// 0x0

	silconfig->PortUsb20PerPortTxPeHalf[0] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[0] = 0x7;
	silconfig->PortUsb20PerPortTxiSet[0] = 0x0;
	silconfig->PortUsb20HsSkewSel[0] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[0] = 0x3;
	silconfig->PortUsb20PerPortRXISet[0] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[0] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[1] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[1] = 0x6;
	silconfig->PortUsb20PerPortTxiSet[1] = 0x0;
	silconfig->PortUsb20HsSkewSel[1] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[1] = 0x3;
	silconfig->PortUsb20PerPortRXISet[1] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[1] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[2] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[2] = 0x6;
	silconfig->PortUsb20PerPortTxiSet[2] = 0x0;
	silconfig->PortUsb20HsSkewSel[2] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[2] = 0x3;
	silconfig->PortUsb20PerPortRXISet[2] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[2] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[3] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[3] = 0x6;
	silconfig->PortUsb20PerPortTxiSet[3] = 0x0;
	silconfig->PortUsb20HsSkewSel[3] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[3] = 0x3;
	silconfig->PortUsb20PerPortRXISet[3] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[3] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[4] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[4] = 0x7;
	silconfig->PortUsb20PerPortTxiSet[4] = 0x0;
	silconfig->PortUsb20HsSkewSel[4] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[4] = 0x3;
	silconfig->PortUsb20PerPortRXISet[4] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[4] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[5] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[5] = 0x7;
	silconfig->PortUsb20PerPortTxiSet[5] = 0x0;
	silconfig->PortUsb20HsSkewSel[5] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[5] = 0x3;
	silconfig->PortUsb20PerPortRXISet[5] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[5] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[6] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[6] = 0x7;
	silconfig->PortUsb20PerPortTxiSet[6] = 0x0;
	silconfig->PortUsb20HsSkewSel[6] = 0x0;
	silconfig->PortUsb20IUsbTxEmphasisEn[6] = 0x3;
	silconfig->PortUsb20PerPortRXISet[6] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[6] = 0x0;

	silconfig->PortUsb20PerPortTxPeHalf[7] = 0x0;
	silconfig->PortUsb20PerPortPeTxiSet[7] = 0x1;
	silconfig->PortUsb20PerPortTxiSet[7] = 0x3;
	silconfig->PortUsb20HsSkewSel[7] = 0x1;
	silconfig->PortUsb20IUsbTxEmphasisEn[7] = 0x1;
	silconfig->PortUsb20PerPortRXISet[7] = 0x0;
	silconfig->PortUsb20HsNpreDrvSel[7] = 0x3;


	silconfig->WriteProtectionEnable[0] = 0x1;
	silconfig->ReadProtectionEnable[0] = 0x1;
	silconfig->ProtectedRangeLimit[0] = 0xFFF;
	silconfig->ProtectedRangeBase[0] = 0x0;

	silconfig->IPC[0] = 0xFFFFEEF8;
	silconfig->IPC[1] = 0xFFFFFFFF;
	silconfig->IPC[2] = 0xFFFFFFFF;
	silconfig->IPC[3] = 0xFFFFFFFF;

	silconfig->SataPortsDisableDynamicPg[0] = 0x0;
	silconfig->SataPortsEnable[0] = 0x1;
	silconfig->SataPortsDevSlp[0] = 0x0;
	silconfig->SataPortsHotPlug[0] = 0x0;
	silconfig->SataPortsInterlockSw[0] = 0x1;
	silconfig->SataPortsExternal[0] = 0x0;
	silconfig->SataPortsSpinUp[0] = 0x0;
	silconfig->SataPortsSolidStateDrive[0] = 0x0;
	silconfig->SataPortsEnableDitoConfig[0] = 0x0;
	silconfig->SataPortsDmVal[0] = 0xF;
	silconfig->SataPortsDitoVal[0] = 0x271;

	silconfig->SataPortsDisableDynamicPg[1] = 0x0;
	silconfig->SataPortsEnable[1] = 0x1;
	silconfig->SataPortsDevSlp[1] = 0x0;
	silconfig->SataPortsHotPlug[1] = 0x0;
	silconfig->SataPortsInterlockSw[1] = 0x1;
	silconfig->SataPortsExternal[1] = 0x0;
	silconfig->SataPortsSpinUp[1] = 0x0;
	silconfig->SataPortsSolidStateDrive[1] = 0x0;
	silconfig->SataPortsEnableDitoConfig[1] = 0x0;
	silconfig->SataPortsDmVal[1] = 0xF;
	silconfig->SataPortsDitoVal[1] = 0x271;


	silconfig->PcieRootPortEn[0] = 0x1;
	silconfig->PcieRpHide[0] = 0x0;
	silconfig->PcieRpSlotImplemented[0] = 0x1;
	silconfig->PcieRpHotPlug[0] = 0x0;
	silconfig->PcieRpPmSci[0] = 0x1;
	silconfig->PcieRpExtSync[0] = 0x1;
	silconfig->PcieRpTransmitterHalfSwing[0] = 0x0;
	silconfig->PcieRpAcsEnabled[0] = 0x1;
	silconfig->PcieRpClkReqSupported[0] = 0x1;
	silconfig->PcieRpClkReqNumber[0] = 0x2;
	silconfig->PcieRpClkReqDetect[0] = 0x0;
	silconfig->AdvancedErrorReporting[0] = 0x0;
	silconfig->PmeInterrupt[0] = 0x0;
	silconfig->UnsupportedRequestReport[0] = 0x0;
	silconfig->FatalErrorReport[0] = 0x0;
	silconfig->NoFatalErrorReport[0] = 0x0;
	silconfig->CorrectableErrorReport[0] = 0x0;
	silconfig->SystemErrorOnFatalError[0] = 0x0;
	silconfig->SystemErrorOnNonFatalError[0] = 0x0;
	silconfig->SystemErrorOnCorrectableError[0] = 0x0;
	silconfig->PcieRpSpeed[0] = 0x0;
	silconfig->PhysicalSlotNumber[0] = 0x0;
	silconfig->PcieRpCompletionTimeout[0] = 0x0;
	silconfig->PtmEnable[0] = 0x0;
	silconfig->PcieRpAspm[0] = 0x4;
	silconfig->PcieRpL1Substates[0] = 0x3;
	silconfig->PcieRpLtrEnable[0] = 0x1;
	silconfig->PcieRpLtrConfigLock[0] = 0x0;
	silconfig->PcieRpSelectableDeemphasis[0] = 0x1;
	silconfig->PcieRpNonSnoopLatencyOverrideValue[0] = 0x3C;
	silconfig->PcieRpNonSnoopLatencyOverrideMultiplier[0] = 0x2;
	silconfig->PcieRpSlotPowerLimitScale[0] = 0x0;
	silconfig->PcieRpSlotPowerLimitValue[0] = 0x0;
	silconfig->PcieRpLtrMaxNonSnoopLatency[0] = 0x1003;
	silconfig->PcieRpNonSnoopLatencyOverrideMode[0] = 0x2;
	silconfig->PcieRpLtrMaxSnoopLatency[0] = 0x1003;
	silconfig->PcieRpSnoopLatencyOverrideMode[0] = 0x2;
	silconfig->PcieRpSnoopLatencyOverrideValue[0] = 0x3C;
	silconfig->PcieRpSnoopLatencyOverrideMultiplier[0] = 0x2;

	silconfig->PcieRootPortEn[1] = 0x1;
	silconfig->PcieRpHide[1] = 0x0;
	silconfig->PcieRpSlotImplemented[1] = 0x1;
	silconfig->PcieRpHotPlug[1] = 0x0;
	silconfig->PcieRpPmSci[1] = 0x1;
	silconfig->PcieRpExtSync[1] = 0x1;
	silconfig->PcieRpTransmitterHalfSwing[1] = 0x0;
	silconfig->PcieRpAcsEnabled[1] = 0x1;
	silconfig->PcieRpClkReqSupported[1] = 0x1;
	silconfig->PcieRpClkReqNumber[1] = 0x3;
	silconfig->PcieRpClkReqDetect[1] = 0x0;
	silconfig->AdvancedErrorReporting[1] = 0x0;
	silconfig->PmeInterrupt[1] = 0x0;
	silconfig->UnsupportedRequestReport[1] = 0x0;
	silconfig->FatalErrorReport[1] = 0x0;
	silconfig->NoFatalErrorReport[1] = 0x0;
	silconfig->CorrectableErrorReport[1] = 0x0;
	silconfig->SystemErrorOnFatalError[1] = 0x0;
	silconfig->SystemErrorOnNonFatalError[1] = 0x0;
	silconfig->SystemErrorOnCorrectableError[1] = 0x0;
	silconfig->PcieRpSpeed[1] = 0x0;
	silconfig->PhysicalSlotNumber[1] = 0x1;
	silconfig->PcieRpCompletionTimeout[1] = 0x0;
	silconfig->PtmEnable[1] = 0x0;
	silconfig->PcieRpAspm[1] = 0x4;
	silconfig->PcieRpL1Substates[1] = 0x3;
	silconfig->PcieRpLtrEnable[1] = 0x1;
	silconfig->PcieRpLtrConfigLock[1] = 0x0;
	silconfig->PcieRpSelectableDeemphasis[1] = 0x1;
	silconfig->PcieRpNonSnoopLatencyOverrideValue[1] = 0x3C;
	silconfig->PcieRpNonSnoopLatencyOverrideMultiplier[1] = 0x2;
	silconfig->PcieRpSlotPowerLimitScale[1] = 0x0;
	silconfig->PcieRpSlotPowerLimitValue[1] = 0x0;
	silconfig->PcieRpLtrMaxNonSnoopLatency[1] = 0x1003;
	silconfig->PcieRpNonSnoopLatencyOverrideMode[1] = 0x2;
	silconfig->PcieRpLtrMaxSnoopLatency[1] = 0x1003;
	silconfig->PcieRpSnoopLatencyOverrideMode[1] = 0x2;
	silconfig->PcieRpSnoopLatencyOverrideValue[1] = 0x3C;
	silconfig->PcieRpSnoopLatencyOverrideMultiplier[1] = 0x2;

	silconfig->PcieRootPortEn[2] = 0x1;
	silconfig->PcieRpHide[2] = 0x0;
	silconfig->PcieRpSlotImplemented[2] = 0x1;
	silconfig->PcieRpHotPlug[2] = 0x0;
	silconfig->PcieRpPmSci[2] = 0x1;
	silconfig->PcieRpExtSync[2] = 0x1;
	silconfig->PcieRpTransmitterHalfSwing[2] = 0x0;
	silconfig->PcieRpAcsEnabled[2] = 0x1;
	silconfig->PcieRpClkReqSupported[2] = 0x1;
	silconfig->PcieRpClkReqNumber[2] = 0x0;
	silconfig->PcieRpClkReqDetect[2] = 0x0;
	silconfig->AdvancedErrorReporting[2] = 0x0;
	silconfig->PmeInterrupt[2] = 0x0;
	silconfig->UnsupportedRequestReport[2] = 0x0;
	silconfig->FatalErrorReport[2] = 0x0;
	silconfig->NoFatalErrorReport[2] = 0x0;
	silconfig->CorrectableErrorReport[2] = 0x0;
	silconfig->SystemErrorOnFatalError[2] = 0x0;
	silconfig->SystemErrorOnNonFatalError[2] = 0x0;
	silconfig->SystemErrorOnCorrectableError[2] = 0x0;
	silconfig->PcieRpSpeed[2] = 0x0;
	silconfig->PhysicalSlotNumber[2] = 0x2;
	silconfig->PcieRpCompletionTimeout[2] = 0x0;
	silconfig->PtmEnable[2] = 0x0;
	silconfig->PcieRpAspm[2] = 0x4;
	silconfig->PcieRpL1Substates[2] = 0x3;
	silconfig->PcieRpLtrEnable[2] = 0x1;
	silconfig->PcieRpLtrConfigLock[2] = 0x0;
	silconfig->PcieRpSelectableDeemphasis[2] = 0x1;
	silconfig->PcieRpNonSnoopLatencyOverrideValue[2] = 0x3C;
	silconfig->PcieRpNonSnoopLatencyOverrideMultiplier[2] = 0x2;
	silconfig->PcieRpSlotPowerLimitScale[2] = 0x0;
	silconfig->PcieRpSlotPowerLimitValue[2] = 0x0;
	silconfig->PcieRpLtrMaxNonSnoopLatency[2] = 0x1003;
	silconfig->PcieRpNonSnoopLatencyOverrideMode[2] = 0x2;
	silconfig->PcieRpLtrMaxSnoopLatency[2] = 0x1003;
	silconfig->PcieRpSnoopLatencyOverrideMode[2] = 0x2;
	silconfig->PcieRpSnoopLatencyOverrideValue[2] = 0x0;
	silconfig->PcieRpSnoopLatencyOverrideMultiplier[2] = 0x2;

	silconfig->PcieRootPortEn[3] = 0x1;
	silconfig->PcieRpHide[3] = 0x0;
	silconfig->PcieRpSlotImplemented[3] = 0x1;
	silconfig->PcieRpHotPlug[3] = 0x0;
	silconfig->PcieRpPmSci[3] = 0x1;
	silconfig->PcieRpExtSync[3] = 0x1;
	silconfig->PcieRpTransmitterHalfSwing[3] = 0x0;
	silconfig->PcieRpAcsEnabled[3] = 0x1;
	silconfig->PcieRpClkReqSupported[3] = 0x1;
	silconfig->PcieRpClkReqNumber[3] = 0x1;
	silconfig->PcieRpClkReqDetect[3] = 0x0;
	silconfig->AdvancedErrorReporting[3] = 0x0;
	silconfig->PmeInterrupt[3] = 0x0;
	silconfig->UnsupportedRequestReport[3] = 0x0;
	silconfig->FatalErrorReport[3] = 0x0;
	silconfig->NoFatalErrorReport[3] = 0x0;
	silconfig->CorrectableErrorReport[3] = 0x0;
	silconfig->SystemErrorOnFatalError[3] = 0x0;
	silconfig->SystemErrorOnNonFatalError[3] = 0x0;
	silconfig->SystemErrorOnCorrectableError[3] = 0x0;
	silconfig->PcieRpSpeed[3] = 0x0;
	silconfig->PhysicalSlotNumber[3] = 0x3;
	silconfig->PcieRpCompletionTimeout[3] = 0x0;
	silconfig->PtmEnable[3] = 0x0;
	silconfig->PcieRpAspm[3] = 0x4;
	silconfig->PcieRpL1Substates[3] = 0x3;
	silconfig->PcieRpLtrEnable[3] = 0x1;
	silconfig->PcieRpLtrConfigLock[3] = 0x0;
	silconfig->PcieRpSelectableDeemphasis[3] = 0x1;
	silconfig->PcieRpNonSnoopLatencyOverrideValue[3] = 0x3C;
	silconfig->PcieRpNonSnoopLatencyOverrideMultiplier[3] = 0x2;
	silconfig->PcieRpSlotPowerLimitScale[3] = 0x0;
	silconfig->PcieRpSlotPowerLimitValue[3] = 0x0;
	silconfig->PcieRpLtrMaxNonSnoopLatency[3] = 0x1003;
	silconfig->PcieRpNonSnoopLatencyOverrideMode[3] = 0x2;
	silconfig->PcieRpLtrMaxSnoopLatency[3] = 0x1003;
	silconfig->PcieRpSnoopLatencyOverrideMode[3] = 0x2;
	silconfig->PcieRpSnoopLatencyOverrideValue[3] = 0x3C;
	silconfig->PcieRpSnoopLatencyOverrideMultiplier[3] = 0x2;

	silconfig->PcieRootPortEn[4] = 0x1;
	silconfig->PcieRpHide[4] = 0x0;
	silconfig->PcieRpSlotImplemented[4] = 0x1;
	silconfig->PcieRpHotPlug[4] = 0x0;
	silconfig->PcieRpPmSci[4] = 0x1;
	silconfig->PcieRpExtSync[4] = 0x1;
	silconfig->PcieRpTransmitterHalfSwing[4] = 0x0;
	silconfig->PcieRpAcsEnabled[4] = 0x1;
	silconfig->PcieRpClkReqSupported[4] = 0x1;
	silconfig->PcieRpClkReqNumber[4] = 0x2;
	silconfig->PcieRpClkReqDetect[4] = 0x0;
	silconfig->AdvancedErrorReporting[4] = 0x0;
	silconfig->PmeInterrupt[4] = 0x0;
	silconfig->UnsupportedRequestReport[4] = 0x0;
	silconfig->FatalErrorReport[4] = 0x0;
	silconfig->NoFatalErrorReport[4] = 0x0;
	silconfig->CorrectableErrorReport[4] = 0x0;
	silconfig->SystemErrorOnFatalError[4] = 0x0;
	silconfig->SystemErrorOnNonFatalError[4] = 0x0;
	silconfig->SystemErrorOnCorrectableError[4] = 0x0;
	silconfig->PcieRpSpeed[4] = 0x0;
	silconfig->PhysicalSlotNumber[4] = 0x4;
	silconfig->PcieRpCompletionTimeout[4] = 0x0;
	silconfig->PtmEnable[4] = 0x0;
	silconfig->PcieRpAspm[4] = 0x4;
	silconfig->PcieRpL1Substates[4] = 0x3;
	silconfig->PcieRpLtrEnable[4] = 0x1;
	silconfig->PcieRpLtrConfigLock[4] = 0x0;
	silconfig->PcieRpSelectableDeemphasis[4] = 0x1;
	silconfig->PcieRpNonSnoopLatencyOverrideValue[4] = 0x3C;
	silconfig->PcieRpNonSnoopLatencyOverrideMultiplier[4] = 0x2;
	silconfig->PcieRpSlotPowerLimitScale[4] = 0x0;
	silconfig->PcieRpSlotPowerLimitValue[4] = 0x0;
	silconfig->PcieRpLtrMaxNonSnoopLatency[4] = 0x1003;
	silconfig->PcieRpNonSnoopLatencyOverrideMode[4] = 0x2;
	silconfig->PcieRpLtrMaxSnoopLatency[4] = 0x1003;
	silconfig->PcieRpSnoopLatencyOverrideMode[4] = 0x2;
	silconfig->PcieRpSnoopLatencyOverrideValue[4] = 0x3C;
	silconfig->PcieRpSnoopLatencyOverrideMultiplier[4] = 0x2;

	silconfig->PcieRootPortEn[5] = 0x1;
	silconfig->PcieRpHide[5] = 0x0;
	silconfig->PcieRpSlotImplemented[5] = 0x1;
	silconfig->PcieRpHotPlug[5] = 0x0;
	silconfig->PcieRpPmSci[5] = 0x1;
	silconfig->PcieRpExtSync[5] = 0x1;
	silconfig->PcieRpTransmitterHalfSwing[5] = 0x0;
	silconfig->PcieRpAcsEnabled[5] = 0x1;
	silconfig->PcieRpClkReqSupported[5] = 0x1;
	silconfig->PcieRpClkReqNumber[5] = 0x3;
	silconfig->PcieRpClkReqDetect[5] = 0x0;
	silconfig->AdvancedErrorReporting[5] = 0x0;
	silconfig->PmeInterrupt[5] = 0x0;
	silconfig->UnsupportedRequestReport[5] = 0x0;
	silconfig->FatalErrorReport[5] = 0x0;
	silconfig->NoFatalErrorReport[5] = 0x0;
	silconfig->CorrectableErrorReport[5] = 0x0;
	silconfig->SystemErrorOnFatalError[5] = 0x0;
	silconfig->SystemErrorOnNonFatalError[5] = 0x0;
	silconfig->SystemErrorOnCorrectableError[5] = 0x0;
	silconfig->PcieRpSpeed[5] = 0x0;
	silconfig->PhysicalSlotNumber[5] = 0x5;
	silconfig->PcieRpCompletionTimeout[5] = 0x0;
	silconfig->PtmEnable[5] = 0x0;
	silconfig->PcieRpAspm[5] = 0x4;
	silconfig->PcieRpL1Substates[5] = 0x3;
	silconfig->PcieRpLtrEnable[5] = 0x1;
	silconfig->PcieRpLtrConfigLock[5] = 0x0;
	silconfig->PcieRpSelectableDeemphasis[5] = 0x1;
	silconfig->PcieRpNonSnoopLatencyOverrideValue[5] = 0x3C;
	silconfig->PcieRpNonSnoopLatencyOverrideMultiplier[5] = 0x2;
	silconfig->PcieRpSlotPowerLimitScale[5] = 0x0;
	silconfig->PcieRpSlotPowerLimitValue[5] = 0x0;
	silconfig->PcieRpLtrMaxNonSnoopLatency[5] = 0x1003;
	silconfig->PcieRpNonSnoopLatencyOverrideMode[5] = 0x2;
	silconfig->PcieRpLtrMaxSnoopLatency[5] = 0x1003;
	silconfig->PcieRpSnoopLatencyOverrideMode[5] = 0x2;
	silconfig->PcieRpSnoopLatencyOverrideValue[5] = 0x3C;
	silconfig->PcieRpSnoopLatencyOverrideMultiplier[5] = 0x2;


	silconfig->SsicRate[0] = 0x1;
	silconfig->SsicPortEnable[0] = 0x0;
	silconfig->SsicRate[1] = 0x1;
	silconfig->SsicPortEnable[1] = 0x0;

	silconfig->PortUsb30Enable[0] = 0x1;
	silconfig->PortUs30bOverCurrentPin[0] = 0x0;
	silconfig->PortUsb30Enable[1] = 0x1;
	silconfig->PortUs30bOverCurrentPin[1] = 0x1;
	silconfig->PortUsb30Enable[2] = 0x1;
	silconfig->PortUs30bOverCurrentPin[2] = 0x1;
	silconfig->PortUsb30Enable[3] = 0x1;
	silconfig->PortUs30bOverCurrentPin[3] = 0x1;
	silconfig->PortUsb30Enable[4] = 0x1;
	silconfig->PortUs30bOverCurrentPin[4] = 0x1;
	silconfig->PortUsb30Enable[5] = 0x1;
	silconfig->PortUs30bOverCurrentPin[5] = 0x1;

	silconfig->PortUsb20Enable[0] = 0x1;
	silconfig->PortUs20bOverCurrentPin[0] = 0x0;
	silconfig->PortUsb20Enable[1] = 0x1;
	silconfig->PortUs20bOverCurrentPin[1] = 0x1;
	silconfig->PortUsb20Enable[2] = 0x1;
	silconfig->PortUs20bOverCurrentPin[2] = 0x1;
	silconfig->PortUsb20Enable[3] = 0x1;
	silconfig->PortUs20bOverCurrentPin[3] = 0x1;
	silconfig->PortUsb20Enable[4] = 0x1;
	silconfig->PortUs20bOverCurrentPin[4] = 0x1;
	silconfig->PortUsb20Enable[5] = 0x1;
	silconfig->PortUs20bOverCurrentPin[5] = 0x1;
	silconfig->PortUsb20Enable[6] = 0x1;
	silconfig->PortUs20bOverCurrentPin[6] = 0x2;
	silconfig->PortUsb20Enable[7] = 0x1;
	silconfig->PortUs20bOverCurrentPin[7] = 0x2;
}
