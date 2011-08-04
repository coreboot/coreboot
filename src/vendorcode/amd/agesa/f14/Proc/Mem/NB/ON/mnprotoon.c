/* $NoKeywords:$ */
/**
 * @file
 *
 * mnprotoon.c
 *
 * Northbridge support functions for Errata and early samples
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/ON)
 * @e \$Revision: 37691 $ @e \$Date: 2010-09-10 04:28:23 +0800 (Fri, 10 Sep 2010) $
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



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "cpuRegisters.h"
#include "AdvancedApi.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_ON_MNPROTOON_FILECODE

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNForceAutoCompON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemNDetectMemPllErrorON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
VOID
MemNInitEarlySampleSupportON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function initializes early sample support for Ontario
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitEarlySampleSupportON (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F14_ON_A0) != 0) {
    NBPtr->IsSupported[DramSrHys] = FALSE;      // UBTS 233978
    NBPtr->NBRegTable[BFRxMaxDurDllNoLock] = 0;
    NBPtr->NBRegTable[BFTxMaxDurDllNoLock] = 0;
    NBPtr->FamilySpecificHook[ForceAutoComp] = MemNForceAutoCompON;
  }
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & (AMD_F14_ON_A0 | AMD_F14_ON_A1)) != 0) {
    NBPtr->FamilySpecificHook[DetectMemPllError] = MemNDetectMemPllErrorON;
  }
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *
 *
 *     This function forces auto compensation to be disabled all the time for
 *     ON A0
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNForceAutoCompON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  MemNSetBitFieldNb (NBPtr, BFPNOdtCal, 0x2020);
  MemNSetBitFieldNb (NBPtr, BFPNDrvCal, 0x8080);
  MemNSetBitFieldNb (NBPtr, BFCalVal, 0x8000);

  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function detects MemPll divide by 3 bug
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNDetectMemPllErrorON (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT16 Value;

  Value = 0xFFFF;
  if ((*(UINT8 *) OptParam) < 5) {
    LibAmdIoWrite (AccessWidth16, IDS_DEBUG_PORT, &Value, &NBPtr->MemPtr->StdHeader);
  }
  return TRUE;
}