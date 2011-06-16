/* $NoKeywords:$ */
/**
 * @file
 *
 * mrt3.c
 *
 * Common Technology  functions for DDR3  Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "mrport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mrt3.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_TECH_DDR3_MRT3_FILECODE
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
 *   This function Constructs the technology block
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemRecConstructTechBlock3 (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 Channel;
  for (Dct = 0; Dct < NBPtr->MCTPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    for (Channel = 0; Channel < NBPtr->DCTPtr->ChannelCount; Channel++) {
      NBPtr->SwitchChannel (NBPtr, Channel);
      NBPtr->ChannelPtr->TechType = DDR3_TECHNOLOGY;
    }
  }

  TechPtr->NBPtr = NBPtr;
  TechPtr->RefPtr = NBPtr->RefPtr;

  TechPtr->DramInit = MemRecTDramInitSw3;
  TechPtr->SetDramMode = MemRecTSetDramMode3;
  TechPtr->DimmPresence = MemRecTDIMMPresence3;
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
MemRecTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdReadCpuReg (CR4_REG, &TechPtr->CR4reg);
  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg | ((UINT32) 1 << 9)); // enable SSE2

  LibAmdMsrRead (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);            // HWCR
  TechPtr->HwcrLo = SMsr.lo;
  SMsr.lo |= 0x00020000;                  // turn on HWCR.wrap32dis
  SMsr.lo &= 0xFFFF7FFF;                  // turn off HWCR.SSEDIS
  LibAmdMsrWrite (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);
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
MemRecTEndTraining (
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
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
