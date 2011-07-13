/**
 * @file
 *
 * mt.c
 *
 * Common Technology file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
 * @e \$Revision: 6789 $ @e \$Date: 2008-07-17 15:56:25 -0500 (Thu, 17 Jul 2008) $
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
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mu.h"
#include "mt.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_TECH_MT_FILECODE
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the default return for non-training technology features
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
BOOLEAN
MemTFeatDef (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the TestFail bit for all CS that fail training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
VOID
MemTMarkTrainFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  UINT8 Dct;
  UINT8 ChipSel;

  NBPtr = TechPtr->NBPtr;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.CsEnabled &= ~NBPtr->DCTPtr->Timings.CsTrainFail;
    for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel ++) {
      if ((NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16)1 << ChipSel)) != 0) {
        NBPtr->SetBitField (NBPtr, (BFCSBaseAddr0Reg + ChipSel), (UINT32)1 << BFTestFail);
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the initial controller environment before training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdReadCpuReg (CR4_REG, &TechPtr->CR4reg);
  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg | ((UINT32)1 << 9)); // enable SSE2

  LibAmdMsrRead (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);            // HWCR
  TechPtr->HwcrLo = SMsr.lo;
  SMsr.lo |= 0x00020000;                  // turn on HWCR.wrap32dis
  SMsr.lo &= 0xFFFF7FFF;                  // turn off HWCR.SSEDIS
  LibAmdMsrWrite (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);

  TechPtr->DramEcc = (UINT8) NBPtr->GetBitField (NBPtr, BFDramEccEn);
  NBPtr->SetBitField (NBPtr, BFDramEccEn, 0); // Disable ECC
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the final controller environment after training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg);

  LibAmdMsrRead (HWCR, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo = TechPtr->HwcrLo;
  LibAmdMsrWrite (HWCR, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  NBPtr->SetBitField (NBPtr, BFDramEccEn, TechPtr->DramEcc);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets all the bytelanes/nibbles to the same delay value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dly - Delay value to set
 *
 */

VOID
MemTSetDQSDelayAllCSR (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dly
  )
{
  UINT8 i;

  for (i = 0; i < TechPtr->MaxByteLanes (); i++) {
    TechPtr->SetDQSDelayCSR (TechPtr, i, Dly);
  }
}
