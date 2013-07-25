/* $NoKeywords:$ */
/**
 * @file
 *
 * mnkb.h
 *
 * Northbridge KB
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/KB)
 * @e \$Revision: 87494 $ @e \$Date: 2013-02-04 12:06:47 -0600 (Mon, 04 Feb 2013) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * ***************************************************************************
  *
 */

#ifndef _MNKB_H_
#define _MNKB_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DCTS_PER_NODE_KB          1
#define MAX_CHANNELS_PER_DCT_KB       1
#define MAX_NODES_SUPPORTED_KB        1
#define MAX_CS_PER_CHANNEL_KB         4

#define DEFAULT_WR_ODT_KB 6
#define DEFAULT_RD_ODT_KB 6
#define DEFAULT_RD_ODT_TRNONDLY_KB 0

#define TOTAL_BIT_TIMES_2D_RD_TRAINING_KB      (64 * 1024)
#define TOTAL_BIT_TIMES_2D_WR_TRAINING_KB      (64 * 1024)
/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
MemConstructNBBlockKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitNBDataKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDefaultsKB (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNInitializeMctKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAutoConfigKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitPhyCompKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNInitNBRegTableKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

BOOLEAN
MemNIsIdSupportedKB (
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

UINT32
MemNCmnGetSetFieldKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

BOOLEAN
memNEnableTrainSequenceKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTechBlockSwitchKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAfterDQSTrainingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNCapSpeedBatteryLifeKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetMaxLatParamsKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

VOID
MemNSetMaxLatencyKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

BOOLEAN
MemNExitPhyAssistedTrainingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNOverrideRcvEnSeedKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

VOID
MemNBeforeDQSTrainingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT16
MemNGetMemClkFreqInCurrentContextKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramNbPstateDependentRegistersKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAdjustPllLockTimeKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllLockTime
  );

BOOLEAN
MemNOverrideWLSeedKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

BOOLEAN
MemNFinalizeMctKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNHtMemMapInitKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetUmaSizeKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforeDramInitKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNCSIntLvLowAddrAdjKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *LowBit
  );

VOID
MemNAllocateC6StorageKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPFenceAdjustKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  );

BOOLEAN
MemNReleaseNbPstateKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNMemPstateStageChangeKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNProgramFence2RxDllKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Fence2Data
  );

BOOLEAN
MemNRdDqsDlyRestartChkKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Center
  );

BOOLEAN
MemNHookBfWrDatTrnKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  );

VOID
MemNSetOtherTimingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPowerDownCtlKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforePlatformSpecKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNWLMR1KB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Value
  );

BOOLEAN
MemNProgramPOdtOffKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Value
  );

BOOLEAN
memNRelocatePscTableEntryByMotherBoardLayerKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *TblEntryPtr
  );

BOOLEAN
MemNAdjustHwRcvEnSeedGrossKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Value
  );

BOOLEAN
MemNPowerSavingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNForceEccSymbolSizeKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Size
  );

BOOLEAN
MemNCalcWrDqDqsEarlyKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNConfigureDctTrainingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramNonSeqDependentRegistersKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNConfigureDctNormalKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNSetMaxRdLatBasedOnSeededRxEnDlyKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNPhyPowerSavingMPstateKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNDisableScrubberKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNRestoreScrubberKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramCycTimingsKB (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDisableMemHoleMappingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNRestoreMemHoleMappingKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNPhyInitVrefKB (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );
#endif  /* _MNKB_H_ */
