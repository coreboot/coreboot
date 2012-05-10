/* $NoKeywords:$ */
/**
 * @file
 *
 * mfchi.c
 *
 * Feature Channel interleaving support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/Chintlv)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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


/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "mm.h"
#include "mn.h"
#include "mfchi.h"
#include "Ids.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_CHINTLV_MFCHI_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _4GB_ (0x10000 >> 10)

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
 *   MemFInterleaveChannels:
 *
 *  Applies DIMM channel interleaving if enabled, if not ganged mode, and
 *  there are valid dimms in both channels.  Called once per Node.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFInterleaveChannels (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT32 DramBase;
  UINT32 DctSelBase;
  UINT32 HoleSize;
  UINT32 HoleBase;
  UINT32 HoleOffset;
  UINT32 Dct0Size;
  UINT32 Dct1Size;
  UINT32 SmallerDct;
  UINT8 DctSelIntLvAddr;
  UINT8 DctSelHi;
  UINT8 DctSelHiRngEn;
  UINT32 HoleValid;

  MEM_PARAMETER_STRUCT *RefPtr;
  DIE_STRUCT *MCTPtr;

  ASSERT (NBPtr != NULL);

  RefPtr = NBPtr->RefPtr;

  DctSelIntLvAddr = NBPtr->DefDctSelIntLvAddr;
  if (RefPtr->EnableChannelIntlv) {
    HoleSize = 0;
    HoleBase = 0;
    if (RefPtr->GStatus[GsbSoftHole] || RefPtr->GStatus[GsbHWHole]) {
      // HoleBase scaled from [47:16] to [47:26]
      HoleBase = RefPtr->HoleBase >> 10;
      HoleSize = _4GB_ - HoleBase;
    }

    MCTPtr = NBPtr->MCTPtr;

    HoleValid = NBPtr->GetBitField (NBPtr, BFDramHoleValid);
    if ((!MCTPtr->GangedMode) &&
        (MCTPtr->DctData[0].Timings.DctMemSize != 0) &&
        (MCTPtr->DctData[1].Timings.DctMemSize != 0)) {
      // DramBase scaled [47:16] to [47:26]
      DramBase = MCTPtr->NodeSysBase >> 10;
      // Scale NodeSysLimit [47:16] to [47:26]
      Dct1Size = (MCTPtr->NodeSysLimit + 1) >> 10;
      Dct0Size = NBPtr->GetBitField (NBPtr, BFDctSelBaseOffset);
      if ((Dct0Size >= _4GB_) && (DramBase < HoleBase)) {
        Dct0Size -= HoleSize;
      }
      if ((Dct1Size >= _4GB_) && (DramBase < HoleBase)) {
        Dct1Size -= HoleSize;
      }
      Dct1Size -= Dct0Size;
      Dct0Size -= DramBase;

       // Select the bigger size DCT to put in DctSelHi
      DctSelHiRngEn = 1;
      DctSelHi = 0;
      SmallerDct = Dct1Size;
      if (Dct1Size == Dct0Size) {
        SmallerDct = 0;
        DctSelHiRngEn = 0;
      } else if (Dct1Size > Dct0Size) {
        SmallerDct = Dct0Size;
        DctSelHi = 1;
      }

      if (SmallerDct != 0) {
        DctSelBase = (SmallerDct * 2) + DramBase;
      } else {
        DctSelBase = 0;
      }
      if ((DctSelBase >= HoleBase) && (DramBase < HoleBase)) {
        DctSelBase += HoleSize;
      }
      IDS_OPTION_HOOK (IDS_CHANNEL_INTERLEAVE, &DctSelIntLvAddr, &(NBPtr->MemPtr->StdHeader));
      NBPtr->SetBitField (NBPtr, BFDctSelBaseAddr, DctSelBase >> 1);
      NBPtr->SetBitField (NBPtr, BFDctSelHiRngEn, DctSelHiRngEn);
      NBPtr->SetBitField (NBPtr, BFDctSelHi, DctSelHi);
      NBPtr->SetBitField (NBPtr, BFDctSelIntLvAddr, DctSelIntLvAddr);
      NBPtr->SetBitField (NBPtr, BFDctSelIntLvEn, 1);

       // DctSelBaseOffset = DctSelBaseAddr - Interleaved region
      NBPtr->SetBitField (NBPtr, BFDctSelBaseOffset, DctSelBase - SmallerDct);

       // Adjust DramHoleOffset
      if (HoleValid != 0) {
        HoleOffset = DramBase;
        if ((DctSelBase < HoleBase) && (DctSelBase != 0)) {
          HoleOffset += (DctSelBase - DramBase) >> 1;
        }
        HoleOffset += HoleSize;
        NBPtr->SetBitField (NBPtr, BFDramHoleOffset, HoleOffset << 3);
      }
    } else {
      //
      // Channel Interleaving is requested but cannot be enabled
      //
      PutEventLog (AGESA_WARNING, MEM_WARNING_CHANNEL_INTERLEAVING_NOT_ENABLED, NBPtr->Node, 0, 0, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, MCTPtr);
    }

    return TRUE;
  } else {
    return FALSE;
  }
}
