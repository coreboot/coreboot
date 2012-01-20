/* $NoKeywords:$ */
/**
 * @file
 *
 * mt2.c
 *
 * Common Technology  functions for DDR2
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech/DDR2)
 * @e \$Revision: 56279 $ @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
#include "Ids.h"
#include "AdvancedApi.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "mt2.h"
#include "mtspd2.h"
#include "mtot2.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

/* features */
#include "mftds.h"
#define FILECODE PROC_MEM_TECH_DDR2_MT2_FILECODE

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
 *   This function Constructs the technology block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

BOOLEAN
MemConstructTechBlock2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  TECHNOLOGY_TYPE *TechTypePtr;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 i;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  CH_DEF_STRUCT *ChannelPtr;
  UINT8 DimmSlots;

  TechTypePtr = (TECHNOLOGY_TYPE *) FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MEM_TECH, NBPtr->MCTPtr->SocketId, 0, 0, NULL, NULL);
  if (TechTypePtr != NULL) {
    // Ensure the platform override value is valid
    ASSERT ((*TechTypePtr == DDR3_TECHNOLOGY) || (*TechTypePtr == DDR2_TECHNOLOGY));
    if (*TechTypePtr != DDR2_TECHNOLOGY) {
      return FALSE;
    }
  }


  TechPtr->NBPtr = NBPtr;
  TechPtr->RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;

  TechPtr->NBPtr = NBPtr;
  TechPtr->RefPtr = NBPtr->RefPtr;

  TechPtr->SetDramMode = MemTSetDramMode2;
  TechPtr->DimmPresence = MemTDIMMPresence2;
  TechPtr->SpdCalcWidth = MemTSPDCalcWidth2;
  TechPtr->SpdGetTargetSpeed = MemTSPDGetTargetSpeed2;
  TechPtr->AutoCycTiming = MemTAutoCycTiming2;
  TechPtr->SpdSetBanks = MemTSPDSetBanks2;
  TechPtr->SetDqsEccTmgs = MemTSetDQSEccTmgs;
  TechPtr->GetCSIntLvAddr = MemTGetCSIntLvAddr2;
  TechPtr->AdjustTwrwr = MemTAdjustTwrwr2;
  TechPtr->AdjustTwrrd = MemTAdjustTwrrd2;
  TechPtr->GetDimmSpdBuffer = MemTGetDimmSpdBuffer2;
  TechPtr->GetLD = MemTGetLD2;
  TechPtr->MaxFilterDly = 0;

  //
  // Map the Logical Dimms on this channel to the SPD that should be used for that logical DIMM.
  // The pointers to the DIMM SPD information is as follows (2 Dimm/Ch and 3 Dimm/Ch examples).
  //
  //    DIMM Spd Buffer                          Current Channel DimmSpdPtr[MAX_DIMMS_PER_CHANNEL] array
  //    (Number of dimms varies by platform)     (Array size is determined in AGESA.H) Dimm operations loop
  //                                              on this array only)
  //    2 DIMMS PER CHANNEL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1 SR/DR DIMM <--------------DimmSpdPtr[1]
  //                                                         DimmSpdPtr[2]------->NULL
  //                                                         DimmSpdPtr[3]------->NULL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <---------+----DimmSpdPtr[1]
  //                                                    |    DimmSpdPtr[2]------->NULL
  //                                                    +----DimmSpdPtr[3]
  //
  //    Socket N  Channel N Dimm 0    QR DIMM <-----+--------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <-----|---+----DimmSpdPtr[1]
  //                                                +-- | ---DimmSpdPtr[2]
  //                                                    +----DimmSpdPtr[3]
  //
  //    3 DIMMS PER CHANNEL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1 SR/DR DIMM <--------------DimmSpdPtr[1]
  //                        Dimm 3 SR/DR DIMM <--------------DimmSpdPtr[2]
  //                                                         DimmSpdPtr[3]------->NULL
  //
  //    Socket N  Channel N Dimm 0 SR/DR DIMM <--------------DimmSpdPtr[0]
  //                        Dimm 1    QR DIMM <---------+----DimmSpdPtr[1]
  //                        Dimm 3 SR/DR DIMM <-------- | ---DimmSpdPtr[2]
  //                                                    +----DimmSpdPtr[3]
  //
  //

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    DCTPtr = NBPtr->DCTPtr;
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      ChannelPtr->TechType = DDR2_TECHNOLOGY;
      ChannelPtr->MCTPtr = MCTPtr;
      ChannelPtr->DCTPtr = DCTPtr;

      DimmSlots = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                         MCTPtr->SocketId,
                                         NBPtr->GetSocketRelativeChannel (NBPtr, Dct, Channel)
                                        );
      //
      // Initialize the SPD pointers for each Dimm
      //
      for (i = 0 ; i < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0])) ; i++) {
        ChannelPtr->DimmSpdPtr[i] = NULL;
      }
      for (i = 0 ; i < DimmSlots; i++) {
        ChannelPtr->DimmSpdPtr[i] = &(ChannelPtr->SpdPtr[i]);
        if ( (i + 2) < (sizeof (ChannelPtr->DimmSpdPtr) / sizeof (ChannelPtr->DimmSpdPtr[0]))) {
          if (ChannelPtr->DimmSpdPtr[i]->DimmPresent) {
            if ((((ChannelPtr->DimmSpdPtr[i]->Data[SPD_DM_BANKS] >> 3) & 0x07) + 1) > 2) {
              ChannelPtr->DimmSpdPtr[i + 2] = &(ChannelPtr->SpdPtr[i]);
            }
          }
        }
      }
    }
  }
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


