/* $NoKeywords:$ */
/**
 * @file
 *
 * mpuon3.c
 *
 * Platform specific settings for ON DDR3 U-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/ON)
 * @e \$Revision: 46937 $ @e \$Date: 2011-02-11 08:50:58 -0700 (Fri, 11 Feb 2011) $
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
#include "Ids.h"
#include "AdvancedApi.h"
#include "mport.h"
#include "PlatformMemoryConfiguration.h"
#include "ma.h"
#include "cpuFamRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_PS_ON_MPUON3_FILECODE
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
MemPDoPsUON3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemPGetPORFreqLimitUON3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

STATIC CONST DRAM_TERM_ENTRY OnUDdr3DramTerm[] = {
  {DDR800 + DDR1066, ONE_DIMM, NO_DIMM, 2, 0, 0},
  {DDR1333, ONE_DIMM, NO_DIMM, 1, 0, 0},
  {DDR800 + DDR1066, TWO_DIMM, NO_DIMM, 3, 0, 2},
  {DDR1333, TWO_DIMM, NO_DIMM, 5, 0, 2}
};

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is the constructor for the platform specific settings for U-DDR3 ON DDR3
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in,out]   *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in,out]   *PsPtr       Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_SUCCESS
 *
 */

AGESA_STATUS
MemPConstructPsUON3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  )
{
  ASSERT (MemPtr != 0);
  ASSERT (ChannelPtr != 0);

  if ((ChannelPtr->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_14_ON) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (ChannelPtr->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if ((ChannelPtr->RegDimmPresent != 0) || (ChannelPtr->SODimmPresent != 0)) {
    return AGESA_UNSUPPORTED;
  }
  PsPtr->MemPDoPs = MemPDoPsUON3;

  if ((ChannelPtr->MCTPtr->LogicalCpuid.Revision & AMD_F14_ON_Cx) == 0) {
  PsPtr->MemPGetPORFreqLimit = MemPGetPORFreqLimitUON3;
  } else {
    // Do not force frequency limit for Rev C
    PsPtr->MemPGetPORFreqLimit = (VOID (*) (MEM_NB_BLOCK *)) memDefRet;
  }

  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for U-DDR3 ON DDR3
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *     @return          TRUE - Find settings for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find settings for corresponding platform and dimm population.
 *
 */

BOOLEAN
STATIC
MemPDoPsUON3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (!MemPGetDramTerm (NBPtr, GET_SIZE_OF (OnUDdr3DramTerm), OnUDdr3DramTerm)) {
    return FALSE;
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function gets the POR speed limit for U-DDR3 of ON
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *
 */

VOID
STATIC
MemPGetPORFreqLimitUON3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if ((NBPtr->DCTPtr->Timings.TargetSpeed == DDR1333_FREQUENCY) && (MemNGetBitFieldNb (NBPtr, BFFixedErrataSkipPorFreqCap) == 0)) {
    NBPtr->DCTPtr->Timings.TargetSpeed = DDR1066_FREQUENCY;
  }
}