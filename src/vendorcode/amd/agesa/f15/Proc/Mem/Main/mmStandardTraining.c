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
 * @e \$Revision: 56279 $ @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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


#include "Porting.h"
#include "AGESA.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "ma.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_MAIN_MMSTANDARDTRAINING_FILECODE
/* features */
#include "mftds.h"
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

BOOLEAN
MemMStandardTraining (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

BOOLEAN
MemMStandardTrainingUsingAdjacentDies (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  );

/*-----------------------------------------------------------------------------
*                                EXPORTED FUNCTIONS
*
*-----------------------------------------------------------------------------
*/
extern MEM_FEAT_TRAIN_SEQ memTrainSequenceDDR3[];
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
    mmPtr->NBPtr[Die].Execute1dMaxRdLatTraining = TRUE;
    mmPtr->NBPtr[Die].FeatPtr->Training (&mmPtr->NBPtr[Die]);
    mmPtr->NBPtr[Die].TechPtr->TechnologySpecificHook[LrdimmSyncTrainedDlys] (mmPtr->NBPtr[Die].TechPtr, NULL);
    mmPtr->NBPtr[Die].AfterDqsTraining (&mmPtr->NBPtr[Die]);
    if (mmPtr->NBPtr[Die].MCTPtr->ErrCode == AGESA_FATAL) {
      break;
    }
  }
  return (BOOLEAN) (Die == mmPtr->DieCount);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 * MemMStandardTrainingUsingAdjacentDies
 *
 * This function implements standard memory training whereby training functions
 * for all nodes are run by the BSP while enabling other dies to eable argressor channel
 *
 *
 *     @param[in,out]   *mmPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */
BOOLEAN
MemMStandardTrainingUsingAdjacentDies (
  IN OUT   MEM_MAIN_DATA_BLOCK *mmPtr
  )
{
  UINT8 Die;
  UINT8 AdjacentDie;
  UINT32 AdjacentSocketNum;
  UINT32 TargetSocketNum;
  UINT32 ModuleNum;
  UINT8 i;
  UINT8 Dct;
  UINT8 ChipSel;
  BOOLEAN FirstCsFound;
  //
  // Run Northbridge-specific Standard Training feature for each die.
  //
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart standard serial training\n");
  for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
    IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", Die);
    AGESA_TESTPOINT (TpProcMemBeforeAnyTraining, &(mmPtr->MemPtr->StdHeader));
    mmPtr->NBPtr[Die].BeforeDqsTraining (&mmPtr->NBPtr[Die]);
    mmPtr->NBPtr[Die].Execute1dMaxRdLatTraining = FALSE;
    mmPtr->NBPtr[Die].FeatPtr->Training (&mmPtr->NBPtr[Die]);
    if (mmPtr->NBPtr[Die].MCTPtr->ErrCode == AGESA_FATAL) {
      break;
    }
  }
  IDS_HDT_CONSOLE (MEM_STATUS, "\nStart training with agressors\n");
  for (Die = 0 ; Die < mmPtr->DieCount ; Die ++ ) {
    IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", Die);
    AGESA_TESTPOINT (TpProcMemBeforeAnyTraining, &(mmPtr->MemPtr->StdHeader));
    GetSocketModuleOfNode (Die, &TargetSocketNum, &ModuleNum, &(mmPtr->MemPtr->StdHeader));
    for (AdjacentDie = 0; AdjacentDie < mmPtr->DieCount; AdjacentDie++) {
      mmPtr->NBPtr[Die].DieEnabled[AdjacentDie] = FALSE;
      GetSocketModuleOfNode (AdjacentDie, &AdjacentSocketNum, &ModuleNum, &(mmPtr->MemPtr->StdHeader));
      if (TargetSocketNum == AdjacentSocketNum) {
        if (AdjacentDie != Die) {
          if (mmPtr->NBPtr[AdjacentDie].MCTPtr->NodeMemSize != 0) {
            mmPtr->NBPtr[Die].AdjacentDieNBPtr = &mmPtr->NBPtr[AdjacentDie];
            mmPtr->NBPtr[Die].DieEnabled[AdjacentDie] = TRUE;
          }
        } else {
          if (mmPtr->NBPtr[Die].MCTPtr->NodeMemSize != 0) {
            mmPtr->NBPtr[Die].DieEnabled[Die] = TRUE;
          }
        }
        // Determine the initial target CS, Max Dimms and max CS number for all DCTs (potential aggressors)
        if (mmPtr->NBPtr[AdjacentDie].MCTPtr->NodeMemSize != 0) {
          for (Dct = 0; Dct < mmPtr->NBPtr[AdjacentDie].DctCount; Dct++) {
            FirstCsFound = FALSE;
            mmPtr->NBPtr[AdjacentDie].SwitchDCT (&mmPtr->NBPtr[AdjacentDie], Dct);
            for (ChipSel = 0; ChipSel < mmPtr->NBPtr[AdjacentDie].CSPerChannel (&mmPtr->NBPtr[AdjacentDie]); ChipSel = ChipSel + mmPtr->NBPtr[AdjacentDie].CSPerDelay (&mmPtr->NBPtr[AdjacentDie]) ) {
              if ((mmPtr->NBPtr[AdjacentDie].DCTPtr->Timings.CsEnabled & ((UINT16) 1 << ChipSel)) != 0) {
                if (FirstCsFound == FALSE) {
                  // Set Initial CS value for Current Aggressor CS
                  mmPtr->NBPtr[AdjacentDie].InitialAggressorCSTarget[Dct] = ChipSel;
                  mmPtr->NBPtr[AdjacentDie].CurrentAggressorCSTarget[Dct] = mmPtr->NBPtr[AdjacentDie].InitialAggressorCSTarget[Dct];
                  FirstCsFound = TRUE;
                }
                mmPtr->NBPtr[AdjacentDie].MaxAggressorCSEnabled[Dct] = ChipSel;
                mmPtr->NBPtr[AdjacentDie].MaxAggressorDimms[Dct]++;
              }
            }
          }
        }
      }
    }
    if (mmPtr->NBPtr[Die].MCTPtr->NodeMemSize != 0) {
      //Execute Technology specific training features
      i = 0;
      while (memTrainSequenceDDR3[i].TrainingSequenceEnabled != 0) {
        if (memTrainSequenceDDR3[i].TrainingSequenceEnabled (&mmPtr->NBPtr[Die])) {
          mmPtr->NBPtr[Die].TrainingSequenceIndex = i;
          // Execute RdDqs Training
          memTrainSequenceDDR3[i].MemTechFeatBlock->RdDqs__Training (mmPtr->NBPtr[Die].TechPtr);
          // Execute MaxRdLat Training After training
          do {
            if (memTrainSequenceDDR3[i].MemTechFeatBlock->MaxRdLatencyTraining (mmPtr->NBPtr[Die].TechPtr)) {
              MemFInitTableDrive (&mmPtr->NBPtr[Die], MTAfterMaxRdLatTrn);
            }
          } while (mmPtr->NBPtr->ChangeNbFrequency (&mmPtr->NBPtr[Die]));
          break;
        }
        i++;
      }
    }
    mmPtr->NBPtr[Die].TechPtr->TechnologySpecificHook[LrdimmSyncTrainedDlys] (mmPtr->NBPtr[Die].TechPtr, NULL);
    mmPtr->NBPtr[Die].AfterDqsTraining (&mmPtr->NBPtr[Die]);
    if (mmPtr->NBPtr[Die].MCTPtr->ErrCode == AGESA_FATAL) {
      break;
    }
  }
  return (BOOLEAN) (Die == mmPtr->DieCount);
}

