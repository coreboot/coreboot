/* $NoKeywords:$ */
/**
 * @file
 *
 * mnflowhy.c
 *
 * Hydra initializer for MCT and DCT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mnhy.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_HY_MNFLOWHY_FILECODE
/* features */

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
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledHy[MAX_FF_TYPES];

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes the platform specific block
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - AGESA_SUCCESS at least one dorm factor was found
 *     @return          FALSE - AGESA_UNSUPPORTED - Error indicating that no form factors were found
 */

BOOLEAN
MemNPlatformSpecificFormFactorInitHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 f;
  UINT8 ErrUnSuppFFCount;
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_HY; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    if (NBPtr->ChannelPtr->ChDimmValid != 0) {
      ErrUnSuppFFCount = 0;
      for (f = 0; f < MAX_FF_TYPES; f++) {
        ASSERT (memPlatSpecFFInstalledHy[f] != NULL);
        if (memPlatSpecFFInstalledHy[f] (NBPtr->MemPtr, NBPtr->ChannelPtr, NBPtr->PsPtr) == AGESA_UNSUPPORTED) {
          ErrUnSuppFFCount++; //Count the number of AGESA_UNSUPPORTED errors
        } else {
          break;
        }
      }
      if (ErrUnSuppFFCount == MAX_FF_TYPES) {
        return FALSE; // No FF types are supported
      }
    }
  }
  return TRUE;
}
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

