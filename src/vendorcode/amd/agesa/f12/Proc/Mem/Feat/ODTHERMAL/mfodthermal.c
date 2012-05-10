/* $NoKeywords:$ */
/**
 * @file
 *
 * mfodthermal.c
 *
 * On Dimm thermal management.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat)
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
#include "mt.h"
#include "Ids.h"
#include "mfodthermal.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_ODTHERMAL_MFODTHERMAL_FILECODE

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
 *   This function does On-Dimm thermal management.
 *
 *   @param[in, out]   *NBPtr - Pointer to the MEM_NB_BLOCK.
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */

BOOLEAN
MemFOnDimmThermal (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  UINT8 Dct;
  CH_DEF_STRUCT *ChannelPtr;
  MEM_DATA_STRUCT *MemPtr;
  UINT8 *SpdBufferPtr;
  UINT8 ThermalOp;
  BOOLEAN ODTSEn;
  BOOLEAN ExtendTmp;

  ODTSEn = FALSE;
  ExtendTmp = FALSE;

  ASSERT (NBPtr != NULL);
  MemPtr = NBPtr->MemPtr;
  AGESA_TESTPOINT (TpProcMemOnDimmThermal, &MemPtr->StdHeader);
  if (NBPtr->MCTPtr->NodeMemSize != 0) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      // Only go through the DCT if it is not disabled.
      if (NBPtr->GetBitField (NBPtr, BFDisDramInterface) == 0) {
        ChannelPtr = NBPtr->ChannelPtr;
        // If Ganged mode is enabled, need to go through all dram devices on both DCTs.
        if (!NBPtr->Ganged || (NBPtr->Dct != 1)) {
          if (!(NBPtr->IsSupported[CheckSetSameDctODTsEn]) || (NBPtr->IsSupported[CheckSetSameDctODTsEn] && (NBPtr->Dct != 1))) {
            ODTSEn = TRUE;
            ExtendTmp = TRUE;
          }
        }
        for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i ++) {
          if (NBPtr->TechPtr->GetDimmSpdBuffer (NBPtr->TechPtr, &SpdBufferPtr, i)) {
            // Check byte 31: thermal and refresh option.
            ThermalOp = SpdBufferPtr[THERMAL_OPT];
            // Bit 3: ODTS readout
            if (!((ThermalOp >> 3) & 1)) {
              ODTSEn = FALSE;
            }
            // Bit 0: Extended Temperature Range.
            if (!(ThermalOp & 1)) {
              ExtendTmp = FALSE;
            }
          }
        }

        if (!NBPtr->Ganged || (NBPtr->Dct == 1)) {
          // If in ganged mode, need to switch back to DCT0 to set the registers.
          if (NBPtr->Ganged || NBPtr->IsSupported[CheckSetSameDctODTsEn]) {
            NBPtr->SwitchDCT (NBPtr, 0);
            ChannelPtr = NBPtr->ChannelPtr;
          }
          // If all dram devices on a DCT support ODTS
          if (ODTSEn) {
            NBPtr->SetBitField (NBPtr, BFODTSEn, 1);
          }
          ChannelPtr->ExtendTmp = ExtendTmp;
        }
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\tDct %d\n", Dct);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\tODTSEn = %d\n", ODTSEn);
      IDS_HDT_CONSOLE (MEM_FLOW, "\t\tExtendTmp = %d\n", ExtendTmp);
    }
  }
  return TRUE;
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
