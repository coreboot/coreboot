/* $NoKeywords:$ */
/**
 * @file
 *
 * mnor.h
 *
 * Northbridge Orochi
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 60556 $ @e \$Date: 2011-10-17 20:19:58 -0600 (Mon, 17 Oct 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
  *
  * ***************************************************************************
  *
 */

#ifndef _MNOR_H_
#define _MNOR_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CHANNELS_PER_SOCKET_OR    4
#define MAX_DCTS_PER_NODE_OR          2
#define MAX_CHANNELS_PER_DCT_OR       1
#define MAX_NODES_SUPPORTED_OR        8

#define DEFAULT_WR_ODT_OR 6
#define DEFAULT_RD_ODT_OR 6

#define GetBufDatDly     0
#define GetBufDatDlySkew 1

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
/// Data Structure of Parameters for MR0 PPD set during S3 resume
typedef struct {
  UINT16 MR0Value;                 ///< MRO Value saved during memory initialization
  UINT16 ChipSelEnMap;             ///< Bitmap of Enabled Chip Select per DCT
} MR0_DATA_STRUCT;

typedef MR0_DATA_STRUCT (*MR0_DATA_ARRAY_PTR)[MAX_NODES_SUPPORTED_OR][MAX_DCTS_PER_NODE_OR];
/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
MemConstructNBBlockOR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitNBDataOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDefaultsOR (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNFinalizeMctOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAutoConfigOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitPhyCompOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNInitNBRegTableOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

UINT8
MemNGetSocketRelativeChannelOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Dct,
  IN       UINT8 Channel
  );

BOOLEAN
MemNIsIdSupportedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

UINT32
MemNCmnGetSetFieldOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

BOOLEAN
memNEnableTrainSequenceOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTechBlockSwitchOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNScrubberErratumOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNAfterDQSTrainingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNDataTxFifoWrDlyOverrideOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNCapSpeedBatteryLifeOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetMaxLatParamsOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

VOID
MemNSetMaxLatencyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

BOOLEAN
MemNExitPhyAssistedTrainingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNOverrideRcvEnSeedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

BOOLEAN
MemNOverrideRcvEnSeedPassNOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedTotal
  );

BOOLEAN
MemNOverrideWLSeedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

BOOLEAN
MemNAfterMemClkFreqChgOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );


BOOLEAN
MemNTrainWlPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Dimm
  );

BOOLEAN
MemNTrainWlPerNibbleAdjustWLDlyOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Delay
  );

VOID
MemNBeforeDQSTrainingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNTrainRxEnPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  );

BOOLEAN
MemNTrainRxEnAdjustDlyPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *RcvEnDly
  );

BOOLEAN
MemNTrainRxEnGetAvgDlyPerNibbleOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNInitPerNibbleTrnOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNTrainWlPerNibbleSeedOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsDly
  );

BOOLEAN
MemNTrainingNibbleZeroOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  );

BOOLEAN
MemNBeforeSetCsTriOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *CsTriBitmap
  );

BOOLEAN
MemNEnableParityAfterMemRstOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNProgOdtControlOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNBeforeDramInitOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPFenceAdjustOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  );

BOOLEAN
MemNReleaseNbPstateOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

VOID
MemNSaveMR0Or (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 MrsAddress
  );

UINT32
MemNCalBufDatDelaySkewOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8   GetDelay
  );

VOID
MemNPowerDownCtlOr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAdjustWrDqsBeforeSeedScalingOr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *WrDqsBias
  );

#endif  /* _MNOR_H_ */


