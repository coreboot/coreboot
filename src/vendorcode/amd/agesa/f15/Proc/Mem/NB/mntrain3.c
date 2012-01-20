/* $NoKeywords:$ */
/**
 * @file
 *
 * mntrain3.c
 *
 * Common Northbridge function for training flow for DDR3
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB)
 * @e \$Revision: 59563 $ @e \$Date: 2011-09-26 12:06:49 -0600 (Mon, 26 Sep 2011) $
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
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_NB_MNTRAIN3_FILECODE
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

BOOLEAN
STATIC
MemNHwWlPart2Nb (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[];
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates DQS training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
MemNDQSTiming3Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 i;
  BOOLEAN Retval;
  TechPtr = NBPtr->TechPtr;
  Retval = TRUE;
  if (TechPtr->NBPtr->MCTPtr->NodeMemSize) {
    //Execute Technology specific training features
    i = 0;
    while (memTrainSequenceDDR3[i].TrainingSequenceEnabled != 0) {
      if (memTrainSequenceDDR3[i].TrainingSequenceEnabled (NBPtr)) {
        NBPtr->TrainingSequenceIndex = i;
        Retval = memTrainSequenceDDR3[i].TrainingSequence (NBPtr);
        break;
      }
      i++;
    }
  }
  return Retval;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function initiates DQS training for Server NB
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

BOOLEAN
memNSequenceDDR3Nb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 i;
  TechPtr = NBPtr->TechPtr;
  i = NBPtr->TrainingSequenceIndex;
  if (TechPtr->NBPtr->MCTPtr->NodeMemSize != 0) {
    AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDQSTraining, &NBPtr->MemPtr->StdHeader);
    IDS_HDT_CONSOLE (MEM_FLOW, "\nCalling out to Platform BIOS...\n");
    if (AgesaHookBeforeDQSTraining (NBPtr->MCTPtr->SocketId, TechPtr->NBPtr->MemPtr) == AGESA_SUCCESS) {
      // Right now we do not have anything to do if the callout is implemented
    }
    AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDQSTraining, &NBPtr->MemPtr->StdHeader);
    //Execute Technology specific training features
    if (memTrainSequenceDDR3[i].MemTechFeatBlock->EnterHardwareTraining (TechPtr)) {
      TechPtr->TechnologySpecificHook[LrdimmBuf2DramTrain] (TechPtr, NULL);
      if (memTrainSequenceDDR3[i].MemTechFeatBlock->SwWLTraining (TechPtr)) {
        MemFInitTableDrive (NBPtr, MTAfterSwWLTrn);
        if (memTrainSequenceDDR3[i].MemTechFeatBlock->HwBasedWLTrainingPart1 (TechPtr)) {
          MemFInitTableDrive (NBPtr, MTAfterHwWLTrnP1);
          if (memTrainSequenceDDR3[i].MemTechFeatBlock->HwBasedDQSReceiverEnableTrainingPart1 (TechPtr)) {
            MemFInitTableDrive (NBPtr, MTAfterHwRxEnTrnP1);
            // If target speed is higher than start-up speed, do frequency change and second pass of WL
            do {
              if (MemNHwWlPart2Nb (TechPtr)) {
                if (memTrainSequenceDDR3[i].MemTechFeatBlock->TrainExitHwTrn (TechPtr)) {
                  IDS_OPTION_HOOK (IDS_PHY_DLL_STANDBY_CTRL, NBPtr, &(NBPtr->MemPtr->StdHeader));
                  if (memTrainSequenceDDR3[i].MemTechFeatBlock->NonOptimizedSWDQSRecEnTrainingPart1 (TechPtr)) {
                    if (memTrainSequenceDDR3[i].MemTechFeatBlock->OptimizedSwDqsRecEnTrainingPart1 (TechPtr)) {
                      MemFInitTableDrive (NBPtr, MTAfterSwRxEnTrn);
                      if (memTrainSequenceDDR3[i].MemTechFeatBlock->NonOptimizedSRdWrPosTraining (TechPtr)) {
                        if (memTrainSequenceDDR3[i].MemTechFeatBlock->OptimizedSRdWrPosTraining (TechPtr)) {
                          MemFInitTableDrive (NBPtr, MTAfterDqsRwPosTrn);
                          if (!NBPtr->FamilySpecificHook[MemPstateStageChange] (NBPtr, NULL)) {
                            continue;
                          }
                          if (NBPtr->Execute1dMaxRdLatTraining) {
                            do {
                              if (memTrainSequenceDDR3[i].MemTechFeatBlock->MaxRdLatencyTraining (TechPtr)) {
                                MemFInitTableDrive (NBPtr, MTAfterMaxRdLatTrn);
                              }
                            } while (NBPtr->ChangeNbFrequency (NBPtr));
						  } else {
                            // If not running MRL training, set everything back for training
							memTrainSequenceDDR3[i].MemTechFeatBlock->TrainExitHwTrn (TechPtr);
                          }
                        }
                      }
                    }
                  }
                }
              }
            } while (NBPtr->MemPstateStage == MEMORY_PSTATE_2ND_STAGE);
          }
        }
      }
    }
    MemTMarkTrainFail (TechPtr);
  }
  return TRUE;
}
/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function executes HW WL at multiple speeds
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @return       TRUE - No errors occurred
 *                   FALSE - errors occurred
 */

BOOLEAN
STATIC
MemNHwWlPart2Nb (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  BOOLEAN retVal;
  UINT8 i;
  retVal = TRUE;
  i = TechPtr->NBPtr->TrainingSequenceIndex;
  while ((TechPtr->NBPtr->DCTPtr->Timings.TargetSpeed > TechPtr->NBPtr->DCTPtr->Timings.Speed) && (TechPtr->NBPtr->MemPstateStage != MEMORY_PSTATE_1ST_STAGE)) {
    TechPtr->PrevSpeed = TechPtr->NBPtr->DCTPtr->Timings.Speed;
    if (TechPtr->NBPtr->RampUpFrequency (TechPtr->NBPtr)) {
      TechPtr->TechnologySpecificHook[LrdimmBuf2DramTrain] (TechPtr, NULL);
      if (!memTrainSequenceDDR3[i].MemTechFeatBlock->HwBasedWLTrainingPart2 (TechPtr)) {
        retVal = FALSE;
        break;
      }
      MemFInitTableDrive (TechPtr->NBPtr, MTAfterHwWLTrnP2);
      if (!memTrainSequenceDDR3[i].MemTechFeatBlock->HwBasedDQSReceiverEnableTrainingPart2 (TechPtr)) {
        retVal = FALSE;
        break;
      }
      MemFInitTableDrive (TechPtr->NBPtr, MTAfterHwRxEnTrnP2);
    } else {
      retVal = FALSE;
      break;
    }
  }
  return retVal;
}
