/* $NoKeywords:$ */
/**
 * @file
 *
 * mnon.h
 *
 * Ontario Northbridge block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 48511 $ @e \$Date: 2011-03-09 13:53:13 -0700 (Wed, 09 Mar 2011) $
 *
 **/
/*
 *****************************************************************************
 *
 * Copyright (c) 2011, Advanced Micro Devices, Inc.
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

#ifndef _MNON_H_
#define _MNON_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CHANNELS_PER_SOCKET_ON    1
#define MAX_DCTS_PER_NODE_ON          1
#define MAX_CHANNELS_PER_DCT_ON       1
#define MAX_DIMMS_PER_CHANNEL_ON      2
#define MAX_NODES_SUPPORTED_ON        1
#define MAX_CS_PER_CHANNEL_ON         4

#define DEFAULT_WR_ODT_ON_ON 6
#define DEFAULT_RD_ODT_ON_ON 6

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
MemConstructNBBlockON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  CONST IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitDefaultsON (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemNSendMrsCmdON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAutoConfigON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNCSPerChannelON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNInitNBRegTableON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

VOID
MemNBeforeDQSTrainingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAfterDQSTrainingON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecificFormFactorInitON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNIsIdSupportedON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

UINT32
MemNCmnGetSetFieldON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

UINT32
MemNGetUmaSizeON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNBrdcstCheckON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

VOID
MemNTechBlockSwitchON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNStitchMemoryON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNHtMemMapInitON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSetMaxLatencyON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

BOOLEAN
MemNFinalizeMctON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
memNEnableTrainSequenceON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeNbFrequencyWrapON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  );

BOOLEAN
MemNOverrideRcvEnSeedON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

VOID
MemNGetMaxLatParamsClientON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxDlyForMaxRdLat,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

BOOLEAN
MemNForceRdDqsPhaseBON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *RdDqsDlyPtr
  );

BOOLEAN
MemNSetDqsODTON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNBeforeMemClkFreqValON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNResetRxFifoPtrON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );
#endif  /* _MNON_H_ */
