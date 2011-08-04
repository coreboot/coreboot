/* $NoKeywords:$ */
/**
 * @file
 *
 * mnflowon.c
 *
 * Llano initializer for MCT and DCT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
 * @e \$Revision: 34897 $ @e \$Date: 2010-07-14 10:07:10 +0800 (Wed, 14 Jul 2010) $
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



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mnon.h"
#include "mt.h"
#include "Filecode.h"
#include "GeneralServices.h"
#define FILECODE PROC_MEM_NB_ON_MNFLOWON_FILECODE
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
extern MEM_PLAT_SPEC_CFG* memPlatSpecFFInstalledON[MAX_FF_TYPES];

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function initializes the platform specific block
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - AGESA_SUCCESS at least one dorm factor was found
 *     @return          FALSE - AGESA_UNSUPPORTED - Error indicating that no form factors were found
 */

BOOLEAN
MemNPlatformSpecificFormFactorInitON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 f;

  if (NBPtr->MCTPtr->DimmValid == 0) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &(NBPtr->MemPtr->StdHeader));
    SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
    ASSERT(FALSE); // Size of memory on BSP = 0, so no DIMM found
    return FALSE; // There is no dimm present on the system.
  }
  for (f = 0; memPlatSpecFFInstalledON[f] != NULL; f++) {
    if (memPlatSpecFFInstalledON[f] (NBPtr->MemPtr, NBPtr->ChannelPtr, NBPtr->PsPtr) == AGESA_SUCCESS) {
      break;
    }
  }
  if (memPlatSpecFFInstalledON[f] == NULL) {
    return FALSE; // No FF types are supported
  }
  return TRUE;
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
MemNTechBlockSwitchON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_TECH_BLOCK *TechPtr;

  TechPtr = NBPtr->TechPtr;

  // Specify Dimm-Byte training for Nb
  MemTDimmByteTrainInit (TechPtr);

  // Remove the following functions because they are not needed for ON
  TechPtr->SetDramMode = (BOOLEAN (*) (MEM_TECH_BLOCK *)) memDefTrue;
  TechPtr->SpdCalcWidth = (BOOLEAN (*) (MEM_TECH_BLOCK *)) memDefTrue;
  TechPtr->SetDqsEccTmgs = (BOOLEAN (*) (MEM_TECH_BLOCK *)) memDefTrue;
  TechPtr->AdjustTwrwr = (VOID (*) (MEM_TECH_BLOCK *)) memDefRet;
  TechPtr->AdjustTwrrd = (VOID (*) (MEM_TECH_BLOCK *)) memDefRet;
  TechPtr->GetLD = (INT8 (*) (MEM_TECH_BLOCK *)) memDefRet;
  TechPtr->FindMaxDlyForMaxRdLat = MemTFindMaxRcvrEnDlyRdDqsDlyByte;
  TechPtr->ResetDCTWrPtr = (VOID (*) (MEM_TECH_BLOCK *, UINT8)) memDefRet;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

