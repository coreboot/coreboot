#
# This file is part of the coreboot project.
#
# Copyright (C) 2011 Advanced Micro Devices, Inc.
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

ramstage-y += chip_name.c
ramstage-y += model_10_init.c

AGESA_ROOT = ../../../../vendorcode/amd/agesa/f10

#agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/F10IoCstate.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuIoCstate.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/ON/mnprotoon.c

agesa_lib_src  = $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/F10MicrocodePatch010000c4.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyInitEarlyTable.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuInitEarlyTable.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cahalt.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuC6State.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mm.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuCommonF10Utilities.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyEquivalenceTable.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mttml.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtrci3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCacheInit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mttdimbt.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtsdi3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mthdi.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mnfeat.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmStandardTraining.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuBist.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/mrdef.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmParallelTraining.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/LVDDR3/mflvddr3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10PowerPlane.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mnreg.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitReset.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/HT/htInitReset.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/CHINTLV/mfchi.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mt.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mnflow.c
agesa_lib_src += $(AGESA_ROOT)/Proc/IDS/Debug/IdsDebug.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuFeatureLeveling.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mttoptsrc.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/PARTRN/mfParallelTraining.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmEcc.c
agesa_lib_src += $(AGESA_ROOT)/Legacy/Proc/Dispatcher.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/MEMCLR/mfmemclr.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuDmi.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mttsrc.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mnmct.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htInterfaceNonCoherent.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuSrat.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdS3LateRestore.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Table.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htInterface.c
agesa_lib_src += $(AGESA_ROOT)/Proc/IDS/Perf/IdsPerf.c
agesa_lib_src += $(AGESA_ROOT)/Lib/amdlib.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCacheFlushOnHalt.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/minit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/INTLVRN/mfintlvrn.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/IDENDIMM/mfidendimm.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmflow.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ardk/ma.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/CommonInits.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuPstateLeveling.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitPost.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuBrandId.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mnphy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitEnv.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmConditionalPso.c

agesa_lib_src += $(AGESA_ROOT)/Proc/Common/CommonReturns.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuPowerMgmt.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitResume.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitEarly.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuFamilyTranslation.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuPostInit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mn.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuHwC1e.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuLateInit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/CreateStruct.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuWhea.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/CSINTLV/mfcsi.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/ECC/mfecc.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/DMI/mfDMI.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdLateRunApTask.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuPowerMgmtMultiSocket.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmUmaAlloc.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuPstateTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuPowerMgmtSingleSocket.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mt3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/EXCLUDIMM/mfdimmexclud.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmExcludeDimm.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htInterfaceGeneral.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/S3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuFeatures.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ps/mp.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mdef.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mtthrc.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuGeneralServices.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htNb.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuSlit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/ECC/mfemp.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmNodeInterleave.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mu.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyMsrTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/IDS/Control/IdsCtrl.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mttEdgeDetect.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitLate.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htFeat.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyLogicalIdTables.c
agesa_lib_src += $(AGESA_ROOT)/Legacy/Proc/hobTransfer.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuApicUtilities.c

#agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmMemRestore.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/NB/ON/mrnmcton.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/NB/OR/mrnprotoor.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/mrm.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/mruc.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/CPU/cpuRecovery.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/HT/htInitRecovery.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/NB/mrntrain3.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/DDR3/mrtsdi3.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/DDR3/mrt3.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/DDR3/mrttwl3.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/DDR3/mrtrci3.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/NB/mrn.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/mrttpos.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/NB/mrnmct.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/DDR3/mrtspd3.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/NB/mrndct.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Recovery/Mem/Tech/mrttsrc.c
#agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/ON/mnS3on.c
#agesa_lib_src += $(AGESA_ROOT)/Lib/helper.c

agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuEventLog.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htInterfaceCoherent.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/muc.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmLvDdr3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuMicrocodePatch.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/mttecc.c
agesa_lib_src += $(AGESA_ROOT)/Legacy/Proc/agesaCallouts.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/ODTHERMAL/mfodthermal.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/PARTRN/mfStandardTraining.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mndct.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmOnlineSpare.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuEarlyInit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/merrhdl.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdS3Save.c
agesa_lib_src += $(AGESA_ROOT)/Proc/IDS/Control/IdsLib.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtspd3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htMain.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/cpuWarmReset.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mttwl3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuPstateGather.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mtot3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/mmMemClr.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/TABLE/mftds.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/S3/mfs3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htNotify.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mnS3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/heapManager.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Tech/DDR3/mttecc3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/AmdInitMid.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/mntrain3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Common/S3SaveState.c

agesa_lib_src += $(AGESA_ROOT)/Proc/Common/S3RestoreState.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/OLSPARE/mfspr.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuMsgBasedC1e.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuCoreLeveling.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/PreserveMailbox.c

agesa_lib_src += $(AGESA_ROOT)/Proc/HT/NbCommon/htNbCoherent.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/NbCommon/htNbNonCoherent.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/NbCommon/htNbOptimization.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/NbCommon/htNbUtilities.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Fam10/htNbCoherentFam10.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Fam10/htNbOptimizationFam10.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Fam10/htNbFam10.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Fam10/htNbNonCoherentFam10.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Fam10/htNbUtilitiesFam10.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Fam10/htNbSystemFam10.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatDynamicDiscovery.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatSets.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatGanging.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatSublinks.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatNoncoherent.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatTrafficDistribution.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatOptimization.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htIds.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/Features/htFeatRouting.c

agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10PciTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10CacheDefaults.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10PowerCheck.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandId.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10Dmi.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10Pstate.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10SoftwareThermal.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10FeatureLeveling.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10Utilities.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10WheaInitDataTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10MsrTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10CacheFlushOnHalt.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10PowerMgmtSystemTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10EarlyInit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdG34.c

##agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdAm3.c
##agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdAsb2.c
##agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdC32.c
##agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdFr1207.c
##agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdS1g3.c
##agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10BrandIdS1g4.c

agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/F10MultiLinkPciTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/cpuF10HtPhyTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/F10RevDUtilities.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/F10RevDMsgBasedC1e.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/F10RevDHtAssist.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyPciTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyMicrocodePatchTables.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/RevD/HY/F10HyHtPhyTables.c

agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph1.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph6DoubloonUpper.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph2.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph6FullyConnected.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph3Line.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph6TwinTriangles.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph3Triangle.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph6TwistedLadder.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph4Degenerate.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph7FullyConnected.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph4FullyConnected.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph7TwistedLadder.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph4Kite.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph8DoubloonM.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph4Line.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph8FullyConnected.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph4Square.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph8Ladder.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph4Star.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph8TwinFullyFourWays.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph5FullyConnected.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph8TwistedLadder.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph5TwistedLadder.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph.c
agesa_lib_src += $(AGESA_ROOT)/Proc/HT/htGraph/htGraph6DoubloonLower.c

agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/HY/mmflowhy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Feat/NDINTLV/mfndi.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnS3hy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnhy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mndcthy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnflowhy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnidendimmhy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnmcthy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnothy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnParTrainHy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnphyhy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnprotohy.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/NB/HY/mnreghy.c

agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ps/HY/mprhy3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ps/HY/mpuhy3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ps/HY/mpshy3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ardk/HY/marhy3.c
agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Ardk/HY/mauhy3.c

agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Feature/cpuHtAssist.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/F10PmNbCofVidInit.c
agesa_lib_src += $(AGESA_ROOT)/Proc/CPU/Family/0x10/F10PmNbPstateInit.c

##C32
#agesa_lib_src += $(AGESA_ROOT)/Proc/Mem/Main/C32/mmflowC32.c

romstage-y += $(agesa_lib_src)
ramstage-y += $(agesa_lib_src)

subdirs-y += ../../mtrr
subdirs-y += ../../../x86/tsc
subdirs-y += ../../../x86/lapic
subdirs-y += ../../../x86/cache
subdirs-y += ../../../x86/mtrr
subdirs-y += ../../../x86/pae
subdirs-y += ../../../x86/smm

