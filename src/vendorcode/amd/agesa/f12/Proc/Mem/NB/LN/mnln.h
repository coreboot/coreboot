/* $NoKeywords:$ */
/**
 * @file
 *
 * mnln.h
 *
 * Llano Northbridge block
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 48400 $ @e \$Date: 2011-03-08 16:28:12 +0800 (Tue, 08 Mar 2011) $
 *
 **/
/*****************************************************************************
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
  * ***************************************************************************
  *
 */

#ifndef _MNLN_H_
#define _MNLN_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CHANNELS_PER_SOCKET_LN    2
#define MAX_DCTS_PER_NODE_LN          2
#define MAX_CHANNELS_PER_DCT_LN       1
#define MAX_DIMMS_PER_CHANNEL_LN      2
#define MAX_NODES_SUPPORTED_LN        1

#define DEFAULT_WR_ODT_ON_LN 6
#define DEFAULT_RD_ODT_ON_LN 6
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
MemConstructNBBlockLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitDefaultsLN (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemNSendMrsCmdLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAutoConfigLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNInitNBRegTableLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

VOID
MemNBeforeDQSTrainingLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAfterDQSTrainingLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecificFormFactorInitLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNIsIdSupportedLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

UINT32
MemNCmnGetSetFieldLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

UINT32
MemNGetUmaSizeLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNFinalizeMctLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTechBlockSwitchLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNHtMemMapInitLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT8
MemNCSPerChannelLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSetMaxLatencyLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

VOID
MemNEnableSwapIntlvRgnLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN          UINT32 Base,
  IN          UINT32 Limit
  );

BOOLEAN
memNEnableTrainSequenceLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeNbFrequencyWrapLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 NBPstate
  );

VOID
MemNGetMaxLatParamsClientLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxDlyForMaxRdLat,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

BOOLEAN
MemNDisLowPwrDrvStrLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

UINT32
MemNGetMR0WRLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOverridePrevPassRcvEnDlyLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PrevPassRcvEnDly
  );

#endif  /* _MNLN_H_ */


