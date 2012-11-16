#*****************************************************************************
#
# Copyright (c) 2011, Advanced Micro Devices, Inc.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of Advanced Micro Devices, Inc. nor the names of
#       its contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

ramstage-y += chip_name.c
ramstage-y += model_12_init.c

AGESA_ROOT = ../../../../vendorcode/amd/agesa/f12

agesa_lib_src   = $(AGESA_ROOT)/Legacy/Proc/agesaCallouts.c
agesa_lib_src  += $(AGESA_ROOT)/Legacy/Proc/Dispatcher.c
agesa_lib_src  += $(AGESA_ROOT)/Legacy/Proc/hobTransfer.c
agesa_lib_src  += $(AGESA_ROOT)/Lib/amdlib.c
agesa_lib_src  += $(AGESA_ROOT)/Lib/helper.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitEarly.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitEnv.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitLate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitMid.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitReset.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdInitResume.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdLateRunApTask.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdS3LateRestore.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/AmdS3Save.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/CommonInits.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/CommonReturns.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/CreateStruct.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/S3RestoreState.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Common/S3SaveState.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cahalt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuApicUtilities.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuBist.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuBrandId.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuEarlyInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuEventLog.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuFamilyTranslation.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuGeneralServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuInitEarlyTable.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuLateInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuMicrocodePatch.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuPostInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuPowerMgmt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuPowerMgmtMultiSocket.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuPowerMgmtSingleSocket.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/cpuWarmReset.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/heapManager.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/S3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Table.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuCommonF12Utilities.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12BrandId.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12BrandIdFm1.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12BrandIdFs1.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12CacheDefaults.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12Dmi.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12EarlyNbPstateInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12MsrTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12PciTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12PerCorePciTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12PowerCheck.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12PowerMgmtSystemTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12PowerPlane.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12Pstate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12SoftwareThermal.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12Utilities.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/cpuF12WheaInitDataTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/F12C6State.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/F12Cpb.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/F12IoCstate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/F12MicrocodePatch03000002.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/F12MicrocodePatch0300000e.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/F12MicrocodePatch0300000f.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/LN/F12LnEarlySamples.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/LN/F12LnEquivalenceTable.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/LN/F12LnLogicalIdTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Family/0x12/LN/F12LnMicrocodePatchTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuC6State.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCacheFlushOnHalt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCacheInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCoreLeveling.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCpb.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuDmi.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuFeatureLeveling.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuFeatures.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuHwC1e.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuIoCstate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuL3Features.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuLowPwrPstate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuPstateGather.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuPstateLeveling.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuPstateTables.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuSlit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuSrat.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuSwC1e.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/cpuWhea.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/CPU/Feature/PreserveMailbox.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/GnbInitAtEarly.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/GnbInitAtEnv.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/GnbInitAtLate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/GnbInitAtMid.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/GnbInitAtPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/GnbInitAtReset.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Common/GnbLibFeatures.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxConfigData.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxGmcInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxInitAtEnvPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxInitAtMidPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxInitAtPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxIntegratedInfoTableInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxLib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxRegisterAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/GfxStrapsInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Gfx/Family/LN/F12GfxServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCableSafe/GnbCableSafe.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibCpuAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibHeap.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibIoAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibMemAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibPci.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibPciAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib/GnbLibStall.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxConfig/GfxConfigEnv.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxConfig/GfxConfigPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxInitLibV1/GfxCardInfo.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxInitLibV1/GfxEnumConnectors.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxInitLibV1/GfxPowerPlayTable.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxInitLibV1/GnbGfxInitLibV1.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbNbInitLibV1/GnbNbInitLibV1.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieAlibV1/PcieAlib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieConfig/PcieConfigData.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieConfig/PcieConfigLib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieConfig/PcieInputParser.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieConfig/PcieMapTopology.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspm.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspmBlackList.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspmExitLatency.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PciePhyServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PciePifServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PciePortRegAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PciePortServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PciePowerMgmt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieSiliconServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieTimer.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieTopologyServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieUtilityLib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1/PcieWrapperRegAcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieTrainingV1/PcieTraining.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieTrainingV1/PcieWorkarounds.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbSbLib/GnbSbLib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Modules/GnbSbLib/GnbSbPcie.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbConfigData.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbInitAtEarly.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbInitAtEnv.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbInitAtLatePost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbInitAtPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbInitAtReset.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbPowerMgmt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/NbSmuLib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/Family/LN/F12NbLclkDpm.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/Family/LN/F12NbPowerGate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/Family/LN/F12NbServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/Family/LN/F12NbSmu.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/Feature/NbFuseTable.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/Nb/Feature/NbLclkDpm.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PcieInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PcieInitAtEarlyPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PcieInitAtEnv.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PcieInitAtLatePost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PcieInitAtPost.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PcieLateInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PciePortInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/PciePortLateInit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN/F12PcieAlib.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN/F12PcieComplexConfig.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN/F12PcieComplexServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN/F12PciePhyServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN/F12PciePifServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN/F12PcieWrapperServices.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/GNB/PCIe/Feature/PciePowerGate.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htFeat.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htInterface.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htInterfaceCoherent.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htInterfaceGeneral.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htInterfaceNonCoherent.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htMain.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htNb.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/htNotify.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/Fam12/htNbFam12.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/HT/Fam12/htNbUtilitiesFam12.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/IDS/Control/IdsCtrl.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/IDS/Control/IdsLib.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/IDS/Control/IdsNvToCmos.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/IDS/Debug/IdsDebug.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/IDS/Family/0x12/IdsF12AllService.c
#agesa_lib_src  += $(AGESA_ROOT)/Proc/IDS/Perf/IdsPerf.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ardk/ma.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ardk/LN/masln3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ardk/LN/mauln3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/CHINTLV/mfchi.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/CSINTLV/mfcsi.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/DMI/mfDMI.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/ECC/mfecc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/ECC/mfemp.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/EXCLUDIMM/mfdimmexclud.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/IDENDIMM/mfidendimm.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/INTLVRN/mfintlvrn.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/LVDDR3/mflvddr3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/MEMCLR/mfmemclr.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/ODTHERMAL/mfodthermal.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/PARTRN/mfParallelTraining.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/PARTRN/mfStandardTraining.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/S3/mfs3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Feat/TABLE/mftds.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mdef.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/merrhdl.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/minit.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mm.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmConditionalPso.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmEcc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmExcludeDimm.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmflow.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmLvDdr3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmMemClr.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmMemRestore.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmNodeInterleave.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmOnlineSpare.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmParallelTraining.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmStandardTraining.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mmUmaAlloc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/mu.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/muc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Main/LN/mmflowln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mn.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mndct.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mnfeat.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mnflow.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mnmct.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mnphy.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mnreg.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mnS3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/mntrain3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mndctln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnflowln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnidendimmln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnmctln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnotln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnphyln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnprotoln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnregln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/NB/LN/mnS3ln.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mp.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mplribt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mplrnlr.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mplrnpr.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mpmaxfreq.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mpmr0.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mpodtpat.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mprc10opspd.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mprc2ibt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mprtt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/mpsao.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/LN/mprln3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/LN/mpsln3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Ps/LN/mpuln3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mthdi.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mttdimbt.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mttecc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mttEdgeDetect.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mtthrc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mtthrcSeedTrain.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mttml.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mttoptsrc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/mttsrc.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mt3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtlrdimm3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtot3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtrci3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtsdi3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtspd3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mttecc3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mttwl3.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Recovery/HT/htInitRecovery.c
agesa_lib_src  += $(AGESA_ROOT)/Proc/Recovery/HT/htInitReset.c

romstage-y += $(agesa_lib_src)
ramstage-y += $(agesa_lib_src)

subdirs-y += ../../mtrr
subdirs-y += ../../../x86/tsc
subdirs-y += ../../../x86/lapic
subdirs-y += ../../../x86/cache
subdirs-y += ../../../x86/mtrr
subdirs-y += ../../../x86/pae
subdirs-y += ../../../x86/smm
