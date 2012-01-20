/* $NoKeywords:$ */
/**
 * @file
 *
 * mrntrain3.c
 *
 * Common Recovery Northbridge function for training flow for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 49896 $ @e \$Date: 2011-03-30 02:18:18 -0600 (Wed, 30 Mar 2011) $
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
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mrt3.h"
#include "Filecode.h"
#define FILECODE PROC_RECOVERY_MEM_NB_MRNTRAIN3_FILECODE
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
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the training control flow
 *      The DDR3 mode bit must be set prior to calling this function
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNRecTrainingFlowNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemRecTTrainDQSWriteHw3 (NBPtr->TechPtr);

  MemRecTTrainRcvrEnSw (NBPtr->TechPtr);

  MemRecTTrainDQSPosSw (NBPtr->TechPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the client training control flow
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNRecTrainingFlowClientNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart serial training\n");
  IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", NBPtr->MCTPtr->DieId);

  MemRecTTrainDQSWriteHw3 (NBPtr->TechPtr);

  MemRecTTrainRcvrEnHw (NBPtr->TechPtr);

  // Clear DisableCal and set DisablePredriverCal
  NBPtr->FamilySpecificHook[ReEnablePhyComp] (NBPtr, NBPtr);
  NBPtr->SetBitField (NBPtr, BFRxPtrInitReq, 1);
  while (NBPtr->GetBitField (NBPtr, BFRxPtrInitReq) != 0) {}
  NBPtr->SetBitField (NBPtr, BFDisDllShutdownSR, 1);
  NBPtr->SetBitField (NBPtr, BFEnterSelfRef, 1);
  while (NBPtr->GetBitField (NBPtr, BFEnterSelfRef) != 0) {}
  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemClkAlign = 2\n");
  NBPtr->SetBitField (NBPtr, BFDbeGskMemClkAlignMode, 2);
  NBPtr->SetBitField (NBPtr, BFExitSelfRef, 1);
  while (NBPtr->GetBitField (NBPtr, BFExitSelfRef) != 0) {}
  NBPtr->SetBitField (NBPtr, BFDisDllShutdownSR, 0);

  MemRecTTrainDQSPosSw (NBPtr->TechPtr);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function sets the Unb training control flow
 *
 *      @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNRecTrainingFlowUnb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemRecTTrainDQSWriteHw3 (NBPtr->TechPtr);

  MemRecTTrainDQSPosSw (NBPtr->TechPtr);

  MemRecTTrainRcvrEnHwSeedless (NBPtr->TechPtr);
}

