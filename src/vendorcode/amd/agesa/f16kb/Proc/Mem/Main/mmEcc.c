/* $NoKeywords:$ */
/**
 * @file
 *
 * mmEcc.c
 *
 * Main Memory Feature implementation file for ECC Initialization
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 87201 $ @e \$Date: 2013-01-30 11:25:53 -0600 (Wed, 30 Jan 2013) $
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


#include "Porting.h"
#include "AGESA.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "ma.h"
#include "mfmemclr.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMECC_FILECODE
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemMEcc (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMEcc (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  UINT8 Die;
  MEM_SHARED_DATA *SharedPtr;
  MEM_PARAMETER_STRUCT *RefPtr;
  BOOLEAN RetVal;

  RetVal = TRUE;
  RefPtr = mmPtr->MemPtr->ParameterListPtr;
  SharedPtr = mmPtr->mmSharedPtr;

  //
  // Run Northbridge-specific ECC initialization feature for each die.
  //
  SharedPtr->AllECC = FALSE;
  if (RefPtr->EnableEccFeature) {
    SharedPtr->AllECC = TRUE;
    AGESA_TESTPOINT (TpProcMemEccInitialization, &(mmPtr->MemPtr->StdHeader));

    for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
      mmPtr->NBPtr[Die].FeatPtr->CheckEcc (&(mmPtr->NBPtr[Die]));
      RetVal &= (BOOLEAN) (mmPtr->NBPtr[Die].MCTPtr->ErrCode < AGESA_FATAL);
    }
    if (SharedPtr->AllECC == TRUE) {
      RefPtr->GStatus[GsbAllECCDimms] = TRUE;
      // Sync mem clear before setting scrub rate.
      for (Die = 0; Die < mmPtr->DieCount; Die++) {
        MemFMctMemClr_Sync (&(mmPtr->NBPtr[Die]));
      }
    }
  }
  // Scrubber control
  for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
    mmPtr->NBPtr[Die].FeatPtr->InitEcc (&(mmPtr->NBPtr[Die]));
  }
  return RetVal;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function disable DRAM scrubber
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
VOID
MemMDisableScrubber (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  MEM_SHARED_DATA *SharedPtr;
  MEM_NB_BLOCK  *NBArray;

  SharedPtr = MemMainPtr->mmSharedPtr;
  NBArray = MemMainPtr->NBPtr;
  if (SharedPtr->AllECC == TRUE) {
    for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
      NBArray[Node].MemNDisableScrubber (&NBArray[Node]);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  This function restore the settings of DRAM scrubber
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 */
VOID
MemMRestoreScrubber (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  UINT8 Node;
  MEM_SHARED_DATA *SharedPtr;
  MEM_NB_BLOCK  *NBArray;

  SharedPtr = MemMainPtr->mmSharedPtr;
  NBArray = MemMainPtr->NBPtr;
  if (SharedPtr->AllECC == TRUE) {
    for (Node = 0; Node < MemMainPtr->DieCount; Node ++) {
      NBArray[Node].MemNRestoreScrubber (&NBArray[Node]);
    }
  }
}
