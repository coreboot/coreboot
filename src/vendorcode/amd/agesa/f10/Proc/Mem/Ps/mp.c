/**
 * @file
 *
 * mndr.c
 *
 * Common Northbridge functions for DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DR)
 * @e \$Revision: 8446 $ @e \$Date: 2008-09-23 10:52:09 -0500 (Tue, 23 Sep 2008) $
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mp.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_PS_MP_FILECODE


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

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This is the default return function of the Platform Specific block. The function always
 *    returns   AGESA_UNSUPPORTED
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in]       *PsPtr            Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_UNSUPPORTED  AGESA status indicating that default is unsupported
 *
 */

AGESA_STATUS
MemPConstructPsUDef (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  )
{
  return AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function will set the DramTerm and DramTermDyn in the structure of a channel.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *     @param[in]       ArraySize        Size of the array of DramTerm
 *     @param[in]       *DramTermPtr     Address the array of DramTerm
 *
 *     @return          TRUE - Find DramTerm and DramTermDyn for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find DramTerm and DramTermDyn for corresponding platform and dimm population.
 *
 */
BOOLEAN
MemPGetDramTerm (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ArraySize,
  IN       CONST DRAM_TERM_ENTRY *DramTermPtr
  )
{
  UINT8 Dimms;
  UINT8 QR_Dimms;
  UINT8 i;
  Dimms = NBPtr->ChannelPtr->Dimms;
  QR_Dimms = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i ++) {
    if (((NBPtr->ChannelPtr->DimmQrPresent & (UINT16) (1 << i)) != 0) && (i < 2)) {
      QR_Dimms ++;
    }
  }

  for (i = 0; i < ArraySize; i ++) {
    if ((DramTermPtr[i].Speed & ((UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66))) != 0) {
      if ((((UINT8) (1 << (Dimms - 1)) & DramTermPtr[i].Dimms) != 0) || (DramTermPtr[i].Dimms == ANY_NUM)) {
        if (((QR_Dimms == 0) && (DramTermPtr[i].QR_Dimms == NO_DIMM)) ||
            ((QR_Dimms > 0) && (((UINT8) (1 << (QR_Dimms - 1)) & DramTermPtr[i].QR_Dimms) != 0)) ||
            (DramTermPtr[i].QR_Dimms == ANY_NUM)) {
          NBPtr->PsPtr->DramTerm = DramTermPtr[i].DramTerm;
          NBPtr->PsPtr->QR_DramTerm = DramTermPtr[i].QR_DramTerm;
          NBPtr->PsPtr->DynamicDramTerm = DramTermPtr[i].DynamicDramTerm;
          break;
        }
      }
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function gets the highest POR supported speed.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *     @param[in]       FreqLimitSize    Size of the array of Frequency Limit
 *     @param[in]       *FreqLimitPtr     Address the array of Frequency Limit
 *
 *     @return          UINT8 - frequency limit
 *
 */
UINT16
MemPGetPorFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqLimitSize,
  IN       CONST POR_SPEED_LIMIT *FreqLimitPtr
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 DimmTpMatch;
  UINT16 SpeedLimit;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;

  SpeedLimit = 0;
  DIMMRankType = MemAGetPsRankType (NBPtr->ChannelPtr);
  for (i = 0; i < FreqLimitSize; i++, FreqLimitPtr++) {
    if (NBPtr->ChannelPtr->Dimms != FreqLimitPtr->Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType = DIMMRankType & FreqLimitPtr->DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j ++) {
      if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
        DimmTpMatch++;
      }
    }
    if (DimmTpMatch == FreqLimitPtr->Dimms) {
      if (NBPtr->RefPtr->DDR3Voltage == VOLT1_5) {
        SpeedLimit = FreqLimitPtr->SpeedLimit_1_5V;
        break;
      } else if (NBPtr->RefPtr->DDR3Voltage == VOLT1_25) {
        SpeedLimit = FreqLimitPtr->SpeedLimit_1_25V;
        break;
      } else {
        SpeedLimit = FreqLimitPtr->SpeedLimit_1_35V;
        break;
      }
    }
  }

  return SpeedLimit;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function is the default function for getting POR speed limit. When a
 *    package does not need to cap the speed, it should use this function to initialize
 *    the corresponding function pointer.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 */
VOID
MemPGetPORFreqLimitDef (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
}
