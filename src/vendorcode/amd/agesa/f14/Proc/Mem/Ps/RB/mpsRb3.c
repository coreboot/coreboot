/* $NoKeywords:$ */
/**
 * @file
 *
 * mpsRb3.c
 *
 * Platform specific settings for RB DDR3 SO-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 35136 $ @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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

/* This file contains routine that add platform specific support L1 */


#include "AGESA.h"
#include "AdvancedApi.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "mu.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_RB_MPSRB3_FILECODE
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
MemPDoPsSRb3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemPGetPORFreqLimitSRb3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
STATIC CONST DRAM_TERM_ENTRY RbSDdr3DramTerm1D[] = {
  {DDR800, ONE_DIMM, NO_DIMM, 2, 0, 0},
  {DDR1066 + DDR1333 + DDR1600, ONE_DIMM, NO_DIMM, 1, 0, 0}
};

STATIC CONST DRAM_TERM_ENTRY RbSDdr3DramTerm2D[] = {
  {DDR800 + DDR1066 + DDR1333 + DDR1600, ONE_DIMM, NO_DIMM, 1, 0, 0},
  {DDR800, TWO_DIMM, NO_DIMM, 3, 0, 2},
  {DDR1066 + DDR1333, TWO_DIMM, NO_DIMM, 5, 0, 2},
  {DDR1600, TWO_DIMM, NO_DIMM, 5, 0, 1}
};
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is the constructor the platform specific settings for SO-DIMM RB DDR3
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in,out]   *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in,out]   *PsPtr       Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_SUCCESS
 *
 */

AGESA_STATUS
MemPConstructPsSRb3 (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  )
{
  ASSERT (MemPtr != 0);
  ASSERT (ChannelPtr != 0);

  if ((ChannelPtr->MCTPtr->LogicalCpuid.Family & AMD_FAMILY_10_RB) == 0) {
    return AGESA_UNSUPPORTED;
  }
  if (ChannelPtr->TechType != DDR3_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if (ChannelPtr->SODimmPresent != ChannelPtr->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }

  PsPtr->MemPDoPs = MemPDoPsSRb3;
  PsPtr->MemPGetPORFreqLimit = MemPGetPORFreqLimitSRb3;
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for SO-DIMM RB DDR3
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *     @return          TRUE - Find settings for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find settings for corresponding platform and dimm population.
 *
 */

BOOLEAN
STATIC
MemPDoPsSRb3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST DRAM_TERM_ENTRY *DramTermPtr;
  UINT8 MaxDimmsPerChannel;
  UINT8 *DimmsPerChPtr;
  UINT8 DramTermSize;

  DramTermSize = 0;
  DramTermPtr = NULL;
  DimmsPerChPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmsPerChannel = *DimmsPerChPtr;
  } else {
    MaxDimmsPerChannel = 2;
  }

  if (MaxDimmsPerChannel == 1) {
    DramTermSize = GET_SIZE_OF (RbSDdr3DramTerm1D);
    DramTermPtr = RbSDdr3DramTerm1D;
  } else if (MaxDimmsPerChannel == 2) {
    DramTermSize = GET_SIZE_OF (RbSDdr3DramTerm2D);
    DramTermPtr = RbSDdr3DramTerm2D;
  } else {
    IDS_ERROR_TRAP;
  }

  if (!MemPGetDramTerm (NBPtr, DramTermSize, DramTermPtr)) {
    return FALSE;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function gets the POR speed limit for SO-DDR3 RB
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *
 */
VOID
STATIC
MemPGetPORFreqLimitSRb3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 *DimmsPerChPtr;
  UINT8 MaxDimmPerCH;
  UINT16 SpeedLimit;

  DimmsPerChPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  if (MaxDimmPerCH == 1) {
    if (NBPtr->RefPtr->DDR3Voltage == VOLT1_5) {
      //
      // Highest POR supported speed for SODimm is 1333
      //
      SpeedLimit = DDR1333_FREQUENCY;
    } else {
      //
      // Max LV DDR3 Speed is 1066 for this silicon
      //
      SpeedLimit = DDR1066_FREQUENCY;
    }
  } else {
    //
    // Highest supported speed in 2DPC configuration is 1066
    //
    SpeedLimit = DDR1066_FREQUENCY;
    //
    // VOLT1_35 won't be supported while two DIMMs are populated in a channel
    //
    if ((NBPtr->RefPtr->DDR3Voltage == VOLT1_35) &&
       (NBPtr->ChannelPtr->Dimms == 2)) {
      NBPtr->RefPtr->DDR3Voltage = VOLT1_5;
      PutEventLog (AGESA_WARNING, MEM_WARNING_VOLTAGE_1_35_NOT_SUPPORTED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_WARNING, NBPtr->MCTPtr);
    }
  }
  if (NBPtr->DCTPtr->Timings.TargetSpeed > SpeedLimit) {
    NBPtr->DCTPtr->Timings.TargetSpeed = SpeedLimit;
  }
}
