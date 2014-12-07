#
# This file is part of the coreboot project.
#
# Copyright (C) 2012 Advanced Micro Devices, Inc.
# Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
#


## ABSOLUTE AGESA V5 ROOT PATH ##
AGESA_ROOT ?= $(PWD)
AGESA_ROOT = src/vendorcode/amd/agesa/f15

AGESA_INC  = -I$(src)/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/../common
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(AGESA_ROOT)/Lib
AGESA_INC += -I$(AGESA_ROOT)/Legacy
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x10
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x15
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x15/OR
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/Fam10
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/Fam15
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/Features
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/NbCommon
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/htGraph
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS

AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/OR
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/C32
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/HY
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/DA
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/PH
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/RB
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/IDENDIMM/
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Main
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/Recovery/Mem

## AGESA need sse feature ##
CFLAGS_x86_32 += -msse3 -fno-zero-initialized-in-bss -fno-strict-aliasing

export AGESA_ROOT := $(AGESA_ROOT)
export AGESA_INC  := $(AGESA_INC)
CPPFLAGS_x86_32 += $(AGESA_INC)
#######################################################################

classes-y += libagesa

$(eval $(call create_class_compiler,libagesa,x86_32))

libagesa-y += Legacy/Proc/agesaCallouts.c
libagesa-y += Legacy/Proc/Dispatcher.c
libagesa-y += Legacy/Proc/hobTransfer.c
libagesa-y += Lib/amdlib.c
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
libagesa-y += Proc/CPU/cahaltasm.S
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
libagesa-y += Proc/CPU/Family/0x10/cpuCommonF10Utilities.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandId.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdAm3.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdAsb2.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdC32.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdFr1207.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdG34.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdS1g3.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdS1g4.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10CacheDefaults.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10CacheFlushOnHalt.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Cpb.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Dmi.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10EarlyInit.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10FeatureLeveling.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10HtPhyTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10MsrTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PciTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PowerCheck.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PowerMgmtSystemTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PowerPlane.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Pstate.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10SoftwareThermal.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Utilities.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10WheaInitDataTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10WorkaroundsTable.c
libagesa-y += Proc/CPU/Family/0x10/F10InitEarlyTable.c
libagesa-y += Proc/CPU/Family/0x10/F10IoCstate.c
libagesa-y += Proc/CPU/Family/0x10/F10MultiLinkPciTables.c
libagesa-y += Proc/CPU/Family/0x10/F10PmAsymBoostInit.c
libagesa-y += Proc/CPU/Family/0x10/F10PmDualPlaneOnlySupport.c
libagesa-y += Proc/CPU/Family/0x10/F10PmNbCofVidInit.c
libagesa-y += Proc/CPU/Family/0x10/F10PmNbPstateInit.c
libagesa-y += Proc/CPU/Family/0x10/F10SingleLinkPciTables.c
libagesa-y += Proc/CPU/Family/0x15/cpuCommonF15Utilities.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15Apm.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15BrandId.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15CacheDefaults.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15Dmi.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15MsrTables.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15PciTables.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15PowerCheck.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15Utilities.c
libagesa-y += Proc/CPU/Family/0x15/cpuF15WheaInitDataTables.c
libagesa-y += Proc/CPU/Family/0x15/F15PstateHpcMode.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrCacheFlushOnHalt.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrCoreAfterReset.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrDmi.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrFeatureLeveling.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrNbAfterReset.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrPstate.c
libagesa-y += Proc/CPU/Family/0x15/OR/cpuF15OrSoftwareThermal.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrC6State.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrCpb.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrEarlySamples.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrEquivalenceTable.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrHtPhyTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrInitEarlyTable.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrIoCstate.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrL3Features.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrLogicalIdTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrLowPwrPstate.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMicrocodePatch06000425.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMicrocodePatch0600050D_Enc.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMicrocodePatch06000624_Enc.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMicrocodePatchTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMsgBasedC1e.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMsrTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrMultiLinkPciTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrPciTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrPmNbCofVidInit.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrPowerMgmtSystemTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrPowerPlane.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrSharedMsrTable.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrSingleLinkPciTables.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrUtilities.c
libagesa-y += Proc/CPU/Family/0x15/OR/F15OrWorkaroundsTable.c
libagesa-y += Proc/CPU/Feature/cpuApm.c
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
libagesa-y += Proc/CPU/Feature/cpuMsgBasedC1e.c
libagesa-y += Proc/CPU/Feature/cpuPstateGather.c
libagesa-y += Proc/CPU/Feature/cpuPstateHpcMode.c
libagesa-y += Proc/CPU/Feature/cpuPstateLeveling.c
libagesa-y += Proc/CPU/Feature/cpuPstateTables.c
libagesa-y += Proc/CPU/Feature/cpuSlit.c
libagesa-y += Proc/CPU/Feature/cpuSrat.c
libagesa-y += Proc/CPU/Feature/cpuSwC1e.c
libagesa-y += Proc/CPU/Feature/cpuWhea.c
libagesa-y += Proc/CPU/Feature/PreserveMailbox.c
libagesa-y += Proc/HT/htFeat.c
libagesa-y += Proc/HT/htInterface.c
libagesa-y += Proc/HT/htInterfaceCoherent.c
libagesa-y += Proc/HT/htInterfaceGeneral.c
libagesa-y += Proc/HT/htInterfaceNonCoherent.c
libagesa-y += Proc/HT/htMain.c
libagesa-y += Proc/HT/htNb.c
libagesa-y += Proc/HT/htNotify.c
libagesa-y += Proc/HT/Fam10/htNbCoherentFam10.c
libagesa-y += Proc/HT/Fam10/htNbFam10.c
libagesa-y += Proc/HT/Fam10/htNbNonCoherentFam10.c
libagesa-y += Proc/HT/Fam10/htNbOptimizationFam10.c
libagesa-y += Proc/HT/Fam10/htNbSystemFam10.c
libagesa-y += Proc/HT/Fam10/htNbUtilitiesFam10.c
libagesa-y += Proc/HT/Fam15/htNbCoherentFam15.c
libagesa-y += Proc/HT/Fam15/htNbFam15.c
libagesa-y += Proc/HT/Fam15/htNbNonCoherentFam15.c
libagesa-y += Proc/HT/Fam15/htNbOptimizationFam15.c
libagesa-y += Proc/HT/Fam15/htNbSystemFam15.c
libagesa-y += Proc/HT/Fam15/htNbUtilitiesFam15.c
libagesa-y += Proc/HT/Features/htFeatDynamicDiscovery.c
libagesa-y += Proc/HT/Features/htFeatGanging.c
libagesa-y += Proc/HT/Features/htFeatNoncoherent.c
libagesa-y += Proc/HT/Features/htFeatOptimization.c
libagesa-y += Proc/HT/Features/htFeatRouting.c
libagesa-y += Proc/HT/Features/htFeatSets.c
libagesa-y += Proc/HT/Features/htFeatSublinks.c
libagesa-y += Proc/HT/Features/htFeatTrafficDistribution.c
libagesa-y += Proc/HT/Features/htIds.c
libagesa-y += Proc/HT/htGraph/htGraph.c
libagesa-y += Proc/HT/htGraph/htGraph1.c
libagesa-y += Proc/HT/htGraph/htGraph2.c
libagesa-y += Proc/HT/htGraph/htGraph3Line.c
libagesa-y += Proc/HT/htGraph/htGraph3Triangle.c
libagesa-y += Proc/HT/htGraph/htGraph4Degenerate.c
libagesa-y += Proc/HT/htGraph/htGraph4FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph4Kite.c
libagesa-y += Proc/HT/htGraph/htGraph4Line.c
libagesa-y += Proc/HT/htGraph/htGraph4Square.c
libagesa-y += Proc/HT/htGraph/htGraph4Star.c
libagesa-y += Proc/HT/htGraph/htGraph5FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph5TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph6DoubloonLower.c
libagesa-y += Proc/HT/htGraph/htGraph6DoubloonUpper.c
libagesa-y += Proc/HT/htGraph/htGraph6FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph6TwinTriangles.c
libagesa-y += Proc/HT/htGraph/htGraph6TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph7FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph7TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph8DoubloonM.c
libagesa-y += Proc/HT/htGraph/htGraph8FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph8Ladder.c
libagesa-y += Proc/HT/htGraph/htGraph8TwinFullyFourWays.c
libagesa-y += Proc/HT/htGraph/htGraph8TwistedLadder.c
libagesa-y += Proc/HT/NbCommon/htNbCoherent.c
libagesa-y += Proc/HT/NbCommon/htNbNonCoherent.c
libagesa-y += Proc/HT/NbCommon/htNbOptimization.c
libagesa-y += Proc/HT/NbCommon/htNbUtilities.c
libagesa-y += Proc/Mem/Ardk/ma.c
libagesa-y += Proc/Mem/Ardk/C32/marc32_3.c
libagesa-y += Proc/Mem/Ardk/C32/mauc32_3.c
libagesa-y += Proc/Mem/Ardk/HY/marhy3.c
libagesa-y += Proc/Mem/Ardk/HY/mauhy3.c
libagesa-y += Proc/Mem/Ardk/OR/maror3.c
libagesa-y += Proc/Mem/Ardk/OR/mauor3.c
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
libagesa-y += Proc/Mem/Feat/NDINTLV/mfndi.c
libagesa-y += Proc/Mem/Feat/ODTHERMAL/mfodthermal.c
libagesa-y += Proc/Mem/Feat/OLSPARE/mfspr.c
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
libagesa-y += Proc/Mem/NB/mn.c
libagesa-y += Proc/Mem/NB/mndct.c
libagesa-y += Proc/Mem/NB/mnfeat.c
libagesa-y += Proc/Mem/NB/mnflow.c
libagesa-y += Proc/Mem/NB/mnmct.c
libagesa-y += Proc/Mem/NB/mnphy.c
libagesa-y += Proc/Mem/NB/mnreg.c
libagesa-y += Proc/Mem/NB/mnS3.c
libagesa-y += Proc/Mem/NB/mntrain3.c
libagesa-y += Proc/Mem/NB/C32/mnc32.c
libagesa-y += Proc/Mem/NB/C32/mndctc32.c
libagesa-y += Proc/Mem/NB/C32/mnflowc32.c
libagesa-y += Proc/Mem/NB/C32/mnidendimmc32.c
libagesa-y += Proc/Mem/NB/C32/mnmctc32.c
libagesa-y += Proc/Mem/NB/C32/mnotc32.c
libagesa-y += Proc/Mem/NB/C32/mnParTrainc32.c
libagesa-y += Proc/Mem/NB/C32/mnphyc32.c
libagesa-y += Proc/Mem/NB/C32/mnprotoc32.c
libagesa-y += Proc/Mem/NB/C32/mnregc32.c
libagesa-y += Proc/Mem/NB/C32/mnS3c32.c
libagesa-y += Proc/Mem/NB/HY/mndcthy.c
libagesa-y += Proc/Mem/NB/HY/mnflowhy.c
libagesa-y += Proc/Mem/NB/HY/mnhy.c
libagesa-y += Proc/Mem/NB/HY/mnidendimmhy.c
libagesa-y += Proc/Mem/NB/HY/mnmcthy.c
libagesa-y += Proc/Mem/NB/HY/mnothy.c
libagesa-y += Proc/Mem/NB/HY/mnParTrainHy.c
libagesa-y += Proc/Mem/NB/HY/mnphyhy.c
libagesa-y += Proc/Mem/NB/HY/mnprotohy.c
libagesa-y += Proc/Mem/NB/HY/mnreghy.c
libagesa-y += Proc/Mem/NB/HY/mnS3hy.c
libagesa-y += Proc/Mem/NB/OR/mndctor.c
libagesa-y += Proc/Mem/NB/OR/mnflowor.c
libagesa-y += Proc/Mem/NB/OR/mnidendimmor.c
libagesa-y += Proc/Mem/NB/OR/mnmctor.c
libagesa-y += Proc/Mem/NB/OR/mnor.c
libagesa-y += Proc/Mem/NB/OR/mnotor.c
libagesa-y += Proc/Mem/NB/OR/mnpartrainor.c
libagesa-y += Proc/Mem/NB/OR/mnphyor.c
libagesa-y += Proc/Mem/NB/OR/mnprotoor.c
libagesa-y += Proc/Mem/NB/OR/mnregor.c
libagesa-y += Proc/Mem/NB/OR/mns3or.c
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
libagesa-y += Proc/Mem/Ps/mps2d.c
libagesa-y += Proc/Mem/Ps/mpsao.c
libagesa-y += Proc/Mem/Ps/mpseeds.c
libagesa-y += Proc/Mem/Ps/C32/mprc32_3.c
libagesa-y += Proc/Mem/Ps/C32/mpuc32_3.c
libagesa-y += Proc/Mem/Ps/HY/mprhy3.c
libagesa-y += Proc/Mem/Ps/HY/mpshy3.c
libagesa-y += Proc/Mem/Ps/HY/mpuhy3.c
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

libagesa-y += Proc/Mem/Main/mmflow.c
libagesa-y += Proc/Mem/Main/OR/mmflowor.c
libagesa-y += Proc/Mem/Ps/OR/mpor3.c

ifeq ($(CONFIG_CPU_AMD_SOCKET_C32), y)
	libagesa-y += Proc/Mem/Main/C32/mmflowC32.c
	libagesa-y += Proc/Mem/Ps/OR/C32/mpLorC3.c
	libagesa-y += Proc/Mem/Ps/OR/C32/mpRorC3.c
	libagesa-y += Proc/Mem/Ps/OR/C32/mpUorC3.c

#	libagesa-y += Proc/Mem/Main/HY/mmflowhy.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10MicrocodePatch010000c5.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10MicrocodePatch010000d9.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDL3Features.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDMsgBasedC1e.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDUtilities.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyEquivalenceTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyInitEarlyTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyLogicalIdTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyMicrocodePatchTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyPciTables.c
endif

ifeq ($(CONFIG_CPU_AMD_SOCKET_G34), y)
	libagesa-y += Proc/Mem/Main/HY/mmflowhy.c
	libagesa-y += Proc/Mem/Ps/OR/G34/mpLorG3.c
	libagesa-y += Proc/Mem/Ps/OR/G34/mpRorG3.c
	libagesa-y += Proc/Mem/Ps/OR/G34/mpUorG3.c

	libagesa-y += Proc/CPU/Family/0x10/RevD/F10MicrocodePatch010000c5.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10MicrocodePatch010000d9.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDL3Features.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDMsgBasedC1e.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDUtilities.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyEquivalenceTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyInitEarlyTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyLogicalIdTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyMicrocodePatchTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyPciTables.c
endif

ifeq ($(CONFIG_CPU_AMD_SOCKET_AM3R2), y)
	libagesa-y += Proc/Mem/Main/DA/mmflowda.c
	libagesa-y += Proc/Mem/Main/RB/mmflowRb.c
	libagesa-y += Proc/Mem/Main/PH/mmflowPh.c

	libagesa-y += Proc/Mem/Ps/OR/AM3/mpUorA3.c
	libagesa-y += Proc/Mem/Ps/OR/AM3/mpSorA3.c

	libagesa-y += Proc/CPU/Family/0x10/RevC/F10MicrocodePatch010000c6.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/RB/F10RbMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/RB/F10RbLogicalIdTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/RB/F10RbHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/RB/F10RbMicrocodePatchTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/RB/F10RbEquivalenceTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/RB/F10RbPciTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlEquivalenceTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlPciTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlLogicalIdTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlCacheFlushOnHalt.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/BL/F10BlMicrocodePatchTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10RevCUtilities.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10RevCMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10MicrocodePatch010000c7.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaMicrocodePatchTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaCacheFlushOnHalt.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaPciTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaEquivalenceTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/DA/F10DaLogicalIdTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10MicrocodePatch01000085.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10RevCHwC1e.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10RevCPciTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10RevCHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10RevCSwC1e.c
	libagesa-y += Proc/CPU/Family/0x10/RevC/F10MicrocodePatch010000c8.c

	libagesa-y += Proc/CPU/Family/0x10/RevE/PH/F10PhMicrocodePatchTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/PH/F10PhHtPhyTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/PH/F10PhEquivalenceTable.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/PH/F10PhLogicalIdTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/F10MicrocodePatch010000bf.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/F10RevEUtilities.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/F10RevEPciTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/F10RevEMsrTables.c
	libagesa-y += Proc/CPU/Family/0x10/RevE/F10RevEHtPhyTables.c

	libagesa-y += Proc/Mem/NB/DA/mnidendimmda.c
	libagesa-y += Proc/Mem/NB/DA/mnregda.c
	libagesa-y += Proc/Mem/NB/DA/mnS3da.c
	libagesa-y += Proc/Mem/NB/DA/mnprotoda.c
	libagesa-y += Proc/Mem/NB/DA/mnda.c
	libagesa-y += Proc/Mem/NB/DA/mndctda.c
	libagesa-y += Proc/Mem/NB/DA/mnmctda.c
	libagesa-y += Proc/Mem/NB/DA/mnflowda.c
	libagesa-y += Proc/Mem/NB/DA/mnParTrainDa.c
	libagesa-y += Proc/Mem/NB/DA/mnotda.c
	libagesa-y += Proc/Mem/NB/PH/mnS3Ph.c
	libagesa-y += Proc/Mem/NB/PH/mnflowPh.c
	libagesa-y += Proc/Mem/NB/PH/mnidendimmPh.c
	libagesa-y += Proc/Mem/NB/PH/mnmctPh.c
	libagesa-y += Proc/Mem/NB/PH/mnPh.c
	libagesa-y += Proc/Mem/NB/PH/mnmctPh.c
	libagesa-y += Proc/Mem/NB/PH/mnflowPh.c
	libagesa-y += Proc/Mem/NB/PH/mnidendimmPh.c
	libagesa-y += Proc/Mem/NB/RB/mnidendimmRb.c
	libagesa-y += Proc/Mem/NB/RB/mnRb.c
	libagesa-y += Proc/Mem/NB/RB/mnflowRb.c
	libagesa-y += Proc/Mem/NB/RB/mnS3Rb.c

	libagesa-y += Proc/Mem/Ardk/DA/masda2.c
	libagesa-y += Proc/Mem/Ardk/DA/masda3.c
	libagesa-y += Proc/Mem/Ardk/DA/mauda3.c
	libagesa-y += Proc/Mem/Ardk/PH/mauPh3.c
	libagesa-y += Proc/Mem/Ardk/PH/masph3.c
	libagesa-y += Proc/Mem/Ardk/RB/mauRb3.c
	libagesa-y += Proc/Mem/Ardk/RB/masRb3.c

	libagesa-y += Proc/Mem/Ps/DA/mpuda3.c
	libagesa-y += Proc/Mem/Ps/DA/mpsda3.c
	libagesa-y += Proc/Mem/Ps/DA/mpsda2.c
	libagesa-y += Proc/Mem/Ps/PH/mpuph3.c
	libagesa-y += Proc/Mem/Ps/PH/mpsph3.c
	libagesa-y += Proc/Mem/Ps/RB/mpuRb3.c
	libagesa-y += Proc/Mem/Ps/RB/mpsRb3.c
endif

$(obj)/libagesa.fam15.a: $$(libagesa-objs)
	@printf "    AGESA        $(subst $(obj)/,,$(@))\n"
	ar rcs $@ $+

romstage-libs  += $(obj)/libagesa.fam15.a
ramstage-libs  += $(obj)/libagesa.fam15.a
