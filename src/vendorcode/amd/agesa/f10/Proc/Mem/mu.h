/**
 * @file
 *
 * mu.h
 *
 * Utility support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem)
 * @e \$Revision: 44323 $ @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
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
  * 
  * ***************************************************************************
  *
 */

#ifndef _MU_H_
#define _MU_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#ifndef PSO_ENTRY
  #define PSO_ENTRY UINT8
#endif

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/// Test patterns for DQS training
typedef enum {
  TestPattern0,             ///< Test pattern used in first pass of receiver enable training
  TestPattern1,             ///< Test pattern used in first pass of receiver enable training
  TestPattern2,             ///< Test pattern used in second pass of receiver enable training
  TestPatternJD1B,          ///< 72-bit test pattern used in position training (ganged mode)
  TestPatternJD1A,          ///< 72-bit test pattern used in position training
  TestPatternJD256B,        ///< 256-bit test pattern used in position training (ganged mode)
  TestPatternJD256A,        ///< 256-bit test pattern used in position training
  TestPatternML,            ///< Test pattern used in first pass of max latency training
  TestPattern3,             ///< Test pattern used in first pass of receiver enable training
  TestPattern4              ///< Test pattern used in first pass of receiver enable training
} TRAIN_PATTERN;

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
MemUWriteCachelines (
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemUReadCachelines (
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemUDummyCLRead (
  IN       UINT32 Address
  );

VOID
MemUFlushPattern (
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );


VOID
MemUFillTrainPattern (
  IN       TRAIN_PATTERN Pattern,
  IN       UINT8 Buffer[],
  IN       UINT16 Size
  );

UINT32
MemUSetUpperFSbase (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUSetTargetWTIO (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUResetTargetWTIO (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUProcIOClFlush (
  IN       UINT32 Address,
  IN       UINT16 ClCount,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

UINT16
MemUCompareTestPattern (
  IN       UINT8 Buffer[],
  IN       UINT8 Pattern[],
  IN       UINT16 ByteCount
  );

VOID
MemUWait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
MemUGetWrLvNblErr (
  IN OUT   UINT16 *ErrBitmap,
  IN       UINT32 TestAddr,
  IN       UINT16 ClCount
  );

VOID
AlignPointerTo16Byte (
  IN OUT   UINT8 **BufferPtrPtr
  );

VOID *
FindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       PSO_ENTRY EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  );

UINT8
GetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN UINT8 ChannelID
  );

UINT8
GetMaxChannelsPerSocket (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
GetMaxCSPerChannel (
  IN      PSO_TABLE *PlatformMemoryConfiguration,
  IN      UINT8 SocketID,
  IN      UINT8 ChannelID
  );

UINT8
GetSpdSocketIndex (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
GetSpdChannelIndex (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN UINT8 SocketID,
  IN UINT8 ChannelID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
GetVarMtrrHiMsk (
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

#endif  /* _MU_H_ */


