/* $NoKeywords:$ */
/**
 * @file
 *
 * mntn.h
 *
 * Northbridge TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/TN)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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

#ifndef _MNTN_H_
#define _MNTN_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DCTS_PER_NODE_TN          2
#define MAX_CHANNELS_PER_DCT_TN       1
#define MAX_NODES_SUPPORTED_TN        1
#define MAX_CS_PER_CHANNEL_TN         4

#define DEFAULT_WR_ODT_TN 6
#define DEFAULT_RD_ODT_TN 6
#define DEFAULT_RD_ODT_TRNONDLY_TN 0
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
MemConstructNBBlockTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitNBDataTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDefaultsTN (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNInitializeMctTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAutoConfigTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitPhyCompTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNInitNBRegTableTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

BOOLEAN
MemNIsIdSupportedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

UINT32
MemNCmnGetSetFieldTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

BOOLEAN
memNEnableTrainSequenceTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTechBlockSwitchTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAfterDQSTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNCapSpeedBatteryLifeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetMaxLatParamsTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

VOID
MemNSetMaxLatencyTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

BOOLEAN
MemNExitPhyAssistedTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNOverrideRcvEnSeedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

VOID
MemNBeforeDQSTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramNbPstateDependentRegistersTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAdjustPllLockTimeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllLockTime
  );

BOOLEAN
MemNOverrideWLSeedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

BOOLEAN
MemNFinalizeMctTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNHtMemMapInitTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetUmaSizeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforeDramInitTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNCSIntLvLowAddrAdjTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *LowBit
  );

VOID
MemNAllocateC6StorageTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPFenceAdjustTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  );

BOOLEAN
MemNReleaseNbPstateTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNMemPstateStageChangeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNProgramFence2RxDllTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Fence2Data
  );

VOID
MemNAdjustNBPstateVolTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNRdDqsDlyRestartChkTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Center
  );

BOOLEAN
MemNHookBfWrDatTrnTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  );

VOID
MemNSetOtherTimingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPowerDownCtlTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforePlatformSpecTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNWLMR1TN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Value
  );
#endif  /* _MNTN_H_ */
