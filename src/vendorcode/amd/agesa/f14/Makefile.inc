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
AGESA_ROOT = src/vendorcode/amd/agesa/f14

AGESA_INC =  -Isrc/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/../common
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(AGESA_ROOT)/Lib
AGESA_INC += -I$(AGESA_ROOT)/Legacy
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x14
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x14/ON
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature 
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Gfx
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Gfx/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Modules/GnbCommonLib
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Family/0x14
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/Nb/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Family/0x14
AGESA_INC += -I$(AGESA_ROOT)/Proc/GNB/PCIe/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/ON
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/GNB
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/Mem

CFLAGS_x86_32 += -march=k8-sse3 -mtune=k8-sse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

export AGESA_ROOT := $(AGESA_ROOT)
export AGESA_INC  := $(AGESA_INC)
CPPFLAGS_x86_32 += $(AGESA_INC)
#######################################################################

classes-y += libagesa

$(eval $(call create_class_compiler,libagesa,x86_32))

libagesa-y  = Proc/GNB/Modules/GnbGfxConfig/GfxConfigPost.c
libagesa-y += Proc/GNB/Modules/GnbGfxConfig/GfxConfigEnv.c
libagesa-y += Proc/GNB/Nb/Family/0x14/F14NbLclkDpm.c
libagesa-y += Proc/GNB/Modules/GnbPcieAlibV1/PcieAlib.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieSiliconServices.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibPciAcc.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibHeap.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibCpuAcc.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GfxCardInfo.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibIoAcc.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GfxPowerPlayTable.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieMapTopology.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieInputParser.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieConfigData.c
libagesa-y += Proc/GNB/Modules/GnbPcieTrainingV1/PcieWorkarounds.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibMemAcc.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieTopologyServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieTrainingV1/PcieTraining.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePortServices.c
libagesa-y += Proc/GNB/Modules/GnbGfxInitLibV1/GfxEnumConnectors.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieTimer.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePifServices.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspmExitLatency.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieWrapperRegAcc.c
libagesa-y += Proc/GNB/Modules/GnbPcieConfig/PcieConfigLib.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePortRegAcc.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieUtilityLib.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspmBlackList.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PciePowerMgmt.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieAspm.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLibPci.c
libagesa-y += Proc/GNB/Modules/GnbPcieInitLibV1/PcieSbLink.c
libagesa-y += Proc/GNB/Modules/GnbNbInitLibV1/GnbNbInitLibV1.c
libagesa-y += Proc/GNB/Modules/GnbCommonLib/GnbLib.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14PerCorePciTables.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14BrandIdFt1.c
libagesa-y += Proc/GNB/Nb/NbInitAtPost.c
libagesa-y += Proc/GNB/PCIe/Family/0x14/F14PcieAlib.c
libagesa-y += Proc/CPU/Family/0x14/F14IoCstate.c
libagesa-y += Proc/CPU/Feature/cpuIoCstate.c
libagesa-y += Proc/Mem/NB/ON/mnprotoon.c
libagesa-y += Proc/CPU/Family/0x14/F14MicrocodePatch0500000B.c
libagesa-y += Proc/CPU/Family/0x14/F14MicrocodePatch0500001A.c
libagesa-y += Proc/CPU/Family/0x14/F14MicrocodePatch05000029.c
libagesa-y += Proc/CPU/Family/0x14/F14MicrocodePatch05000119.c
libagesa-y += Proc/GNB/PCIe/Family/0x14/F14PciePifServices.c
libagesa-y += Proc/CPU/Family/0x14/ON/F14OnInitEarlyTable.c
libagesa-y += Proc/CPU/cpuInitEarlyTable.c
libagesa-y += Proc/GNB/Nb/Family/0x14/F14NbLclkNclkRatio.c
libagesa-y += Proc/GNB/Nb/Family/0x14/F14NbSmu.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtEnv.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtPost.c
libagesa-y += Proc/GNB/Nb/Feature/NbLclkDpm.c
libagesa-y += Proc/GNB/Nb/Feature/NbFuseTable.c
libagesa-y += Proc/GNB/Nb/Family/0x14/F14NbPowerGate.c
libagesa-y += Proc/GNB/Common/GnbLibFeatures.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14Pstate.c
libagesa-y += Proc/CPU/Feature/cpuC6State.c
libagesa-y += Proc/CPU/Family/0x14/F14C6State.c
libagesa-y += Proc/Mem/Main/mmMemRestore.c
libagesa-y += Proc/Mem/Main/mm.c
libagesa-y += Proc/CPU/Family/0x14/cpuCommonF14Utilities.c
libagesa-y += Proc/CPU/Family/0x14/ON/F14OnEquivalenceTable.c
libagesa-y += Proc/Mem/Tech/mttml.c
libagesa-y += Proc/Mem/Tech/DDR3/mtrci3.c
libagesa-y += Proc/GNB/GnbInitAtReset.c
libagesa-y += Proc/Mem/NB/ON/mnmcton.c
libagesa-y += Proc/CPU/Feature/cpuCacheInit.c
libagesa-y += Proc/Mem/Tech/mttdimbt.c
libagesa-y += Proc/Mem/Tech/DDR3/mtsdi3.c
libagesa-y += Proc/Mem/Tech/mthdi.c
libagesa-y += Proc/GNB/GnbInitAtEarly.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14PowerMgmtSystemTables.c
libagesa-y += Proc/GNB/PCIe/Family/0x14/F14PcieWrapperServices.c
libagesa-y += Proc/Mem/NB/mnfeat.c
libagesa-y += Proc/Mem/Main/mmStandardTraining.c
libagesa-y += Proc/HT/Fam14/htNbUtilitiesFam14.c
libagesa-y += Proc/CPU/cpuBist.c
libagesa-y += Proc/Mem/Main/mmParallelTraining.c
libagesa-y += Proc/Mem/Feat/LVDDR3/mflvddr3.c
libagesa-y += Proc/GNB/PCIe/Family/0x14/F14PcieComplexConfig.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14PowerPlane.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14LowPowerInit.c
libagesa-y += Proc/Mem/NB/mnreg.c
libagesa-y += Proc/Mem/NB/ON/mnflowon.c
libagesa-y += Proc/Mem/Feat/CHINTLV/mfchi.c
libagesa-y += Proc/GNB/PCIe/PcieInit.c
libagesa-y += Proc/GNB/Gfx/GfxInitAtMidPost.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14CacheDefaults.c
libagesa-y += Proc/Mem/Ps/ON/mpuon3.c
libagesa-y += Proc/CPU/cahalt.c
libagesa-y += Proc/CPU/cahaltasm.S
libagesa-y += Proc/Mem/Tech/mt.c
libagesa-y += Proc/CPU/Family/0x14/ON/F14OnMicrocodePatchTables.c
libagesa-y += Proc/CPU/Family/0x14/ON/F14OnPciTables.c
libagesa-y += Proc/GNB/PCIe/PciePortInit.c
libagesa-y += Proc/Mem/NB/mnflow.c
libagesa-y += Proc/CPU/Feature/cpuFeatureLeveling.c
libagesa-y += Proc/Mem/Tech/mttoptsrc.c
libagesa-y += Proc/Mem/Feat/PARTRN/mfParallelTraining.c
libagesa-y += Proc/GNB/Gfx/GfxInitAtPost.c
libagesa-y += Proc/Mem/Main/mmEcc.c
libagesa-y += Legacy/Proc/Dispatcher.c
libagesa-y += Proc/Mem/Feat/MEMCLR/mfmemclr.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14WheaInitDataTables.c
libagesa-y += Proc/CPU/Feature/cpuDmi.c
libagesa-y += Proc/Mem/Tech/mttsrc.c
libagesa-y += Proc/Mem/NB/mnmct.c
libagesa-y += Proc/HT/htInterfaceNonCoherent.c
libagesa-y += Proc/Mem/NB/ON/mnidendimmon.c
libagesa-y += Proc/CPU/Feature/cpuSrat.c
libagesa-y += Proc/Common/AmdS3LateRestore.c
libagesa-y += Proc/CPU/Table.c
libagesa-y += Proc/GNB/PCIe/Family/0x14/F14PcieComplexServices.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14Utilities.c
libagesa-y += Proc/HT/htInterface.c
libagesa-y += Proc/GNB/Gfx/GfxStrapsInit.c
libagesa-y += Proc/GNB/Nb/NbInitAtEarly.c
libagesa-y += Lib/amdlib.c
libagesa-y += Proc/CPU/Feature/cpuCacheFlushOnHalt.c
libagesa-y += Proc/CPU/Feature/cpuCpb.c
libagesa-y += Proc/Mem/Main/minit.c
libagesa-y += Proc/GNB/PCIe/Family/0x14/F14PciePhyServices.c
libagesa-y += Proc/GNB/GnbInitAtLate.c
libagesa-y += Proc/Mem/NB/ON/mnoton.c
libagesa-y += Proc/Mem/Feat/INTLVRN/mfintlvrn.c
libagesa-y += Proc/GNB/Nb/NbInit.c
libagesa-y += Proc/Mem/Feat/IDENDIMM/mfidendimm.c
libagesa-y += Proc/Mem/Main/mmflow.c
libagesa-y += Proc/Mem/Ardk/ma.c
libagesa-y += Proc/Common/CommonInits.c
libagesa-y += Proc/CPU/Feature/cpuPstateLeveling.c
libagesa-y += Proc/Mem/NB/ON/mndcton.c
libagesa-y += Proc/Common/AmdInitPost.c
libagesa-y += Proc/GNB/Gfx/GfxGmcInit.c
libagesa-y += Proc/CPU/cpuBrandId.c
libagesa-y += Proc/Mem/NB/mnphy.c
libagesa-y += Proc/Common/AmdInitEnv.c
libagesa-y += Proc/Mem/Main/mmConditionalPso.c
libagesa-y += Proc/GNB/GnbInitAtPost.c
libagesa-y += Proc/Common/CommonReturns.c
libagesa-y += Proc/CPU/cpuPowerMgmt.c
libagesa-y += Proc/Common/AmdInitResume.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14PowerCheck.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14Dmi.c
libagesa-y += Proc/Mem/NB/ON/mnregon.c
libagesa-y += Proc/GNB/Gfx/GfxIntegratedInfoTableInit.c
libagesa-y += Proc/Common/AmdInitEarly.c
libagesa-y += Proc/CPU/cpuFamilyTranslation.c
libagesa-y += Proc/CPU/cpuPostInit.c
libagesa-y += Proc/Mem/NB/mn.c
libagesa-y += Proc/GNB/Gfx/GfxInitAtEnvPost.c
libagesa-y += Proc/CPU/Feature/cpuHwC1e.c
libagesa-y += Proc/CPU/cpuLateInit.c
libagesa-y += Proc/Common/CreateStruct.c
libagesa-y += Proc/CPU/Feature/cpuWhea.c
libagesa-y += Proc/Mem/Feat/CSINTLV/mfcsi.c
libagesa-y += Proc/Mem/Feat/ECC/mfecc.c
libagesa-y += Proc/Mem/Feat/DMI/mfDMI.c
libagesa-y += Proc/Mem/Ps/ON/mpson3.c
libagesa-y += Proc/Common/AmdLateRunApTask.c
libagesa-y += Proc/CPU/cpuPowerMgmtMultiSocket.c
libagesa-y += Proc/Mem/Main/mmUmaAlloc.c
libagesa-y += Proc/CPU/Feature/cpuPstateTables.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14PciTables.c
libagesa-y += Proc/CPU/cpuPowerMgmtSingleSocket.c
libagesa-y += Proc/GNB/GnbInitAtMid.c
libagesa-y += Proc/Mem/Tech/DDR3/mt3.c
libagesa-y += Proc/Mem/Feat/EXCLUDIMM/mfdimmexclud.c
libagesa-y += Proc/Mem/Main/mmExcludeDimm.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtEarlyPost.c
libagesa-y += Proc/HT/htInterfaceGeneral.c
libagesa-y += Proc/CPU/S3.c
libagesa-y += Proc/CPU/Feature/cpuFeatures.c
libagesa-y += Proc/Mem/Ps/mp.c
libagesa-y += Proc/Mem/Main/mdef.c
libagesa-y += Proc/Mem/Tech/mtthrc.c
libagesa-y += Proc/CPU/cpuGeneralServices.c
libagesa-y += Proc/HT/htNb.c
libagesa-y += Proc/CPU/Feature/cpuSlit.c
libagesa-y += Proc/Mem/Feat/ECC/mfemp.c
libagesa-y += Proc/GNB/Nb/NbInitAtLatePost.c
libagesa-y += Proc/Mem/Main/mmNodeInterleave.c
libagesa-y += Proc/HT/Fam14/htNbFam14.c
libagesa-y += Proc/GNB/Gfx/GfxConfigData.c
libagesa-y += Proc/Mem/Main/ON/mmflowon.c
libagesa-y += Proc/Common/AmdInitReset.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14MsrTables.c
libagesa-y += Proc/GNB/PCIe/PcieInitAtLatePost.c
libagesa-y += Proc/Mem/Tech/mttEdgeDetect.c
libagesa-y += Proc/GNB/Gfx/GfxRegisterAcc.c
libagesa-y += Proc/Common/AmdInitLate.c
libagesa-y += Proc/HT/htFeat.c
libagesa-y += Proc/GNB/Nb/NbInitAtReset.c
libagesa-y += Proc/CPU/Family/0x14/ON/F14OnLogicalIdTables.c
libagesa-y += Proc/CPU/Family/0x14/ON/F14OnCpb.c
libagesa-y += Legacy/Proc/hobTransfer.c
libagesa-y += Proc/CPU/cpuApicUtilities.c
libagesa-y += Proc/GNB/GnbInitAtEnv.c
libagesa-y += Proc/CPU/cpuEventLog.c
libagesa-y += Proc/HT/htInterfaceCoherent.c
libagesa-y += Proc/Mem/Main/muc.c
libagesa-y += Proc/Mem/Main/mmLvDdr3.c
libagesa-y += Proc/CPU/cpuMicrocodePatch.c
libagesa-y += Proc/Mem/Tech/mttecc.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14SoftwareThermal.c
libagesa-y += Legacy/Proc/agesaCallouts.c
libagesa-y += Proc/Mem/NB/ON/mnon.c
libagesa-y += Proc/Mem/Feat/ODTHERMAL/mfodthermal.c
libagesa-y += Proc/Mem/Feat/PARTRN/mfStandardTraining.c
libagesa-y += Proc/Mem/NB/mndct.c
libagesa-y += Proc/Mem/Main/mmOnlineSpare.c
libagesa-y += Proc/CPU/cpuEarlyInit.c
libagesa-y += Proc/Mem/NB/ON/mnphyon.c
libagesa-y += Proc/Mem/Main/merrhdl.c
libagesa-y += Proc/Common/AmdS3Save.c
libagesa-y += Proc/Mem/Tech/DDR3/mtspd3.c
libagesa-y += Proc/CPU/Family/0x14/cpuF14BrandId.c
libagesa-y += Proc/HT/htMain.c
libagesa-y += Proc/CPU/cpuWarmReset.c
libagesa-y += Proc/GNB/Nb/NbConfigData.c
libagesa-y += Proc/Mem/Tech/DDR3/mttwl3.c
libagesa-y += Proc/CPU/Feature/cpuPstateGather.c
libagesa-y += Proc/Mem/Tech/DDR3/mtot3.c
libagesa-y += Proc/Mem/Main/mmMemClr.c
libagesa-y += Proc/Mem/Ardk/ON/mauon3.c
libagesa-y += Proc/Mem/Feat/TABLE/mftds.c
libagesa-y += Proc/Mem/Ardk/ON/mason3.c
libagesa-y += Proc/Mem/Feat/S3/mfs3.c
libagesa-y += Proc/HT/htNotify.c
libagesa-y += Proc/Mem/NB/mnS3.c
libagesa-y += Proc/CPU/heapManager.c
libagesa-y += Proc/Mem/Tech/DDR3/mttecc3.c
libagesa-y += Proc/Mem/NB/ON/mnS3on.c
libagesa-y += Proc/Common/AmdInitMid.c
libagesa-y += Proc/Mem/NB/mntrain3.c
libagesa-y += Proc/GNB/Gfx/Family/0x14/F14GfxServices.c
libagesa-y += Proc/Common/S3SaveState.c
libagesa-y += Proc/Common/S3RestoreState.c
libagesa-y += Proc/GNB/PCIe/PcieLateInit.c
libagesa-y += Proc/GNB/PCIe/PciePortLateInit.c
libagesa-y += Proc/GNB/PCIe/PcieMiscLib.c
libagesa-y += Proc/GNB/Nb/NbSmuLib.c
libagesa-y += Proc/GNB/Nb/NbInitAtEnv.c
libagesa-y += Proc/GNB/Nb/Family/0x14/F14NbServices.c
libagesa-y += Proc/GNB/Gfx/GfxLib.c
libagesa-y += Proc/GNB/Nb/NbPowerMgmt.c
libagesa-y += Proc/Recovery/HT/htInitReset.c
libagesa-y += Proc/Mem/Main/mu.c

$(obj)/libagesa.fam14.a: $$(libagesa-objs)
	@printf " AGESA $(subst $(obj)/,,$(@))\n"
	ar rcs $@ $+

romstage-libs += $(obj)/libagesa.fam14.a
ramstage-libs += $(obj)/libagesa.fam14.a
