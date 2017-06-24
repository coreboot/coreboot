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

# AGESA V5 Files
AGESA_ROOT = src/vendorcode/amd/agesa/f12

AGESA_INC  = -I$(src)/vendorcode/amd/include -I$(src)/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/../common
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x12
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Gfx
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Gfx/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxConfig
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbGfxInitLibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbSbLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieConfig
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbNbInitLibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Family/LN
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbCableSafe
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieInitLibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieAlibV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbPcieTrainingV1
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Family/LN
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/Fam12
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/CHINTLV
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/CSINTLV
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/ECC
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/IDENDIMM
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/INTLVRN
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/LVDDR3
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/ODTHERMAL
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Main
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/LN
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Tech
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Tech/DDR3

CFLAGS_x86_32 += -march=k8-sse3 -mtune=k8-sse3 -fno-zero-initialized-in-bss -fno-strict-aliasing
CFLAGS_x86_64 += -march=k8-sse3 -mtune=k8-sse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

export AGESA_INC  := $(AGESA_INC)
CPPFLAGS_x86_32 += $(AGESA_INC)
CPPFLAGS_x86_64 += $(AGESA_INC)

#######################################################################

libagesa-y += Legacy/Proc/agesaCallouts.c
libagesa-y += Legacy/Proc/Dispatcher.c
libagesa-y += Legacy/Proc/hobTransfer.c
libagesa-y += Proc/Common/AmdInitEarly.c
libagesa-y += Proc/Common/AmdInitEnv.c
libagesa-y += Proc/Common/AmdInitLate.c
libagesa-y += Proc/Common/AmdInitMid.c
libagesa-y += Proc/Common/AmdInitPost.c
libagesa-y += Proc/Common/AmdInitReset.c
libagesa-y += Proc/Common/AmdInitResume.c
libagesa-y += Proc/Common/AmdLateRunApTask.c
libagesa-y += Proc/Common/AmdS3LateRestore.c
libagesa-y += Proc/Common/AmdS3Save.c
libagesa-y += Proc/Common/CommonInits.c
libagesa-y += Proc/Common/CommonReturns.c
libagesa-y += Proc/Common/CreateStruct.c
libagesa-y += Proc/Common/S3RestoreState.c
libagesa-y += Proc/Common/S3SaveState.c
libagesa-y += Proc/CPU/cahalt.c
libagesa-y += Proc/CPU/cpuApicUtilities.c
libagesa-y += Proc/CPU/cpuBist.c
libagesa-y += Proc/CPU/cpuBrandId.c
libagesa-y += Proc/CPU/cpuEarlyInit.c
libagesa-y += Proc/CPU/cpuEventLog.c
libagesa-y += Proc/CPU/cpuFamilyTranslation.c
libagesa-y += Proc/CPU/cpuGeneralServices.c
libagesa-y += Proc/CPU/cpuInitEarlyTable.c
libagesa-y += Proc/CPU/cpuLateInit.c
libagesa-y += Proc/CPU/cpuMicrocodePatch.c
libagesa-y += Proc/CPU/cpuPostInit.c
libagesa-y += Proc/CPU/cpuPowerMgmt.c
libagesa-y += Proc/CPU/cpuPowerMgmtMultiSocket.c
libagesa-y += Proc/CPU/cpuPowerMgmtSingleSocket.c
libagesa-y += Proc/CPU/cpuWarmReset.c
libagesa-y += Proc/CPU/heapManager.c
libagesa-y += Proc/CPU/S3.c
libagesa-y += Proc/CPU/Table.c
libagesa-y += Proc/CPU/Family/0x12/cpuCommonF12Utilities.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12BrandId.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12BrandIdFm1.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12BrandIdFs1.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12CacheDefaults.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12Dmi.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12EarlyNbPstateInit.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12MsrTables.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12PciTables.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12PerCorePciTables.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12PowerCheck.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12PowerMgmtSystemTables.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12PowerPlane.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12Pstate.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12SoftwareThermal.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12Utilities.c
libagesa-y += Proc/CPU/Family/0x12/cpuF12WheaInitDataTables.c
libagesa-y += Proc/CPU/Family/0x12/F12C6State.c
libagesa-y += Proc/CPU/Family/0x12/F12Cpb.c
libagesa-y += Proc/CPU/Family/0x12/F12IoCstate.c
libagesa-y += Proc/CPU/Family/0x12/F12MicrocodePatch03000002.c
libagesa-y += Proc/CPU/Family/0x12/F12MicrocodePatch0300000e.c
libagesa-y += Proc/CPU/Family/0x12/F12MicrocodePatch0300000f.c
#libagesa-y += Proc/CPU/Family/0x12/LN/F12LnEarlySamples.c
libagesa-y += Proc/CPU/Family/0x12/LN/F12LnEquivalenceTable.c
libagesa-y += Proc/CPU/Family/0x12/LN/F12LnLogicalIdTables.c
libagesa-y += Proc/CPU/Family/0x12/LN/F12LnMicrocodePatchTables.c
libagesa-y += Proc/CPU/Feature/cpuC6State.c
libagesa-y += Proc/CPU/Feature/cpuCacheFlushOnHalt.c
libagesa-y += Proc/CPU/Feature/cpuCacheInit.c
libagesa-y += Proc/CPU/Feature/cpuCoreLeveling.c
libagesa-y += Proc/CPU/Feature/cpuCpb.c
libagesa-y += Proc/CPU/Feature/cpuDmi.c
libagesa-y += Proc/CPU/Feature/cpuFeatureLeveling.c
libagesa-y += Proc/CPU/Feature/cpuFeatures.c
libagesa-y += Proc/CPU/Feature/cpuHwC1e.c
libagesa-y += Proc/CPU/Feature/cpuIoCstate.c
libagesa-y += Proc/CPU/Feature/cpuL3Features.c
libagesa-y += Proc/CPU/Feature/cpuLowPwrPstate.c
libagesa-y += Proc/CPU/Feature/cpuPstateGather.c
libagesa-y += Proc/CPU/Feature/cpuPstateLeveling.c
libagesa-y += Proc/CPU/Feature/cpuPstateTables.c
libagesa-y += Proc/CPU/Feature/cpuSlit.c
libagesa-y += Proc/CPU/Feature/cpuSrat.c
libagesa-y += Proc/CPU/Feature/cpuSwC1e.c
libagesa-y += Proc/CPU/Feature/cpuWhea.c
libagesa-y += Proc/CPU/Feature/PreserveMailbox.c
libagesa-y += Proc/GNB/GnbInitAtEarly.c
libagesa-y += Proc/GNB/GnbInitAtEnv.c
libagesa-y += Proc/GNB/GnbInitAtLate.c
libagesa-y += Proc/GNB/GnbInitAtMid.c
libagesa-y += Proc/GNB/GnbInitAtPost.c
libagesa-y += Proc/GNB/GnbInitAtReset.c
libagesa-y += Proc/GNB/Common/GnbLibFeatures.c
libagesa-y += Proc/GNB/Gfx/GfxConfigData.c
libagesa-y += Proc/GNB/Gfx/GfxGmcInit.c
libagesa-y += Proc/GNB/Gfx/GfxInitAtEnvPost.c
libagesa-y += Proc/GNB/Gfx/GfxInitAtMidPost.c
libagesa-y += Proc/GNB/Gfx/GfxInitAtPost.c
libagesa-y += Proc/GNB/Gfx/GfxIntegratedInfoTableInit.c
libagesa-y += Proc/GNB/Gfx/GfxLib.c
libagesa-y += Proc/GNB/Gfx/GfxRegisterAcc.c
libagesa-y += Proc/GNB/Gfx/GfxStrapsInit.c
libagesa-y += Proc/GNB/Gfx/Family/LN/F12GfxServices.c
libagesa-y += Proc/GNB/Modules/GnbCableSafe/GnbCableSafe.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLib.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibCpuAcc.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibHeap.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibIoAcc.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibMemAcc.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibPci.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibPciAcc.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibStall.c
libagesa-y += Proc/GNB/Modules/GnbGfxConfig/GfxConfigEnv.c
libagesa-y += Proc/GNB/Modules/GnbGfxConfig/GfxConfigPost.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GfxCardInfo.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GfxEnumConnectors.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GfxPowerPlayTable.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GnbGfxInitLibV1.c
libagesa-y += Proc/GNB/Modules/GnbNbInitLibV1/GnbNbInitLibV1.c
libagesa-y += Proc/GNB/Modules/GnbPcieAlibV1/PcieAlib.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieConfigData.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieConfigLib.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieInputParser.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieMapTopology.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspm.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspmBlackList.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspmExitLatency.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePhyServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePifServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePortRegAcc.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePortServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePowerMgmt.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieSiliconServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieTimer.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieTopologyServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieUtilityLib.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieWrapperRegAcc.c
libagesa-y += Proc/GNB/Modules/GnbPcieTrainingV1/PcieTraining.c
libagesa-y += Proc/GNB/Modules/GnbPcieTrainingV1/PcieWorkarounds.c
libagesa-y += Proc/GNB/Modules/GnbSbLib/GnbSbLib.c
libagesa-y += Proc/GNB/Modules/GnbSbLib/GnbSbPcie.c
libagesa-y += Proc/GNB/Nb/NbConfigData.c
libagesa-y += Proc/GNB/Nb/NbInit.c
libagesa-y += Proc/GNB/Nb/NbInitAtEarly.c
libagesa-y += Proc/GNB/Nb/NbInitAtEnv.c
libagesa-y += Proc/GNB/Nb/NbInitAtLatePost.c
libagesa-y += Proc/GNB/Nb/NbInitAtPost.c
libagesa-y += Proc/GNB/Nb/NbInitAtReset.c
libagesa-y += Proc/GNB/Nb/NbPowerMgmt.c
libagesa-y += Proc/GNB/Nb/NbSmuLib.c
libagesa-y += Proc/GNB/Nb/Family/LN/F12NbLclkDpm.c
libagesa-y += Proc/GNB/Nb/Family/LN/F12NbPowerGate.c
libagesa-y += Proc/GNB/Nb/Family/LN/F12NbServices.c
libagesa-y += Proc/GNB/Nb/Family/LN/F12NbSmu.c
libagesa-y += Proc/GNB/Nb/Feature/NbFuseTable.c
libagesa-y += Proc/GNB/Nb/Feature/NbLclkDpm.c
libagesa-y += Proc/GNB/PCIe/PcieInit.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtEarlyPost.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtEnv.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtLatePost.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtPost.c
libagesa-y += Proc/GNB/PCIe/PcieLateInit.c
libagesa-y += Proc/GNB/PCIe/PciePortInit.c
libagesa-y += Proc/GNB/PCIe/PciePortLateInit.c
libagesa-y += Proc/GNB/PCIe/Family/LN/F12PcieAlib.c
libagesa-y += Proc/GNB/PCIe/Family/LN/F12PcieComplexConfig.c
libagesa-y += Proc/GNB/PCIe/Family/LN/F12PcieComplexServices.c
libagesa-y += Proc/GNB/PCIe/Family/LN/F12PciePhyServices.c
libagesa-y += Proc/GNB/PCIe/Family/LN/F12PciePifServices.c
libagesa-y += Proc/GNB/PCIe/Family/LN/F12PcieWrapperServices.c
libagesa-y += Proc/GNB/PCIe/Feature/PciePowerGate.c
libagesa-y += Proc/HT/htFeat.c
libagesa-y += Proc/HT/htInterface.c
libagesa-y += Proc/HT/htInterfaceCoherent.c
libagesa-y += Proc/HT/htInterfaceGeneral.c
libagesa-y += Proc/HT/htInterfaceNonCoherent.c
libagesa-y += Proc/HT/htMain.c
libagesa-y += Proc/HT/htNb.c
libagesa-y += Proc/HT/htNotify.c
libagesa-y += Proc/HT/Fam12/htNbFam12.c
libagesa-y += Proc/HT/Fam12/htNbUtilitiesFam12.c
#libagesa-y += Proc/IDS/Control/IdsCtrl.c
#libagesa-y += Proc/IDS/Control/IdsLib.c
#libagesa-y += Proc/IDS/Control/IdsNvToCmos.c
#libagesa-y += Proc/IDS/Debug/IdsDebug.c
#libagesa-y += Proc/IDS/Family/0x12/IdsF12AllService.c
#libagesa-y += Proc/IDS/Perf/IdsPerf.c
libagesa-y += Proc/Mem/Ardk/ma.c
libagesa-y += Proc/Mem/Ardk/LN/masln3.c
libagesa-y += Proc/Mem/Ardk/LN/mauln3.c
libagesa-y += Proc/Mem/Feat/CHINTLV/mfchi.c
libagesa-y += Proc/Mem/Feat/CSINTLV/mfcsi.c
libagesa-y += Proc/Mem/Feat/DMI/mfDMI.c
libagesa-y += Proc/Mem/Feat/ECC/mfecc.c
libagesa-y += Proc/Mem/Feat/ECC/mfemp.c
libagesa-y += Proc/Mem/Feat/EXCLUDIMM/mfdimmexclud.c
libagesa-y += Proc/Mem/Feat/IDENDIMM/mfidendimm.c
libagesa-y += Proc/Mem/Feat/INTLVRN/mfintlvrn.c
libagesa-y += Proc/Mem/Feat/LVDDR3/mflvddr3.c
libagesa-y += Proc/Mem/Feat/MEMCLR/mfmemclr.c
libagesa-y += Proc/Mem/Feat/ODTHERMAL/mfodthermal.c
libagesa-y += Proc/Mem/Feat/PARTRN/mfParallelTraining.c
libagesa-y += Proc/Mem/Feat/PARTRN/mfStandardTraining.c
libagesa-y += Proc/Mem/Feat/S3/mfs3.c
libagesa-y += Proc/Mem/Feat/TABLE/mftds.c
libagesa-y += Proc/Mem/Main/mdef.c
libagesa-y += Proc/Mem/Main/merrhdl.c
libagesa-y += Proc/Mem/Main/minit.c
libagesa-y += Proc/Mem/Main/mm.c
libagesa-y += Proc/Mem/Main/mmConditionalPso.c
libagesa-y += Proc/Mem/Main/mmEcc.c
libagesa-y += Proc/Mem/Main/mmExcludeDimm.c
libagesa-y += Proc/Mem/Main/mmflow.c
libagesa-y += Proc/Mem/Main/mmLvDdr3.c
libagesa-y += Proc/Mem/Main/mmMemClr.c
libagesa-y += Proc/Mem/Main/mmMemRestore.c
libagesa-y += Proc/Mem/Main/mmNodeInterleave.c
libagesa-y += Proc/Mem/Main/mmOnlineSpare.c
libagesa-y += Proc/Mem/Main/mmParallelTraining.c
libagesa-y += Proc/Mem/Main/mmStandardTraining.c
libagesa-y += Proc/Mem/Main/mmUmaAlloc.c
libagesa-y += Proc/Mem/Main/mu.c
libagesa-y += Proc/Mem/Main/muc.c
libagesa-y += Proc/Mem/Main/LN/mmflowln.c
libagesa-y += Proc/Mem/NB/mn.c
libagesa-y += Proc/Mem/NB/mndct.c
libagesa-y += Proc/Mem/NB/mnfeat.c
libagesa-y += Proc/Mem/NB/mnflow.c
libagesa-y += Proc/Mem/NB/mnmct.c
libagesa-y += Proc/Mem/NB/mnphy.c
libagesa-y += Proc/Mem/NB/mnreg.c
libagesa-y += Proc/Mem/NB/mnS3.c
libagesa-y += Proc/Mem/NB/mntrain3.c
libagesa-y += Proc/Mem/NB/LN/mndctln.c
libagesa-y += Proc/Mem/NB/LN/mnflowln.c
libagesa-y += Proc/Mem/NB/LN/mnidendimmln.c
libagesa-y += Proc/Mem/NB/LN/mnln.c
libagesa-y += Proc/Mem/NB/LN/mnmctln.c
libagesa-y += Proc/Mem/NB/LN/mnotln.c
libagesa-y += Proc/Mem/NB/LN/mnphyln.c
libagesa-y += Proc/Mem/NB/LN/mnprotoln.c
libagesa-y += Proc/Mem/NB/LN/mnregln.c
libagesa-y += Proc/Mem/NB/LN/mnS3ln.c
libagesa-y += Proc/Mem/Ps/mp.c
libagesa-y += Proc/Mem/Ps/mplribt.c
libagesa-y += Proc/Mem/Ps/mplrnlr.c
libagesa-y += Proc/Mem/Ps/mplrnpr.c
libagesa-y += Proc/Mem/Ps/mpmaxfreq.c
libagesa-y += Proc/Mem/Ps/mpmr0.c
libagesa-y += Proc/Mem/Ps/mpodtpat.c
libagesa-y += Proc/Mem/Ps/mprc10opspd.c
libagesa-y += Proc/Mem/Ps/mprc2ibt.c
libagesa-y += Proc/Mem/Ps/mprtt.c
libagesa-y += Proc/Mem/Ps/mpsao.c
libagesa-y += Proc/Mem/Ps/LN/mprln3.c
libagesa-y += Proc/Mem/Ps/LN/mpsln3.c
libagesa-y += Proc/Mem/Ps/LN/mpuln3.c
libagesa-y += Proc/Mem/Tech/mt.c
libagesa-y += Proc/Mem/Tech/mthdi.c
libagesa-y += Proc/Mem/Tech/mttdimbt.c
libagesa-y += Proc/Mem/Tech/mttecc.c
libagesa-y += Proc/Mem/Tech/mttEdgeDetect.c
libagesa-y += Proc/Mem/Tech/mtthrc.c
libagesa-y += Proc/Mem/Tech/mtthrcSeedTrain.c
libagesa-y += Proc/Mem/Tech/mttml.c
libagesa-y += Proc/Mem/Tech/mttoptsrc.c
libagesa-y += Proc/Mem/Tech/mttsrc.c
libagesa-y += Proc/Mem/Tech/DDR3/mt3.c
libagesa-y += Proc/Mem/Tech/DDR3/mtlrdimm3.c
libagesa-y += Proc/Mem/Tech/DDR3/mtot3.c
libagesa-y += Proc/Mem/Tech/DDR3/mtrci3.c
libagesa-y += Proc/Mem/Tech/DDR3/mtsdi3.c
libagesa-y += Proc/Mem/Tech/DDR3/mtspd3.c
libagesa-y += Proc/Mem/Tech/DDR3/mttecc3.c
libagesa-y += Proc/Mem/Tech/DDR3/mttwl3.c
libagesa-y += Proc/Recovery/HT/htInitRecovery.c
libagesa-y += Proc/Recovery/HT/htInitReset.c

$(obj)/libagesa.fam12.a: $$(libagesa-objs)
	@printf "    AGESA        $(subst $(obj)/,,$(@))\n"
	ar rcs $@ $+

romstage-libs  += $(obj)/libagesa.fam12.a
ramstage-libs  += $(obj)/libagesa.fam12.a
