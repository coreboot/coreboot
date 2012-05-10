/**
 * @file
 *
 * mpuc32_3.c
 *
 * Platform specific settings for C32 DDR3 U-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 8446 $ @e \$Date: 2008-09-23 10:52:09 -0500 (Tue, 23 Sep 2008) $
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


#include "AGESA.h"
#include "AdvancedApi.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "PlatformMemoryConfiguration.h"
#include "GeneralServices.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_PS_C32_MPUC32_3_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define AMD_FAMILY_10_C32    AMD_FAMILY_10_HY

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
MemPDoPsUC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemPGetPORFreqLimitUC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
STATIC CONST DRAM_TERM_ENTRY C32UDdr3DramTerm[] = {
  {DDR800 + DDR1066 + DDR1333 + DDR1600, ONE_DIMM, NO_DIMM, 1, 0, 0},
  {DDR800 + DDR1066, TWO_DIMM, NO_DIMM, 3, 0, 2},
  {DDR1333, TWO_DIMM, NO_DIMM, 5, 0, 2},
  {DDR1600, TWO_DIMM, NO_DIMM, 5, 0, 1}
};

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is the constructor for the platform specific settings for U-DDR3 C32 DDR3
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in,out]   *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in,out]   *PsPtr       Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_SUCCESS
 *
 */

AGESA_STATUS
MemPConstructPsUC32_3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  )
{
  ASSERT (MemPtr != 0);
  ASSERT (ChannelPtr != 0);

  if ((ChannelPtr->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_10_C32) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (ChannelPtr->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if ((ChannelPtr->RegDimmPresent != 0) || (ChannelPtr->SODimmPresent != 0)) {
    return AGESA_UNSUPPORTED;
  }
  PsPtr->MemPDoPs = MemPDoPsUC32_3;
  PsPtr->MemPGetPORFreqLimit = MemPGetPORFreqLimitUC32_3;
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for U-DDR3 C32 DDR3
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *     @return          TRUE - Find settings for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find settings for corresponding platform and dimm population.
 *
 */

BOOLEAN
STATIC
MemPDoPsUC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (!MemPGetDramTerm (NBPtr, GET_SIZE_OF (C32UDdr3DramTerm), C32UDdr3DramTerm)) {
    return FALSE;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function gets the POR speed limit for SO-DDR3 C32
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *
 */
VOID
STATIC
MemPGetPORFreqLimitUC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT16 MaxSpeed;
  //
  // For 2/2 or 2/3 DPCH where one is a DR, Max Speed is 1066
  //
  if ( (NBPtr->ChannelPtr->Dimms >= 2) && ((NBPtr->ChannelPtr->DimmDrPresent & 0x07) != 0) ) {
    MaxSpeed = DDR1066_FREQUENCY;
  } else {
    //
    // Highest POR supported speed for Unbuffered dimm is 1333
    //
    MaxSpeed = DDR1333_FREQUENCY;
  }
  if (NBPtr->DCTPtr->Timings.TargetSpeed > MaxSpeed) {
    NBPtr->DCTPtr->Timings.TargetSpeed = MaxSpeed;
  } else if (NBPtr->DCTPtr->Timings.TargetSpeed == DDR667_FREQUENCY) {
    // Unbuffered DDR3 at 333MHz is not supported
    NBPtr->DCTPtr->Timings.DimmExclude |= NBPtr->DCTPtr->Timings.DctDimmValid;
    PutEventLog (AGESA_ERROR, MEM_ERROR_UNSUPPORTED_333MHZ_UDIMM, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
    // Change target speed to highest value so it won't affect other channels when leveling frequency across the node.
    NBPtr->DCTPtr->Timings.TargetSpeed = UNSUPPORTED_DDR_FREQUENCY;
  }
}