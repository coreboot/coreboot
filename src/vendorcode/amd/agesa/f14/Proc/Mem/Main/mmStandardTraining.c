/* $NoKeywords:$ */
/**
 * @file
 *
 * mmStandardTraining.c
 *
 * Main Memory Feature implementation file for Standard Training
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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


#include "Porting.h"
#include "AGESA.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "ma.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMSTANDARDTRAINING_FILECODE

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
BOOLEAN
MemMStandardTraining (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 * MemMStandardTraining
 *
 * This function implements standard memory training whereby training functions
 * for all nodes are run by the BSP.
 *
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMStandardTraining (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  UINT8 Die;
  //
  // Run Northbridge-specific Standard Training feature for each die.
  //
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart serial training\n");
  for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
    IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", Die);
    AGESA_TESTPOINT (TpProcMemBeforeAnyTraining, &(mmPtr->MemPtr->StdHeader));
    mmPtr->NBPtr[Die].BeforeDqsTraining (&mmPtr->NBPtr[Die]);
    mmPtr->NBPtr[Die].FeatPtr->Training (&mmPtr->NBPtr[Die]);
    mmPtr->NBPtr[Die].AfterDqsTraining (&mmPtr->NBPtr[Die]);
    if (mmPtr->NBPtr[Die].MCTPtr->ErrCode == AGESA_FATAL) {
      break;
    }
  }
  return (BOOLEAN) (Die == mmPtr->DieCount);
}
