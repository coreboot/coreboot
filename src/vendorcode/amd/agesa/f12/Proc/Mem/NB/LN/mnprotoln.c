/* $NoKeywords:$ */
/**
 * @file
 *
 * mnprotoln.c
 *
 * Northbridge support functions for Errata and early samples
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/LN)
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



#include "AGESA.h"
#include "mm.h"
#include "mn.h"
#include "cpuRegisters.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_LN_MNPROTOLN_FILECODE

/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
MemNInitEarlySampleSupportLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
STATIC
MemNAfterMemClkFreqValLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
STATIC
MemNOverridePllMultValueLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllMult
  );

BOOLEAN
STATIC
MemNOverridePllDivValueLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllDiv
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
CONST UINT8 PllDivOverrideTab[]  = {0, 0, 0, 6, 4, 3, 3, 3};
CONST UINT8 PllMultOverrideTab[] = {0, 0, 0, 48, 42, 40, 48, 56};

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function initializes early sample support for Llano
 *
 *     @param[in,out] NBPtr  - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNInitEarlySampleSupportLN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F12_LN_A0) != 0) {
    if (MemNGetBitFieldNb (NBPtr, BFErratum468WorkaroundNotRequired) == 0) {
      NBPtr->FamilySpecificHook[AfterMemClkFreqVal] = MemNAfterMemClkFreqValLN;
      NBPtr->FamilySpecificHook[OverridePllMult] = MemNOverridePllMultValueLN;
      NBPtr->FamilySpecificHook[OverridePllDiv] = MemNOverridePllDivValueLN;
    }
  }

  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & (AMD_F12_LN_A0 | AMD_F12_LN_A1)) != 0) {
    NBPtr->NBRegTable[BFDoubleTrefRateEn] = 0;  // Erratum 445
    NBPtr->IsSupported[AdjustTwr] = TRUE;       // Erratum 434
    NBPtr->IsSupported[ChannelPDMode] = TRUE;   // Erratum 435
    NBPtr->NBRegTable[BFLowPowerDrvStrengthEn] = 0;
    NBPtr->IsSupported[SkipErrTrain] = FALSE;   // Rev A does not support skip error training
    NBPtr->IsSupported[DramSrHys] = FALSE;      // UBTS 233978
    NBPtr->IsSupported[SchedDlySlot1Extra] = FALSE;   // UBTS 244062
    NBPtr->IsSupported[ExtraPclkInMaxRdLat] = TRUE;   // UBTS 185210
    NBPtr->IsSupported[AdjustTrc] = FALSE;
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
 *     This function overrides PllMult and PllDiv bitfields
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNAfterMemClkFreqValLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  // BIOS needs to override PllMult and PllDiv as follow:
  // DDR800    48 / 6
  // DDR1066   42 / 5
  // DDR1333   no override
  // DDR1600   48 / 3
  if ((NBPtr->DCTPtr->Timings.Speed != DDR1333_FREQUENCY) && (NBPtr->DCTPtr->Timings.Speed != DDR1866_FREQUENCY)) {
    MemNBrdcstSetNb (NBPtr, BFDramPhyCtlReg, (MemNGetBitFieldNb (NBPtr, BFDramPhyCtlReg) & 0x7FFF) | (
                     (NBPtr->DCTPtr->Timings.Speed == DDR800_FREQUENCY) ? 0x09980000 :
                     (NBPtr->DCTPtr->Timings.Speed == DDR1066_FREQUENCY) ? 0x02950000 : 0x08980000));
  }
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function overrides PllMult variable
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  PllMult    - PllMult parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNOverridePllMultValueLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllMult
  )
{
  * (UINT8 *) PllMult = PllMultOverrideTab[NBPtr->DCTPtr->Timings.Speed / 133];
  return TRUE;
}

/*-----------------------------------------------------------------------------
 *
 *
 *     This function overrides PllDiv variable
 *
 *     @param[in,out]  *NBPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  PllDiv     - PllDiv parameter
 *
 *     @return    TRUE
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemNOverridePllDivValueLN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllDiv
  )
{
  * (UINT8 *) PllDiv = PllDivOverrideTab[NBPtr->DCTPtr->Timings.Speed / 133];
  return TRUE;
}
