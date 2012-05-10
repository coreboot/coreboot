/**
 * @file
 *
 * mnflow.c
 *
 * Common Northbridge initializer flow for MCT and DCT
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
#define FILECODE PROC_MEM_NB_MNFLOW_FILECODE
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
MemNInitDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemNCleanupDctRegsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function programs the MCT with initial values
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - AGESA_FATAL error did not occur (it is possible to have an Error that is not AGESA_SUCCESS)
 *     @return          FALSE - AGESA_FATAL error occurred
 */

BOOLEAN
MemNInitMCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  UINT8 Dct;
  BOOLEAN Flag;

  TechPtr = NBPtr->TechPtr;
  // Switch Tech functions for Nb
  NBPtr->TechBlockSwitch (NBPtr);
  // Start Memory controller initialization sequence
  Flag = FALSE;
  if (TechPtr->DimmPresence (TechPtr)) {
    AGESA_TESTPOINT (TpProcMemPlatformSpecificInit, &(NBPtr->MemPtr->StdHeader));
    if (NBPtr->MemNPlatformSpecificFormFactorInitNb (NBPtr)) {
      AGESA_TESTPOINT (TpProcMemSpdTiming, &(NBPtr->MemPtr->StdHeader));
      if (TechPtr->SpdCalcWidth (TechPtr)) {
        AGESA_TESTPOINT (TpProcMemSpeedTclConfig, &(NBPtr->MemPtr->StdHeader));
        if (TechPtr->SpdGetTargetSpeed (TechPtr)) {
          for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
            NBPtr->SwitchDCT (NBPtr, Dct);

            Flag |= MemNInitDCTNb (NBPtr);
          }

          if (Flag && (NBPtr->MCTPtr->ErrCode != AGESA_FATAL)) {
            MemFInitTableDrive (NBPtr, MTBeforeDInit);
            AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));
            AgesaHookBeforeDramInit (0, NBPtr->MemPtr);
            AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));
            IDS_OPTION_HOOK (IDS_BEFORE_DRAM_INIT, NBPtr, &(NBPtr->MemPtr->StdHeader));
            NBPtr->StartupDCT (NBPtr);
          }
        }
      }
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode != AGESA_FATAL);
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
 *      This function programs the DCT with initial values
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - Error did not occur
 *     @return          FALSE - Error occurred
 */

BOOLEAN
STATIC
MemNInitDCTNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;
  TechPtr = NBPtr->TechPtr;
  TechPtr->SetDramMode (TechPtr);

  if (!NBPtr->MCTPtr->GangedMode || (NBPtr->MCTPtr->Dct == 0)) {
    if (NBPtr->DCTPtr->Timings.DctDimmValid == 0) {
      NBPtr->DisableDCT (NBPtr);
    } else {
      MemNCleanupDctRegsNb (NBPtr);
      if (TechPtr->AutoCycTiming (TechPtr)) {
        if (TechPtr->SpdSetBanks (TechPtr)) {
          if (NBPtr->StitchMemory (NBPtr)) {
            // if all dimms on a DCT are disabled, the DCT needs to be disabled.
            if (NBPtr->DCTPtr->Timings.CsEnabled != 0) {
              if (NBPtr->AutoConfig (NBPtr)) {
                if (NBPtr->PlatformSpec (NBPtr)) {
                  return TRUE;
                }
              }
            } else {
              NBPtr->DisableDCT (NBPtr);
            }
          }
        }
      }
    }
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function selects appropriate Tech functions for the NB.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNTechBlockSwitchNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;

  // Specify Dimm-Byte training for Nb
  MemTDimmByteTrainInit (TechPtr);

  // Filter included for RcvrEn training.
  // note: If you'd like to drop the filter, you have to comment out these two lines together.
  TechPtr->MaxFilterDly = MAX_FILTER_DLY_DDR3;
  TechPtr->SaveRcvrEnDly = MemTSaveRcvrEnDlyByteFilter;
}

/*-----------------------------------------------------------------------------*/
/**
 *
 *      This function clears DCT registers
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNCleanupDctRegsNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  BIT_FIELD_NAME BitField;

  for (BitField = BFCSBaseAddr0Reg; BitField <= BFCSBaseAddr7Reg; BitField++) {
    MemNSetBitFieldNb (NBPtr, BitField, 0);
  }
}
