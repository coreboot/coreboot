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
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
