/* $NoKeywords:$ */
/**
 * @file
 *
 * muc.c
 *
 * Utility functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "cpuServices.h"
#include "amdlib.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "mport.h"
#include "mu.h"
#include "cpuFamilyTranslation.h"
#include "cpuCacheInit.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MUC_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
CONST UINT32 Pattern2[16] = {
  0x12345678, 0x87654321, 0x23456789, 0x98765432,
  0x59385824, 0x30496724, 0x24490795, 0x99938733,
  0x40385642, 0x38465245, 0x29432163, 0x05067894,
  0x12349045, 0x98723467, 0x12387634, 0x34587623
};

CONST UINT32 MaxLatPat[48] = {
  0x6E0E3FAC, 0x0C3CFF52,
  0x4A688181, 0x49C5B613,
  0x7C780BA6, 0x5C1650E3,
  0x0C4F9D76, 0x0C6753E6,
  0x205535A5, 0xBABFB6CA,
  0x610E6E5F, 0x0C5F1C87,
  0x488493CE, 0x14C9C383,
  0xF5B9A5CD, 0x9CE8F615,

  0xAAD714B5, 0xC38F1B4C,
  0x72ED647C, 0x669F7562,
  0x5233F802, 0x4A898B30,
  0x10A40617, 0x3326B465,
  0x55386E04, 0xC807E3D3,
  0xAB49E193, 0x14B4E63A,
  0x67DF2495, 0xEA517C45,
  0x7624CE51, 0xF8140C51,

  0x4824BD23, 0xB61DD0C9,
  0x072BCFBE, 0xE8F3807D,
  0x919EA373, 0x25E30C47,
  0xFEB12958, 0x4DA80A5A,
  0xE9A0DDF8, 0x792B0076,
  0xE81C73DC, 0xF025B496,
  0x1DB7E627, 0x808594FE,
  0x82668268, 0x655C7783
};

CONST UINT8 PatternJD[9] = {0x44, 0xA6, 0x38, 0x4F, 0x4B, 0x2E, 0xEF, 0xD5, 0x54};

CONST UINT8 PatternJD_256[256] = {
  0x00, 0xFF, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,
  0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF,
  0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0xFF, 0xFF,
  0xFF, 0xFF, 0x00, 0xF7, 0x08, 0xF7, 0x00, 0xFF,
  0x00, 0xF7, 0x00, 0xFF, 0x00, 0xF7, 0x00, 0xF7,
  0x08, 0xF7, 0x08, 0xFF, 0x00, 0xFF, 0x08, 0xFF,
  0x00, 0xFF, 0x08, 0xFF, 0x08, 0xF7, 0xFB, 0x04,
  0xFB, 0xFB, 0x04, 0xFB, 0xFB, 0xFB, 0x04, 0xFB,
  0xFB, 0xFB, 0xFB, 0x04, 0xFB, 0x04, 0x04, 0xFB,
  0x04, 0x04, 0x04, 0xFB, 0x04, 0x04, 0x04, 0x04,
  0xFB, 0x7F, 0x80, 0x7F, 0x00, 0xFF, 0x00, 0x7F,
  0x00, 0xFF, 0x00, 0x7F, 0x00, 0x7F, 0x80, 0x7F,
  0x80, 0xFF, 0x00, 0xFF, 0x80, 0xFF, 0x00, 0xFF,
  0x80, 0xFF, 0x80, 0x7F, 0xBF, 0x40, 0xBF, 0xBF,
  0x40, 0xBF, 0xBF, 0xBF, 0x40, 0xBF, 0xBF, 0xBF,
  0xBF, 0x40, 0xBF, 0x40, 0x40, 0xBF, 0x40, 0x40,
  0x40, 0xBF, 0x40, 0x40, 0x40, 0x40, 0xBF, 0xFD,
  0x02, 0xFD, 0x00, 0xFF, 0x00, 0xFD, 0x00, 0xFF,
  0x00, 0xFD, 0x00, 0xFD, 0x02, 0xFD, 0x02, 0xFF,
  0x00, 0xFF, 0x02, 0xFF, 0x00, 0xFF, 0x02, 0xFF,
  0x02, 0xFD, 0xFE, 0x01, 0xFE, 0xFE, 0x01, 0xFE,
  0xFE, 0xFE, 0x01, 0xFE, 0xFE, 0xFE, 0xFE, 0x01,
  0xFE, 0x01, 0x01, 0xFE, 0x01, 0x01, 0x01, 0xFE,
  0x01, 0x01, 0x01, 0x01, 0xFE, 0xDF, 0x20, 0xDF,
  0x00, 0xFF, 0x00, 0xDF, 0x00, 0xFF, 0x00, 0xDF,
  0x00, 0xDF, 0x20, 0xDF, 0x20, 0xFF, 0x00, 0xFF,
  0x20, 0xFF, 0x00, 0xFF, 0x20, 0xFF, 0x20, 0xDF,
  0xEF, 0x10, 0xEF, 0xEF, 0x10, 0xEF, 0xEF, 0xEF,
  0x10, 0xEF, 0xEF, 0xEF, 0xEF, 0x10, 0xEF, 0x10,
  0x10, 0xEF, 0x10, 0x10, 0x10, 0xEF, 0x10, 0x10,
  0x10, 0x10, 0xEF, 0xF7, 0x00, 0xFF, 0x04, 0x7F,
  0x00, 0xFF, 0x40, 0xFD, 0x00, 0xFF, 0x01, 0xDF
};

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function returns the (index)th UINT8
 *   from an indicated test pattern.
 *
 *     @param[in] Pattern - encoding of test pattern type
 *     @param[in] Buffer[] - buffer to be filled
 *     @param[in] Size - Size of the buffer
 *
 * ----------------------------------------------------------------------------
 */

VOID
MemUFillTrainPattern (
  IN       TRAIN_PATTERN Pattern,
  IN       UINT8 Buffer[],
  IN       UINT16 Size
  )
{
  UINT8 Result;
  UINT8 i;
  UINT8 Mask;
  UINT16 Index;
  UINT16 k;

  for (Index = 0; Index < Size; Index++) {
    k = Index;
    // get one byte from Pattern
    switch (Pattern) {
    case TestPattern0:
      Result = 0xAA;
      break;
    case TestPattern1:
      Result = 0x55;
      break;
    case TestPattern2:
      ASSERT (Index < sizeof (Pattern2));
      Result = ((UINT8 *)Pattern2)[Index];
      break;
    case TestPatternML:
      if (Size != 6 * 64) {
        Result = ((UINT8 *)MaxLatPat)[Index];
      } else {
        Result = ((UINT8 *)MaxLatPat)[Index & 0xF7];
      }
      break;
    case TestPatternJD256B:
      k >>= 1;
      // break is not being used here because TestPatternJD256B also need
      // to run TestPatternJD256A sequence.
    case TestPatternJD256A:
      k >>= 3;
      ASSERT (k < sizeof (PatternJD_256));
      Result = PatternJD_256[k];
      break;
    case TestPatternJD1B:
      k >>= 1;
      // break is not being used here because TestPatternJD1B also need
      // to run TestPatternJD1A sequence.
    case TestPatternJD1A:
      k >>= 3;
      i = (UINT8) (k >> 3);
      Mask = (UINT8) (0x80 >> (k & 7));

      if (i == 0) {
        Result = 0;
      } else {
        Result = (UINT16)1 << (i - 1);
      }

      ASSERT (i < sizeof (PatternJD));
      if (PatternJD[i] & Mask) {
        Result = ~Result;
      }
      break;
    case TestPattern3:
      Result = 0x36;
      break;
    case TestPattern4:
      Result = 0xC9;
      break;
    default:
      Result = 0;
      IDS_ERROR_TRAP;
    }

    // fill in the Pattern buffer
    Buffer[Index] = Result;
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function flushes cache lines
 *
 *     @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *     @param[in] ClCount - Number of cache lines
 *     @param[in] Address - System Address [47:16]
 *
 * ----------------------------------------------------------------------------
 */

VOID
MemUProcIOClFlush (
  IN       UINT32 Address,
  IN       UINT16 ClCount,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  MemUSetTargetWTIO (Address, MemPtr);
  MemUFlushPattern (MemUSetUpperFSbase (Address, MemPtr), ClCount);
  MemUResetTargetWTIO (MemPtr);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the upper 32-bits of the Base address, 4GB aligned) for the FS selector.
 *      @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *      @param[in] Address - System Address [47:16]
 *
 *     @return  Address - Lowest 32-bit of physical address
 * ----------------------------------------------------------------------------
 */

UINT32
MemUSetUpperFSbase (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  S_UINT64 SMsr;

  SMsr.lo = 0;
  SMsr.hi = Address >> 16;
  LibAmdMsrWrite (FS_BASE, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  return Address << 16;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function resets the target address space to Write Through IO by disabling IORRs
 *
 *      @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *
 * ----------------------------------------------------------------------------
 */

VOID
MemUResetTargetWTIO (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  S_UINT64 SMsr;
  SMsr.hi = 0;
  SMsr.lo = 0;
  LibAmdMsrWrite (IORR0_MASK, (UINT64 *)&SMsr, &MemPtr->StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function sets the target range to WT IO (using an IORR overlapping
 *       the already existing
 *
 *      @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *      @param[in] Address - System Address [47:16]
 *
 * ----------------------------------------------------------------------------
 */

VOID
MemUSetTargetWTIO (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  S_UINT64 SMsr;

  SMsr.lo = Address << 16;
  SMsr.hi = Address >> 16;
  LibAmdMsrWrite (IORR0_BASE,(UINT64 *)&SMsr, &MemPtr->StdHeader);           // IORR0 Base
  SMsr.hi = 0xFFFF;
  SMsr.lo = 0xFC000800;
  LibAmdMsrWrite (IORR0_MASK, (UINT64 *)&SMsr, &MemPtr->StdHeader);          // 64MB Mask
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Waits specified number of 10ns cycles
 *      @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *      @param[in] Count - Number of 10ns cycles to wait; Note that Count must not exceed 1000000
 *
 * ----------------------------------------------------------------------------
 */

VOID
MemUWait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT64 TargetTsc;
  UINT64 CurrentTsc;

  ASSERT (Count <= 1000000);

  MemUMFenceInstr ();

  LibAmdMsrRead (TSC, &CurrentTsc, &MemPtr->StdHeader);
  TargetTsc = CurrentTsc + ((Count * MemPtr->TscRate + 99) / 100);
  IEM_SKIP_CODE (IEM_WAIT) {
    do {
      LibAmdMsrRead (TSC, &CurrentTsc, &MemPtr->StdHeader);
    } while (CurrentTsc < TargetTsc);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      Find the entry of platform specific overriding table.
 *
 *      @param[in] PlatformMemoryConfiguration - Platform config table
 *      @param[in] EntryType - Entry type
 *      @param[in] SocketID - Physical socket ID
 *      @param[in] ChannelID - Physical channel ID
 *      @param[in] DimmID - Physical Dimm ID
 *      @param[in] *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *      @param[in,out]  *StdHeader - Pointer of AMD_CONFIG_PARAMS
 *                 - If both *LogicalIdPtr and *StdHeader are "NULL" input,
 *                   that means, the "EntryType" are not CPU family dependent,
 *                   ex. PSO_MAX_DIMMS for NUMBER_OF_DIMMS_SUPPORTED macro.
 *
 *
 *      @return  NULL - entry could not be found.
 *      @return  Pointer - points to the entry's data.
 *
 * ----------------------------------------------------------------------------
 */

VOID *
FindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       PSO_ENTRY EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       UINT8 DimmID,
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 *Buffer;
  CPU_LOGICAL_ID LogicalCpuId;
  UINT32 RawCpuId;

  LogicalCpuId.Family = AMD_FAMILY_UNKNOWN;
  LogicalCpuId.Revision = 0;
  RawCpuId = 0;

  Buffer = PlatformMemoryConfiguration;
  //
  // Do not need to look for CPU family specific PSO if LogicalIdPtr and StdHeader are NULL.
  //
  if ((LogicalIdPtr != NULL) && (StdHeader != NULL)) {
    //
    // Looking for the CPU family signature followed by CPUID value.
    // And check to see if the CPUID value is matched with current CPU's :
    //    - If matched, Buffer points to following PSO macros' start address.
    //    - If not matched, Buffer points to PlatformMemoryConfiguration for global PSO parsing.
    //
    while (Buffer[0] != PSO_END) {
      if (Buffer[0] == PSO_CPU_FAMILY_TO_OVERRIDE) {
        RawCpuId = *(UINT32 *)&Buffer[2];
        GetLogicalIdFromCpuid (RawCpuId, &LogicalCpuId, StdHeader);
        if ((LogicalCpuId.Family & LogicalIdPtr->Family) != 0) {
          if ((LogicalCpuId.Revision & LogicalIdPtr->Revision) != 0) {
            Buffer += Buffer[1] + 2;
            break;
          }
        }
      }
      Buffer += Buffer[1] + 2;
    }
    //
    // If no CPU family specific PSO macros exist, Buffer points to PlatformMemoryConfiguration again
    //
    if (Buffer[0] == PSO_END) {
      Buffer = PlatformMemoryConfiguration;
    }
  }

  while ((Buffer[0] != PSO_END) && (Buffer[0] != PSO_CPU_FAMILY_TO_OVERRIDE)) {
    if (Buffer[0] == EntryType) {
      if ((Buffer[2] & ((UINT8) 1 << SocketID)) != 0 ) {
        if ((Buffer[3] & ((UINT8) 1 << ChannelID)) != 0 ) {
          if ((Buffer[4] & ((UINT8) 1 << DimmID)) != 0 ) {
            return &Buffer[5];
          }
        }
      }
    }
    Buffer += Buffer[1] + 2;
  }

  return NULL;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the max dimms for a given memory channel on a given
 *  processor.  It first searches the platform override table for the max dimms
 *  value.  If it is not provided, the AGESA default value is returned. The target
 *  socket must be a valid present socket.
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID  - ID of the processor that owns the channel
 *     @param[in] ChannelID - Channel to get max dimms for
 *
 *
 *     @return UINT8 - Max Number of Dimms for that channel
 */
UINT8
GetMaxDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  )
{
  UINT8  *DimmsPerChPtr;
  UINT8  MaxDimmPerCH;

  DimmsPerChPtr = FindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_DIMMS, SocketID, ChannelID, 0, NULL, NULL);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = MAX_DIMMS_PER_CHANNEL;
  }
  // Maximum number of dimms per channel cannot be larger than its default value.
  ASSERT (MaxDimmPerCH <= MAX_DIMMS_PER_CHANNEL);

  return MaxDimmPerCH;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the max soldered-down dimms for a given memory channel on a given
 *  processor.  It first searches the platform override table for the soldered-down dimms
 *  value.  If it is not provided, the AGESA default value is returned. The target
 *  socket must be a valid present socket.
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID  - ID of the processor that owns the channel
 *     @param[in] ChannelID - Channel to get max dimms for
 *
 *
 *     @return UINT8 - Max Number of soldered-down dimms for that channel
 */
UINT8
GetMaxSolderedDownDimmsPerChannel (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID
  )
{
  UINT8  *DimmsPerChPtr;
  UINT8  MaxSolderedDownDimmPerCH;

  DimmsPerChPtr = FindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_SOLDERED_DOWN_DIMMS, SocketID, ChannelID, 0, NULL, NULL);
  if (DimmsPerChPtr != NULL) {
    MaxSolderedDownDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxSolderedDownDimmPerCH = 0;
  }
  // Maximum number of dimms per channel cannot be larger than its default value.
  ASSERT (MaxSolderedDownDimmPerCH <= MAX_DIMMS_PER_CHANNEL);

  return MaxSolderedDownDimmPerCH;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the max memory channels on a given processor.
 *  It first searches the platform override table for the max channels value.
 *  If it is not provided, the AGESA default value is returned.
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID - ID of the processor
 *     @param[in] StdHeader - Header for library and services
 *
 *
 *     @return UINT8 - Max Number of Channels on that Processor
 */
UINT8
GetMaxChannelsPerSocket (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  *ChannelsPerSocketPtr;
  UINT8  MaxChannelsPerSocket;

  if (IsProcessorPresent (SocketID, StdHeader)) {
    ChannelsPerSocketPtr = FindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_CHNLS, SocketID, 0, 0, NULL, NULL);
    if (ChannelsPerSocketPtr != NULL) {
      MaxChannelsPerSocket = *ChannelsPerSocketPtr;
    } else {
      MaxChannelsPerSocket = MAX_CHANNELS_PER_SOCKET;
    }
    // Maximum number of channels per socket cannot be larger than its default value.
    ASSERT (MaxChannelsPerSocket <= MAX_CHANNELS_PER_SOCKET);
  } else {
    MaxChannelsPerSocket = 0;
  }

  return MaxChannelsPerSocket;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the max number of chip select on a given channel of
 *  a given processor. It first searches the platform override table for the max
 *  chip select value. If it is not provided, the AGESA default value is returned.
 *  The target socket must be a valid present socket.
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID - ID of the processor
 *     @param[in] ChannelID - ID of a channel
 *
 *
 *     @return UINT8 - Max Number of chip selects on the channel of the Processor
 */
UINT8
GetMaxCSPerChannel (
  IN      PSO_TABLE *PlatformMemoryConfiguration,
  IN      UINT8 SocketID,
  IN      UINT8 ChannelID
  )
{
  UINT8  *CSPerSocketPtr;
  UINT8  MaxCSPerChannel;

  CSPerSocketPtr = FindPSOverrideEntry (PlatformMemoryConfiguration, PSO_MAX_CHIPSELS, SocketID, ChannelID, 0, NULL, NULL);
  if (CSPerSocketPtr != NULL) {
    MaxCSPerChannel = *CSPerSocketPtr;
  } else {
    MaxCSPerChannel = MAX_CS_PER_CHANNEL;
  }
  // Max chip select per channel cannot be larger than its default value
  ASSERT (MaxCSPerChannel <= MAX_CS_PER_CHANNEL);

  return MaxCSPerChannel;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the index of the first Dimm SPD structure for a
 *  given processor socket. It checks the Max Dimms per channel for every memory
 *  channel on every processor up to the current one, and adds them together.
 *
 *      This function may also be used to calculate the maximum dimms per system
 *  by passing the total number of dimm sockets
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID - ID of the processor
 *     @param[in] StdHeader - Header for library and services
 *
 *     @return UINT8 - SPD Index
 */
UINT8
GetSpdSocketIndex (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 SpdSocketIndex;
  UINT8 Socket;
  UINT8 Channel;
  UINT8  MaxChannelsPerSocket;

  SpdSocketIndex = 0;
  for (Socket = 0; Socket < SocketID; Socket++) {
    MaxChannelsPerSocket = GetMaxChannelsPerSocket (PlatformMemoryConfiguration, Socket, StdHeader);
    for (Channel = 0; Channel < MaxChannelsPerSocket; Channel++) {
      SpdSocketIndex = SpdSocketIndex + GetMaxDimmsPerChannel (PlatformMemoryConfiguration, Socket, Channel);
    }
  }
  return SpdSocketIndex;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function returns the index of the first Dimm SPD structure for a
 *  given channel relative to the processor socket. It checks the Max Dimms per
 *  channel for every memory channel on that processor up to the current one,
 *  and adds them together.
 *
 *      This function may also be used to calculate the maximum dimms per system
 *  by passing the total number of DIMM sockets
 *
 *     @param[in] PlatformMemoryConfiguration - Platform config table
 *     @param[in] SocketID - ID of the processor
 *     @param[in] ChannelID - ID of the Channel
 *     @param[in] StdHeader - Header for library and services
 *
 *     @return UINT8 - SPD Index
 */
UINT8
GetSpdChannelIndex (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 SpdChannelIndex;
  UINT8 Channel;

  SpdChannelIndex = 0;
  ASSERT (ChannelID < GetMaxChannelsPerSocket (PlatformMemoryConfiguration, SocketID, StdHeader))
  for (Channel = 0; Channel < ChannelID; Channel++) {
    SpdChannelIndex = SpdChannelIndex + GetMaxDimmsPerChannel (PlatformMemoryConfiguration, SocketID, Channel);
  }
  return SpdChannelIndex;
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *     This function returns the upper 32 bits mask for variable MTRR based on
 *     the CPU_LOGICAL_ID.
 *     @param[in]       *LogicalIdPtr - Pointer to the CPU_LOGICAL_ID
 *     @param[in]       StdHeader - Header for library and services
 *
 *     @return          UINT32 - MTRR mask for upper 32 bits
 *
 */
UINT32
GetVarMtrrHiMsk (
  IN       CPU_LOGICAL_ID *LogicalIdPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 TempNotCare;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  CONST CACHE_INFO *CacheInfoPtr;

  GetCpuServicesFromLogicalId (LogicalIdPtr, (CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetCacheInfo (FamilySpecificServices, (CONST VOID **) &CacheInfoPtr, &TempNotCare, StdHeader);
  return (UINT32) (CacheInfoPtr->VariableMtrrMask >> 32);
}


/*-----------------------------------------------------------------------------*/
/**
 *
 *     This function returns number of memclk converted from ns
 *     @param[in]       Speed - memclk frequency
 *     @param[in]       NumberOfns - number of ns to be converted
 *
 *     @return          UINT32 - number of memclk
 *
 */
UINT32
MemUnsToMemClk (
  IN       UINT32 Speed,
  IN       UINT32 NumberOfns
  )
{
  return (UINT32) ((NumberOfns * Speed + 999) / 1000);
}
