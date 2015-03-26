#*****************************************************************************
#
# This file is part of the coreboot project.
#
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
# Foundation, Inc.
#
#*****************************************************************************

# AGESA V5 Files
AGESA_ROOT = src/vendorcode/amd/agesa/f10

AGESA_INC  = -I$(src)/mainboard/$(MAINBOARDDIR)
AGESA_INC += -I$(AGESA_ROOT)
AGESA_INC += -I$(AGESA_ROOT)/../common
AGESA_INC += -I$(AGESA_ROOT)/Include
AGESA_INC += -I$(AGESA_ROOT)/Lib
AGESA_INC += -I$(AGESA_ROOT)/Legacy
AGESA_INC += -I$(AGESA_ROOT)/Proc/Common
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT
AGESA_INC += -I$(AGESA_ROOT)/Proc/HT/NbCommon
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Feature
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/CPU/Family/0x10
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Feat/IDENDIMM/
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/NB/HY
AGESA_INC += -I$(AGESA_ROOT)/Proc/Mem/Main
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family
AGESA_INC += -I$(AGESA_ROOT)/Proc/IDS/Family/0x10
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

#libagesa-y += Proc/CPU/Family/0x10/F10IoCstate.c
#libagesa-y += Proc/CPU/Feature/cpuIoCstate.c
#libagesa-y += Proc/Mem/NB/ON/mnprotoon.c

libagesa-y  = Proc/CPU/Family/0x10/RevD/F10MicrocodePatch010000c4.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyInitEarlyTable.c
libagesa-y += Proc/CPU/cpuInitEarlyTable.c
libagesa-y += Proc/CPU/cahalt.c
libagesa-y += Proc/CPU/Feature/cpuC6State.c
libagesa-y += Proc/Mem/Main/mm.c
libagesa-y += Proc/CPU/Family/0x10/cpuCommonF10Utilities.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyEquivalenceTable.c
libagesa-y += Proc/Mem/Tech/mttml.c
libagesa-y += Proc/Mem/Tech/DDR3/mtrci3.c
libagesa-y += Proc/CPU/Feature/cpuCacheInit.c
libagesa-y += Proc/Mem/Tech/mttdimbt.c
libagesa-y += Proc/Mem/Tech/DDR3/mtsdi3.c
libagesa-y += Proc/Mem/Tech/mthdi.c
libagesa-y += Proc/Mem/NB/mnfeat.c
libagesa-y += Proc/Mem/Main/mmStandardTraining.c
libagesa-y += Proc/CPU/cpuBist.c
libagesa-y += Proc/Recovery/Mem/mrdef.c
libagesa-y += Proc/Mem/Main/mmParallelTraining.c
libagesa-y += Proc/Mem/Feat/LVDDR3/mflvddr3.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PowerPlane.c
libagesa-y += Proc/Mem/NB/mnreg.c
#libagesa-y += Proc/Common/AmdInitReset.c
#libagesa-y += Proc/Recovery/HT/htInitReset.c
libagesa-y += Proc/Mem/Feat/CHINTLV/mfchi.c
libagesa-y += Proc/Mem/Tech/mt.c
libagesa-y += Proc/Mem/NB/mnflow.c
libagesa-y += Proc/IDS/Debug/IdsDebug.c
libagesa-y += Proc/CPU/Feature/cpuFeatureLeveling.c
libagesa-y += Proc/Mem/Tech/mttoptsrc.c
libagesa-y += Proc/Mem/Feat/PARTRN/mfParallelTraining.c
libagesa-y += Proc/Mem/Main/mmEcc.c
libagesa-y += Legacy/Proc/Dispatcher.c
libagesa-y += Proc/Mem/Feat/MEMCLR/mfmemclr.c
libagesa-y += Proc/CPU/Feature/cpuDmi.c
libagesa-y += Proc/Mem/Tech/mttsrc.c
libagesa-y += Proc/Mem/NB/mnmct.c
libagesa-y += Proc/HT/htInterfaceNonCoherent.c
libagesa-y += Proc/CPU/Feature/cpuSrat.c
libagesa-y += Proc/Common/AmdS3LateRestore.c
libagesa-y += Proc/CPU/Table.c
libagesa-y += Proc/HT/htInterface.c
libagesa-y += Proc/IDS/Perf/IdsPerf.c
libagesa-y += Lib/amdlib.c
libagesa-y += Proc/CPU/Feature/cpuCacheFlushOnHalt.c
libagesa-y += Proc/Mem/Main/minit.c
libagesa-y += Proc/Mem/Feat/INTLVRN/mfintlvrn.c
libagesa-y += Proc/Mem/Feat/IDENDIMM/mfidendimm.c
libagesa-y += Proc/Mem/Main/mmflow.c
libagesa-y += Proc/Mem/Ardk/ma.c
libagesa-y += Proc/Common/CommonInits.c
libagesa-y += Proc/CPU/Feature/cpuPstateLeveling.c
libagesa-y += Proc/Common/AmdInitPost.c
libagesa-y += Proc/CPU/cpuBrandId.c
libagesa-y += Proc/Mem/NB/mnphy.c
libagesa-y += Proc/Common/AmdInitEnv.c
libagesa-y += Proc/Mem/Main/mmConditionalPso.c

libagesa-y += Proc/Common/CommonReturns.c
libagesa-y += Proc/CPU/cpuPowerMgmt.c
libagesa-y += Proc/Common/AmdInitResume.c
libagesa-y += Proc/Common/AmdInitEarly.c
libagesa-y += Proc/CPU/cpuFamilyTranslation.c
libagesa-y += Proc/CPU/cpuPostInit.c
libagesa-y += Proc/Mem/NB/mn.c
libagesa-y += Proc/CPU/Feature/cpuHwC1e.c
libagesa-y += Proc/CPU/cpuLateInit.c
libagesa-y += Proc/Common/CreateStruct.c
libagesa-y += Proc/CPU/Feature/cpuWhea.c
libagesa-y += Proc/Mem/Feat/CSINTLV/mfcsi.c
libagesa-y += Proc/Mem/Feat/ECC/mfecc.c
libagesa-y += Proc/Mem/Feat/DMI/mfDMI.c
libagesa-y += Proc/Common/AmdLateRunApTask.c
libagesa-y += Proc/CPU/cpuPowerMgmtMultiSocket.c
libagesa-y += Proc/Mem/Main/mmUmaAlloc.c
libagesa-y += Proc/CPU/Feature/cpuPstateTables.c
libagesa-y += Proc/CPU/cpuPowerMgmtSingleSocket.c
libagesa-y += Proc/Mem/Tech/DDR3/mt3.c
libagesa-y += Proc/Mem/Feat/EXCLUDIMM/mfdimmexclud.c
libagesa-y += Proc/Mem/Main/mmExcludeDimm.c
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
libagesa-y += Proc/Mem/Main/mmNodeInterleave.c
libagesa-y += Proc/Mem/Main/mu.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyMsrTables.c
libagesa-y += Proc/IDS/Control/IdsCtrl.c
libagesa-y += Proc/Mem/Tech/mttEdgeDetect.c
libagesa-y += Proc/Common/AmdInitLate.c
libagesa-y += Proc/HT/htFeat.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyLogicalIdTables.c
libagesa-y += Legacy/Proc/hobTransfer.c
libagesa-y += Proc/CPU/cpuApicUtilities.c

#libagesa-y += Proc/Mem/Main/mmMemRestore.c
#libagesa-y += Proc/Recovery/Mem/NB/ON/mrnmcton.c
#libagesa-y += Proc/Recovery/Mem/NB/OR/mrnprotoor.c
#libagesa-y += Proc/Recovery/Mem/mrm.c
#libagesa-y += Proc/Recovery/Mem/mruc.c
#libagesa-y += Proc/Recovery/CPU/cpuRecovery.c
#libagesa-y += Proc/Recovery/HT/htInitRecovery.c
#libagesa-y += Proc/Recovery/Mem/NB/mrntrain3.c
#libagesa-y += Proc/Recovery/Mem/Tech/DDR3/mrtsdi3.c
#libagesa-y += Proc/Recovery/Mem/Tech/DDR3/mrt3.c
#libagesa-y += Proc/Recovery/Mem/Tech/DDR3/mrttwl3.c
#libagesa-y += Proc/Recovery/Mem/Tech/DDR3/mrtrci3.c
#libagesa-y += Proc/Recovery/Mem/NB/mrn.c
#libagesa-y += Proc/Recovery/Mem/Tech/mrttpos.c
#libagesa-y += Proc/Recovery/Mem/NB/mrnmct.c
#libagesa-y += Proc/Recovery/Mem/Tech/DDR3/mrtspd3.c
#libagesa-y += Proc/Recovery/Mem/NB/mrndct.c
#libagesa-y += Proc/Recovery/Mem/Tech/mrttsrc.c
#libagesa-y += Proc/Mem/NB/ON/mnS3on.c

libagesa-y += Proc/CPU/cpuEventLog.c
libagesa-y += Proc/HT/htInterfaceCoherent.c
libagesa-y += Proc/Mem/Main/muc.c
libagesa-y += Proc/Mem/Main/mmLvDdr3.c
libagesa-y += Proc/CPU/cpuMicrocodePatch.c
libagesa-y += Proc/Mem/Tech/mttecc.c
libagesa-y += Legacy/Proc/agesaCallouts.c
libagesa-y += Proc/Mem/Feat/ODTHERMAL/mfodthermal.c
libagesa-y += Proc/Mem/Feat/PARTRN/mfStandardTraining.c
libagesa-y += Proc/Mem/NB/mndct.c
libagesa-y += Proc/Mem/Main/mmOnlineSpare.c
libagesa-y += Proc/CPU/cpuEarlyInit.c
libagesa-y += Proc/Mem/Main/merrhdl.c
libagesa-y += Proc/Common/AmdS3Save.c
libagesa-y += Proc/IDS/Control/IdsLib.c
libagesa-y += Proc/Mem/Tech/DDR3/mtspd3.c
libagesa-y += Proc/HT/htMain.c
libagesa-y += Proc/CPU/cpuWarmReset.c
libagesa-y += Proc/Mem/Tech/DDR3/mttwl3.c
libagesa-y += Proc/CPU/Feature/cpuPstateGather.c
libagesa-y += Proc/Mem/Tech/DDR3/mtot3.c
libagesa-y += Proc/Mem/Main/mmMemClr.c
libagesa-y += Proc/Mem/Feat/TABLE/mftds.c
libagesa-y += Proc/Mem/Feat/S3/mfs3.c
libagesa-y += Proc/HT/htNotify.c
libagesa-y += Proc/Mem/NB/mnS3.c
libagesa-y += Proc/CPU/heapManager.c
libagesa-y += Proc/Mem/Tech/DDR3/mttecc3.c
libagesa-y += Proc/Common/AmdInitMid.c
libagesa-y += Proc/Mem/NB/mntrain3.c
libagesa-y += Proc/Common/S3SaveState.c

libagesa-y += Proc/Common/S3RestoreState.c
libagesa-y += Proc/Mem/Feat/OLSPARE/mfspr.c
libagesa-y += Proc/CPU/Feature/cpuMsgBasedC1e.c
libagesa-y += Proc/CPU/Feature/cpuCoreLeveling.c
libagesa-y += Proc/CPU/Feature/PreserveMailbox.c

libagesa-y += Proc/HT/NbCommon/htNbCoherent.c
libagesa-y += Proc/HT/NbCommon/htNbNonCoherent.c
libagesa-y += Proc/HT/NbCommon/htNbOptimization.c
libagesa-y += Proc/HT/NbCommon/htNbUtilities.c
libagesa-y += Proc/HT/Fam10/htNbCoherentFam10.c
libagesa-y += Proc/HT/Fam10/htNbOptimizationFam10.c
libagesa-y += Proc/HT/Fam10/htNbFam10.c
libagesa-y += Proc/HT/Fam10/htNbNonCoherentFam10.c
libagesa-y += Proc/HT/Fam10/htNbUtilitiesFam10.c
libagesa-y += Proc/HT/Fam10/htNbSystemFam10.c
libagesa-y += Proc/HT/Features/htFeatDynamicDiscovery.c
libagesa-y += Proc/HT/Features/htFeatSets.c
libagesa-y += Proc/HT/Features/htFeatGanging.c
libagesa-y += Proc/HT/Features/htFeatSublinks.c
libagesa-y += Proc/HT/Features/htFeatNoncoherent.c
libagesa-y += Proc/HT/Features/htFeatTrafficDistribution.c
libagesa-y += Proc/HT/Features/htFeatOptimization.c
libagesa-y += Proc/HT/Features/htIds.c
libagesa-y += Proc/HT/Features/htFeatRouting.c

libagesa-y += Proc/CPU/Family/0x10/cpuF10PciTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10CacheDefaults.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PowerCheck.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandId.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Dmi.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Pstate.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10SoftwareThermal.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10FeatureLeveling.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10Utilities.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10WheaInitDataTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10MsrTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10CacheFlushOnHalt.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10PowerMgmtSystemTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10EarlyInit.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdG34.c

##libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdAm3.c
##libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdAsb2.c
##libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdC32.c
##libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdFr1207.c
##libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdS1g3.c
##libagesa-y += Proc/CPU/Family/0x10/cpuF10BrandIdS1g4.c

libagesa-y += Proc/CPU/Family/0x10/F10MultiLinkPciTables.c
libagesa-y += Proc/CPU/Family/0x10/cpuF10HtPhyTables.c
libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDUtilities.c
libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDMsgBasedC1e.c
libagesa-y += Proc/CPU/Family/0x10/RevD/F10RevDHtAssist.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyPciTables.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyMicrocodePatchTables.c
libagesa-y += Proc/CPU/Family/0x10/RevD/HY/F10HyHtPhyTables.c

libagesa-y += Proc/HT/htGraph/htGraph1.c
libagesa-y += Proc/HT/htGraph/htGraph6DoubloonUpper.c
libagesa-y += Proc/HT/htGraph/htGraph2.c
libagesa-y += Proc/HT/htGraph/htGraph6FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph3Line.c
libagesa-y += Proc/HT/htGraph/htGraph6TwinTriangles.c
libagesa-y += Proc/HT/htGraph/htGraph3Triangle.c
libagesa-y += Proc/HT/htGraph/htGraph6TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph4Degenerate.c
libagesa-y += Proc/HT/htGraph/htGraph7FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph4FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph7TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph4Kite.c
libagesa-y += Proc/HT/htGraph/htGraph8DoubloonM.c
libagesa-y += Proc/HT/htGraph/htGraph4Line.c
libagesa-y += Proc/HT/htGraph/htGraph8FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph4Square.c
libagesa-y += Proc/HT/htGraph/htGraph8Ladder.c
libagesa-y += Proc/HT/htGraph/htGraph4Star.c
libagesa-y += Proc/HT/htGraph/htGraph8TwinFullyFourWays.c
libagesa-y += Proc/HT/htGraph/htGraph5FullyConnected.c
libagesa-y += Proc/HT/htGraph/htGraph8TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph5TwistedLadder.c
libagesa-y += Proc/HT/htGraph/htGraph.c
libagesa-y += Proc/HT/htGraph/htGraph6DoubloonLower.c

libagesa-y += Proc/Mem/Main/HY/mmflowhy.c
libagesa-y += Proc/Mem/Feat/NDINTLV/mfndi.c
libagesa-y += Proc/Mem/NB/HY/mnS3hy.c
libagesa-y += Proc/Mem/NB/HY/mnhy.c
libagesa-y += Proc/Mem/NB/HY/mndcthy.c
libagesa-y += Proc/Mem/NB/HY/mnflowhy.c
libagesa-y += Proc/Mem/NB/HY/mnidendimmhy.c
libagesa-y += Proc/Mem/NB/HY/mnmcthy.c
libagesa-y += Proc/Mem/NB/HY/mnothy.c
libagesa-y += Proc/Mem/NB/HY/mnParTrainHy.c
libagesa-y += Proc/Mem/NB/HY/mnphyhy.c
libagesa-y += Proc/Mem/NB/HY/mnprotohy.c
libagesa-y += Proc/Mem/NB/HY/mnreghy.c

libagesa-y += Proc/Mem/Ps/HY/mprhy3.c
libagesa-y += Proc/Mem/Ps/HY/mpuhy3.c
libagesa-y += Proc/Mem/Ps/HY/mpshy3.c
libagesa-y += Proc/Mem/Ardk/HY/marhy3.c
libagesa-y += Proc/Mem/Ardk/HY/mauhy3.c

libagesa-y += Proc/CPU/Feature/cpuHtAssist.c
libagesa-y += Proc/CPU/Family/0x10/F10PmNbCofVidInit.c
libagesa-y += Proc/CPU/Family/0x10/F10PmNbPstateInit.c

##C32
#libagesa-y += Proc/Mem/Main/C32/mmflowC32.c

$(obj)/libagesa.fam10.a: $$(libagesa-objs)
	@printf "    AGESA        $(subst $(obj)/,,$(@))\n"
	ar rcs $@ $+

romstage-libs  += $(obj)/libagesa.fam10.a
ramstage-libs  += $(obj)/libagesa.fam10.a
