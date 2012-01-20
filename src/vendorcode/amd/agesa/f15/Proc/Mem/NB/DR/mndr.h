/* $NoKeywords:$ */
/**
 * @file
 *
 * mndr.h
 *
 * Northbridge DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
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

#ifndef _MNDR_H_
#define _MNDR_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CHANNELS_PER_SOCKET_DR    2
#define MAX_DCTS_PER_NODE_DR          2
#define MAX_CHANNELS_PER_DCT_DR       1
#define MAX_NODES_SUPPORTED_DR        8

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
MemConstructNBBlockDR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitNBDataDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDefaultsDR (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNInitializeMctDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNFinalizeMctDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSendMrsCmdDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
memNAutoConfigDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemPNodeMemBoundaryDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT32 *NodeSysLimit
  );

VOID
InitNBRegTableDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

BOOLEAN
MemNIsIdSupportedDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

VOID
MemNBeforeDramInitDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforePlatformSpecDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecificFormFactorInitDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemPPhyFenceTrainingDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNCmnGetSetFieldDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

VOID
MemNPFenceAdjustDr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  );

VOID
MemNProgramCycTimingsDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
memNEnableTrainSequenceDr (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
#endif  /* _MNDR_H_ */
