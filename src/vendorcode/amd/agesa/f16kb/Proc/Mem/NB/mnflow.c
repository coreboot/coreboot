/* $NoKeywords:$ */
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
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
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



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_MNFLOW_FILECODE
/* features */
#include "mftds.h"

extern MEM_PSC_FLOW_BLOCK* memPlatSpecFlowArray[];
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

VOID
STATIC
MemNGetPORFreqLimitTblDrvNb (
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
  ID_INFO CallOutIdInfo;

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

          if (Flag && !NBPtr->IsSupported[TwoStageDramInit] && (NBPtr->MCTPtr->ErrCode != AGESA_FATAL)) {
            MemFInitTableDrive (NBPtr, MTBeforeDInit);
            AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));
            IDS_PERF_TIMESTAMP (TP_BEGINAGESAHOOKBEFOREDRAMINIT, &(NBPtr->MemPtr->StdHeader));
            CallOutIdInfo.IdField.SocketId = NBPtr->MCTPtr->SocketId;
            CallOutIdInfo.IdField.ModuleId = NBPtr->MCTPtr->DieId;
            IDS_HDT_CONSOLE (MEM_FLOW, "\nCalling out to Platform BIOS on Socket %d Module %d...\n", CallOutIdInfo.IdField.SocketId, CallOutIdInfo.IdField.ModuleId);
            AgesaHookBeforeDramInit ((UINTN) CallOutIdInfo.IdInformation, NBPtr->MemPtr);
            IDS_HDT_CONSOLE (MEM_FLOW, "\nVDDIO = 1.%dV\n", (NBPtr->RefPtr->DDR3Voltage == VOLT1_5) ? 5 :
                                                  (NBPtr->RefPtr->DDR3Voltage == VOLT1_35) ? 35 :
                                                  (NBPtr->RefPtr->DDR3Voltage == VOLT1_25) ? 25 : 999);
            AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));
            IDS_PERF_TIMESTAMP (TP_ENDAGESAHOOKBEFOREDRAMINIT, &(NBPtr->MemPtr->StdHeader));
            IDS_OPTION_HOOK (IDS_BEFORE_DRAM_INIT, NBPtr, &(NBPtr->MemPtr->StdHeader));
            NBPtr->StartupDCT (NBPtr);
          }
        }
      }
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode != AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes the platform specific block for families that support
 *      table driven form factor
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - AGESA_SUCCESS
 */

BOOLEAN
MemNPlatformSpecificFormFactorInitTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->PsPtr->MemPDoPs = MemPPSCFlow;
    NBPtr->PsPtr->MemPGetPORFreqLimit = MemNGetPORFreqLimitTblDrvNb;
    NBPtr->PsPtr->MemPGetPass1Seeds = MemPGetPSCPass1Seed;
  }

  return TRUE;
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This is function gets the POR speed limit for families supports table driven form factor
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
STATIC
MemNGetPORFreqLimitTblDrvNb (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;

  i = 0;
  while (memPlatSpecFlowArray[i] != NULL) {
    if ((memPlatSpecFlowArray[i])->MaxFrequency (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
      break;
    }
    i++;
  }
  // Check if there is no table found across CPU families. If so, disable channels.
  if (memPlatSpecFlowArray[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nDCT %d: No MaxFreq table. This channel will be disabled.\n", NBPtr->Dct);
    NBPtr->DCTPtr->Timings.DimmExclude |= NBPtr->DCTPtr->Timings.DctDimmValid;
    PutEventLog (AGESA_ERROR, MEM_ERROR_UNSUPPORTED_DIMM_CONFIG, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
    // Change target speed to highest value so it won't affect other channels when leveling frequency across the node.
    NBPtr->DCTPtr->Timings.TargetSpeed = UNSUPPORTED_DDR_FREQUENCY;
  }
}
