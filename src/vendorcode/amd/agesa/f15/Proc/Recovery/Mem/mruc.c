/* $NoKeywords:$ */
/**
 * @file
 *
 * mruc.c
 *
 * Utility functions recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
 * @e \$Revision: 52577 $ @e \$Date: 2011-05-09 12:58:33 -0600 (Mon, 09 May 2011) $
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */


#include "AGESA.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "amdlib.h"
#include "Ids.h"
#include "mrport.h"
#include "mru.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_MRUC_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

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

VOID
STATIC
MemRecUResetTargetWTIO (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

VOID
STATIC
MemRecUSetTargetWTIO (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function returns the (Index)th UINT8
 *   from an indicated test Pattern.
 *
 *     @param[in] Pattern - encoding of test Pattern type
 *     @param[in] Buffer[] - buffer to be filled
 *     @param[in] Size - Size of the bugger
 *     @param[in] *StdHeader - pointer to AMD_CONFIG_PARAMS
 *
 */

VOID
MemRecUFillTrainPattern (
  IN       TRAIN_PATTERN Pattern,
  IN       UINT8 Buffer[],
  IN       UINT16 Size,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  STATIC UINT8 PatternData[2] = {0x55, 0xAA};

  LibAmdMemFill (Buffer, PatternData[Pattern == TestPattern0 ? TestPattern1 : TestPattern0], Size, StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function flushes cache lines
 *
 *     @param[in] Address - System Address [39:8]
 *     @param[in,out] *MemPtr - Pointer to MEM_DATA_STRUCT
 *
 */

VOID
MemRecUProcIOClFlush (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  MemRecUSetTargetWTIO (Address, MemPtr);
  MemRecUFlushPattern (Address);
  MemRecUResetTargetWTIO (MemPtr);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function resets the target address space to Write Through IO by disabling IORRs
 */

VOID
STATIC
MemRecUResetTargetWTIO (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  S_UINT64 Smsr;
  Smsr.hi = 0;
  Smsr.lo = 0;
  LibAmdMsrWrite (IORR0_MASK, (UINT64 *)&Smsr, &MemPtr->StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *       This function sets the target range to WT IO (using an IORR overlapping
 *       the already existing
 *      @param[in,out] *MemPtr - pointer to MEM_DATA_STRUCTURE
 *      @param[in] Address - System Address
 *
 */

VOID
STATIC
MemRecUSetTargetWTIO (
  IN       UINT32 Address,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  S_UINT64 SMsr;

  SMsr.lo = Address;
  SMsr.hi = 0;
  LibAmdMsrWrite (IORR0_BASE,(UINT64 *)&SMsr, &MemPtr->StdHeader);          // ;IORR0 Base
  SMsr.hi = 0xFFFF;
  SMsr.lo = 0xFC000800;
  LibAmdMsrWrite (IORR0_MASK, (UINT64 *)&SMsr, &MemPtr->StdHeader);          // ;64MB Mask
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  Waits specified number of 10ns cycles
 *      @param[in,out] MemPtr - pointer to MEM_DATA_STRUCTURE
 *      @param[in] Count - Number of 10ns cycles to wait
 *
 */

VOID
MemRecUWait10ns (
  IN       UINT32 Count,
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  S_UINT64 SMsr;

  LibAmdMsrRead (TSC, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  Count += SMsr.lo;
  while (SMsr.lo < Count) {
    LibAmdMsrRead (TSC, (UINT64 *)&SMsr, &MemPtr->StdHeader);
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
 *
 *      @return  NULL - entry could not be found.
 *      @return  Pointer - points to the entry's data.
 *
 * ----------------------------------------------------------------------------
 */
VOID *
MemRecFindPSOverrideEntry (
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       PSO_ENTRY EntryType,
  IN       UINT8 SocketID,
  IN       UINT8 ChannelID,
  IN       UINT8 DimmID
  )
{
  UINT8 *Buffer;

  Buffer = PlatformMemoryConfiguration;
  while (Buffer[0] != PSO_END) {
    if (Buffer[0] == EntryType) {
      if ((Buffer[2] & ((UINT8) 1 << SocketID)) != 0 ) {
        if ((Buffer[3] & ((UINT8) 1 << ChannelID)) != 0 ) {
          ASSERT ((Buffer[0] == PSO_MAX_DIMMS) ? (Buffer[5] <= MAX_DIMMS_PER_CHANNEL) : TRUE);
          ASSERT ((Buffer[0] == PSO_MAX_CHIPSELS) ? (Buffer[5] <= MAX_CS_PER_CHANNEL) : TRUE);
          ASSERT ((Buffer[0] == PSO_MAX_CHNLS) ? (Buffer[5] <= MAX_CHANNELS_PER_SOCKET) : TRUE);
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

