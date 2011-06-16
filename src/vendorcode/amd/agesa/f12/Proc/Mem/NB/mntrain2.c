/* $NoKeywords:$ */
/**
 * @file
 *
 * mntrain2.c
 *
 * Common Northbridge function for training flow for DDR2
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
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
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MNTRAIN2_FILECODE
/* features */
#include "mftds.h"
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
extern MEM_TECH_FEAT_BLOCK memTechTrainingFeatDDR2;

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates DQS training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemNDQSTiming2Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;
  if (TechPtr->NBPtr->MCTPtr->NodeMemSize) {
    AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDQSTraining, &NBPtr->MemPtr->StdHeader);
    AgesaHookBeforeDQSTraining (NBPtr->MCTPtr->SocketId, TechPtr->NBPtr->MemPtr);
    AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDQSTraining, &NBPtr->MemPtr->StdHeader);
    //Execute Technology specific training features
    if (memTechTrainingFeatDDR2.NonOptimizedSWDQSRecEnTrainingPart1 (TechPtr)) {
      if (memTechTrainingFeatDDR2.OptimizedSwDqsRecEnTrainingPart1 (TechPtr)) {
        MemFInitTableDrive (NBPtr, MTAfterSwRxEnTrn);
        if (memTechTrainingFeatDDR2.NonOptimizedSRdWrPosTraining (TechPtr)) {
          if (memTechTrainingFeatDDR2.OptimizedSRdWrPosTraining (TechPtr)) {
            MemFInitTableDrive (NBPtr, MTAfterDqsRwPosTrn);
            if (memTechTrainingFeatDDR2.MaxRdLatencyTraining (TechPtr)) {
              MemFInitTableDrive (NBPtr, MTAfterMaxRdLatTrn);
            }
          }
        }
      }
    }
    MemTMarkTrainFail (TechPtr);
  }
  return TRUE;
}
