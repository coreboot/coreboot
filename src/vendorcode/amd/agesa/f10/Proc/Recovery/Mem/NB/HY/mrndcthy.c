/**
 * @file
 *
 * mrndctHy.c
 *
 * Northbridge DCT support for Hydra Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mru.h"
#include "mrnhy.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_HY_MRNDCTHY_FILECODE

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   Set Dram ODT for mission mode and write leveling mode.
 *
 *     @param[in,out]   *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in]       OdtMode    - Mission mode or write leveling mode
 *     @param[in]       ChipSelect - Chip select number
 *     @param[in]       TargetCS   - Chip select number that is being trained
 *
 */

VOID
MemRecNSetDramOdtHy (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       ODT_MODE OdtMode,
  IN       UINT8 ChipSelect,
  IN       UINT8 TargetCS
  )
{
  UINT8 Dimms;
  UINT8 *DimmsPerChPtr;
  UINT8 MaxDimmsPerChannel;
  UINT8 DramTerm;
  UINT8 DramTermDyn;
  UINT8 WrLvOdt;
  BOOLEAN IsDualRank;

  Dimms = NBPtr->ChannelPtr->Dimms;
  IsDualRank = ((NBPtr->ChannelPtr->DimmDrPresent & (UINT8) 1 << (ChipSelect >> 1)) != 0) ? TRUE : FALSE;

  // Dram nominal termination
  if (Dimms == 1) {
    DramTerm = 1;   // 60 Ohms
  } else {
    DramTerm = 3;   // 40 Ohms
  }

  // Dram dynamic termination
  if (Dimms < 2) {
    DramTermDyn = 0;    // Disabled
  } else {
    DramTermDyn = 2;    // 120 Ohms
  }

  if (OdtMode == WRITE_LEVELING_MODE) {
    if (ChipSelect == TargetCS) {
      DramTerm = DramTermDyn;

      // Program WrLvOdt, the target DIMM should be the closest DIMM.
      DimmsPerChPtr = MemRecFindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, 0, NBPtr->ChannelPtr->ChannelID);
      if (DimmsPerChPtr != NULL) {
        MaxDimmsPerChannel = *DimmsPerChPtr;
      } else {
        MaxDimmsPerChannel = 2;
      }

      if (MaxDimmsPerChannel == 3) {
        if (NBPtr->ChannelPtr->DimmQrPresent != 0) {
          WrLvOdt = 0xF;
        } else {
          WrLvOdt = 7;
        }
      } else {
        if (NBPtr->ChannelPtr->DimmQrPresent != 0) {
          WrLvOdt = 0xB;
        } else if (Dimms == 2) {
          WrLvOdt = 3;
        } else if (IsDualRank) {
          WrLvOdt = 8;
        } else {
          WrLvOdt = 2;
        }
      }
      MemRecNSetBitFieldNb (NBPtr, BFWrLvOdt, WrLvOdt);
    }
  }
  MemRecNSetBitFieldNb (NBPtr, BFDramTerm, DramTerm);
  MemRecNSetBitFieldNb (NBPtr, BFDramTermDyn, DramTermDyn);
}
