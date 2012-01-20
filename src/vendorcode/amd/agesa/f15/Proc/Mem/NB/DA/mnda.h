/* $NoKeywords:$ */
/**
 * @file
 *
 * mnda.h
 *
 * Northbridge DA
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

#ifndef _MNDA_H_
#define _MNDA_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_CHANNELS_PER_SOCKET_DA    2
#define MAX_DCTS_PER_NODE_DA          2
#define MAX_CHANNELS_PER_DCT_DA       1
#define MAX_NODES_SUPPORTED_DA        8

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
MemConstructNBBlockDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitNBDataDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDefaultsDA (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNInitializeMctDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNFinalizeMctDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNSendMrsCmdDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
memNAutoConfigDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemPNodeMemBoundaryDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   UINT32 *NodeSysLimit
  );

VOID
InitNBRegTableDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

VOID
MemNBeforeDramInitDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforePlatformSpecDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecificFormFactorInitDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNIsIdSupportedDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

BOOLEAN
MemNChangeAvgValue3DA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNChangeAvgValue8DA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNEnDLLShutDownDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNCmnGetSetFieldDA (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

VOID
MemNCapSpeedBatteryLifeDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
memNEnableTrainSequenceDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNPlatformSpecificFormFactorInitNi (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
#endif  /* _MNDA_H_ */


