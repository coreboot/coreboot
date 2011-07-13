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
 * @e \$Revision: 7081 $ @e \$Date: 2008-07-31 01:47:27 -0500 (Thu, 31 Jul 2008) $
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
MemTHwWlPart2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_TECH_FEAT_BLOCK memTechTrainingFeatDDR3;

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

  TechPtr = NBPtr->TechPtr;
  if (TechPtr->NBPtr->MCTPtr->NodeMemSize) {
    AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDQSTraining, &NBPtr->MemPtr->StdHeader);
    if (AgesaHookBeforeDQSTraining (0, TechPtr->NBPtr->MemPtr) == AGESA_SUCCESS) {
      // Right now we do not have anything to do if the callout is implemented
    }
    AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDQSTraining, &NBPtr->MemPtr->StdHeader);
    //Execute Technology specific training features
    if (memTechTrainingFeatDDR3.EnterHardwareTraining (TechPtr)) {
      if (memTechTrainingFeatDDR3.SwWLTraining (TechPtr)) {
        MemFInitTableDrive (NBPtr, MTAfterSwWLTrn);
        if (memTechTrainingFeatDDR3.HwBasedWLTrainingPart1 (TechPtr)) {
          MemFInitTableDrive (NBPtr, MTAfterHwWLTrnP1);
          if (memTechTrainingFeatDDR3.HwBasedDQSReceiverEnableTrainingPart1 (TechPtr)) {
            MemFInitTableDrive (NBPtr, MTAfterHwRxEnTrnP1);
            // If target speed is higher than start-up speed, do frequency change and second pass of WL
            if (MemTHwWlPart2 (TechPtr)) {
              if (memTechTrainingFeatDDR3.TrainExitHwTrn (TechPtr)) {
                IDS_OPTION_HOOK (IDS_PHY_DLL_STANDBY_CNTRL, NBPtr, &(NBPtr->MemPtr->StdHeader));
                if (memTechTrainingFeatDDR3.NonOptimizedSWDQSRecEnTrainingPart1 (TechPtr)) {
                  if (memTechTrainingFeatDDR3.OptimizedSwDqsRecEnTrainingPart1 (TechPtr)) {
                    MemFInitTableDrive (NBPtr, MTAfterSwRxEnTrn);
                    if (memTechTrainingFeatDDR3.NonOptimizedSRdWrPosTraining (TechPtr)) {
                      if (memTechTrainingFeatDDR3.OptimizedSRdWrPosTraining (TechPtr)) {
                        MemFInitTableDrive (NBPtr, MTAfterDqsRwPosTrn);
                        do {
                          if (memTechTrainingFeatDDR3.MaxRdLatencyTraining (TechPtr)) {
                            MemFInitTableDrive (NBPtr, MTAfterMaxRdLatTrn);
                          }
                        } while (NBPtr->ChangeNbFrequency (NBPtr));
                      }
                    }
                  }
                }
              }
            }
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
MemTHwWlPart2 (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  BOOLEAN retVal;
  retVal = TRUE;
  while (TechPtr->NBPtr->DCTPtr->Timings.TargetSpeed > TechPtr->NBPtr->DCTPtr->Timings.Speed) {
    TechPtr->PrevSpeed = TechPtr->NBPtr->DCTPtr->Timings.Speed;
    if (TechPtr->NBPtr->RampUpFrequency (TechPtr->NBPtr)) {
      if (!memTechTrainingFeatDDR3.HwBasedWLTrainingPart2 (TechPtr)) {
        retVal = FALSE;
        break;
      }
      MemFInitTableDrive (TechPtr->NBPtr, MTAfterHwWLTrnP2);
      if (!memTechTrainingFeatDDR3.HwBasedDQSReceiverEnableTrainingPart2 (TechPtr)) {
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
