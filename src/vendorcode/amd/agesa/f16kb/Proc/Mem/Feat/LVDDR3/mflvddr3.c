/* $NoKeywords:$ */
/**
 * @file
 *
 * lvddr3.c
 *
 * Voltage change for DDR3 DIMMs.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/LVDDR3)
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
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_LVDDR3_MFLVDDR3_FILECODE
/* features */
#include "mflvddr3.h"


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

/*-----------------------------------------------------------------------------*/
/**
 *
 *   This function calculate the common lowest voltage supported by all DDR3
 *   DIMMs in the system. This function only needs to be called on BSP.
 *
 *   @param[in, out]    *NBPtr - Pointer to NB block
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFLvDdr3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CH_DEF_STRUCT *ChannelPtr;
  MEM_TECH_BLOCK  *TechPtr;
  MEM_SHARED_DATA *mmSharedPtr;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 *SpdBufferPtr;
  UINT8 VDDByte;
  UINT8 VoltageMap;

  mmSharedPtr = NBPtr->SharedPtr;
  TechPtr = NBPtr->TechPtr;
  VoltageMap = 0xFF;

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    for (Channel = 0; Channel < NBPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      ChannelPtr = NBPtr->ChannelPtr;
      for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
        if (TechPtr->GetDimmSpdBuffer (TechPtr, &SpdBufferPtr, Dimm)) {
          // SPD byte 6: Module Nominal Voltage, VDD
          // 1.5v - bit 0
          // 1.35v - bit 1
          // 1.2v - bit 2
          VDDByte = SpdBufferPtr[MNVVDD];
          IDS_HDT_CONSOLE (MEM_FLOW, "Node%d DCT%d Channel%d Dimm%d VDD Byte: 0x%02x\n", NBPtr->Node, Dct, Channel, Dimm, VDDByte);

          // Reverse the 1.5V operable bit. So its encoding can be consistent
          // with that of 1.35V and 1.25V operable bit.
          VDDByte ^= 1;
          ASSERT (VDDByte != 0);

          if (mmSharedPtr->VoltageMap != 0) {
            // Get the common supported voltage map
            VoltageMap &= VDDByte;
          } else {
            // This is the second execution of all the loop as no common voltage is found
            if (VDDByte == (1 << VOLT1_5_ENCODED_VAL)) {
              // Always exclude 1.5V dimm if no common voltage is found
              ChannelPtr->DimmExclude |= (UINT16) 1 << Dimm;
            }
          }
        }
      }
      if (mmSharedPtr->VoltageMap == 0) {
        NBPtr->DCTPtr->Timings.DimmExclude |= ChannelPtr->DimmExclude;
      }
    }
  }

  if (mmSharedPtr->VoltageMap != 0) {
    mmSharedPtr->VoltageMap &= VoltageMap;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
