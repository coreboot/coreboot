/* $NoKeywords:$ */
/**
 * @file
 *
 * mnotda.c
 *
 * Northbridge Non-SPD timings for DA
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DA)
 * @e \$Revision: 36462 $ @e \$Date: 2010-08-20 00:49:49 +0800 (Fri, 20 Aug 2010) $
 *
 **/
/*
 *****************************************************************************
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnda.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_DA_MNOTDA_FILECODE


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
MemNSetOtherTimingDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemNPowerDownCtlDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the non-SPD timings
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNOtherTimingDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSwitchDCTNb (NBPtr, 0);
  if (NBPtr->DCTPtr->Timings.DctDimmValid > 0) {
    MemNSetOtherTimingDA (NBPtr);        // Set DA Timings
    MemNPowerDownCtlNb (NBPtr);
    MemNEnDLLShutDownDA (NBPtr);
  }

  MemNSwitchDCTNb (NBPtr, 1);
  if ((NBPtr->DCTPtr->Timings.DctDimmValid > 0) && (NBPtr->MCTPtr->GangedMode == FALSE)) {
    MemNSetOtherTimingDA (NBPtr);                // Set DA Timings
    MemNPowerDownCtlNb (NBPtr);
    MemNEnDLLShutDownDA (NBPtr);
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets the non-SPD timings into the PCI registers
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNSetOtherTimingDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSetBitFieldNb (NBPtr, BFTrdrd, MemNGetTrdrdNb (NBPtr));
  MemNSetBitFieldNb (NBPtr, BFTwrwr, MemNGetTwrwrNb (NBPtr));
  MemNSetBitFieldNb (NBPtr, BFTwrrd, MemNGetTwrrdNb (NBPtr));
  MemNSetBitFieldNb (NBPtr, BFTrwtTO, MemNGetTrwtTONb (NBPtr));
  MemNSetBitFieldNb (NBPtr, BFTrwtWB, MemNGetTrwtWBNb (NBPtr));
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function enables power down mode
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNPowerDownCtlDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_PARAMETER_STRUCT *RefPtr;
  UINT8 PowerDownMode;

  RefPtr = NBPtr->RefPtr;

  // we can't enable powerdown mode when doing WL
  if (RefPtr->EnablePowerDown) {
    MemNSetBitFieldNb (NBPtr, BFPowerDownEn, 1);
    PowerDownMode = (UINT8) UserOptions.CfgPowerDownMode;
    IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
    if (PowerDownMode) {
      MemNSetBitFieldNb (NBPtr, BFPowerDownMode, 1);
    }
  }
}



