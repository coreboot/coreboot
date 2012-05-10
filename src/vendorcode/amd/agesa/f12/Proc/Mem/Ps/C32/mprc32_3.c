/* $NoKeywords:$ */
/**
 * @file
 *
 * mprc32_3.c
 *
 * Platform specific settings for C32 DDR3 R-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
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
#define FILECODE PROC_MEM_PS_C32_MPRC32_3_FILECODE
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
MemPDoPsRC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
STATIC
MemPGetPORFreqLimitRC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */

/*
 * ODT Settings for 1 or 2 Dimms Per Channel
 *
 * Speeds Supported, # of Dimms, # of QRDimms, DramTerm, QR DramTerm, Dynamic DramTerm
 */
STATIC CONST DRAM_TERM_ENTRY C32RDdr3DramTerm2D[] = {
  {DDR667 + DDR800 + DDR1066 + DDR1333 + DDR1600, ONE_DIMM, NO_DIMM, 1, 0, 0},
  {DDR667 + DDR800 + DDR1066, TWO_DIMM, NO_DIMM, 3, 0, 2},
  {DDR1333, TWO_DIMM, NO_DIMM, 5, 0, 2},
  {DDR1600, TWO_DIMM, NO_DIMM, 5, 0, 1},
  {DDR667 + DDR800 + DDR1066 + DDR1333, ONE_DIMM, ONE_DIMM, 0, 1, 2},
  {DDR1600, ONE_DIMM, ONE_DIMM, 0, 1, 1},
  {DDR667 + DDR800, TWO_DIMM, ONE_DIMM, 5, 1, 2},
  {DDR1066 + DDR1333 + DDR1600, TWO_DIMM, ONE_DIMM, 5, 1, 1},
  {DDR667 + DDR800, TWO_DIMM, TWO_DIMM, 0, 1, 2},
  {DDR1066 + DDR1333 + DDR1600, TWO_DIMM, TWO_DIMM, 0, 1, 1}
};
/*
 * ODT Settings for 3 Dimms Per Channel
 *
 * Speeds Supported, # of Dimms, # of QRDimms, DramTerm, QR DramTerm, Dynamic DramTerm
 */
STATIC CONST DRAM_TERM_ENTRY C32RDdr3DramTerm3D[] = {
  {DDR667 + DDR800 + DDR1066 + DDR1333 + DDR1600, ONE_DIMM, NO_DIMM, 1, 0, 0},
  {DDR667 + DDR800 + DDR1066, TWO_DIMM, NO_DIMM, 3, 0, 2},
  {DDR1333 + DDR1600, TWO_DIMM, NO_DIMM, 5, 0, 2},
  {DDR667 + DDR800 + DDR1066 + DDR1333 + DDR1600, THREE_DIMM, NO_DIMM, 3, 0, 2},
  {DDR667 + DDR800 + DDR1066 + DDR1333, ONE_DIMM, ONE_DIMM, 0, 1, 2},
  {DDR667 + DDR800, TWO_DIMM, ONE_DIMM, 5, 1, 2},
  {DDR1066 + DDR1333 + DDR1600, TWO_DIMM, ONE_DIMM, 5, 1, 1},
  {DDR667 + DDR800 + DDR1066 + DDR1333 + DDR1600, THREE_DIMM, ONE_DIMM, 3, 1, 2}
};
/*
 * POR Max Frequency supported for specific Dimm configurations for 1 Dimm Per Channel
 *
 * Dimm Config, # of Dimms, Max Freq @ 1.5V, Max Freq @ 1.35V, Max Freq @ 1.25
 */
STATIC CONST POR_SPEED_LIMIT C32RDdr3PSPorFreqLimit1D[] = {
  {SR_DIMM0 + DR_DIMM0, 1, DDR1333_FREQUENCY, DDR1333_FREQUENCY, 0},
  {QR_DIMM0, 1, DDR1333_FREQUENCY, DDR1333_FREQUENCY, 0}
};
/*
 * POR Max Frequency supported for specific Dimm configurations for 2 Dimms Per Channel
 *
 * Dimm Config, # of Dimms, Max Freq @ 1.5V, Max Freq @ 1.35V, Max Freq @ 1.25
 */
STATIC CONST POR_SPEED_LIMIT C32RDdr3PSPorFreqLimit2D[] = {
  {SR_DIMM1 + DR_DIMM1, 1, DDR1333_FREQUENCY, DDR1333_FREQUENCY, 0},
  {QR_DIMM1, 1, DDR1333_FREQUENCY, DDR1066_FREQUENCY, 0},
  {SR_DIMM0 + SR_DIMM1, 2, DDR1333_FREQUENCY, DDR1333_FREQUENCY, 0},
  {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1, 2, DDR1066_FREQUENCY, DDR1066_FREQUENCY, 0},
  {QR_DIMM0 + ANY_DIMM1, 2, DDR800_FREQUENCY, DDR667_FREQUENCY, 0},
  {ANY_DIMM0 + QR_DIMM1, 2, DDR800_FREQUENCY, DDR667_FREQUENCY, 0}
};
/*
 * POR Max Frequency supported for specific Dimm configurations for 3 Dimms Per Channel
 *
 * Dimm Config, # of Dimms, Max Freq @ 1.5V, Max Freq @ 1.35V, Max Freq @ 1.25
 */
STATIC CONST POR_SPEED_LIMIT C32RDdr3PSPorFreqLimit3D[] = {
  {SR_DIMM2 + DR_DIMM2, 1, DDR1333_FREQUENCY, DDR1333_FREQUENCY, 0},
  {SR_DIMM0 + SR_DIMM2, 2, DDR1333_FREQUENCY, DDR1333_FREQUENCY, 0},
  {SR_DIMM0 + DR_DIMM0 + SR_DIMM2 + DR_DIMM2, 2, DDR1066_FREQUENCY, DDR1066_FREQUENCY, 0},
  {QR_DIMM1, 1, DDR800_FREQUENCY, DDR800_FREQUENCY, 0},
  {QR_DIMM1 + SR_DIMM2 + DR_DIMM2, 2, DDR800_FREQUENCY, DDR667_FREQUENCY, 0},
  {SR_DIMM0 + SR_DIMM1 + SR_DIMM2, 3, DDR1066_FREQUENCY, DDR800_FREQUENCY, 0},
  {SR_DIMM0 + DR_DIMM0 + SR_DIMM1 + DR_DIMM1 + SR_DIMM2 + DR_DIMM2, 3, DDR800_FREQUENCY, DDR800_FREQUENCY, 0},
  {SR_DIMM0 + DR_DIMM0 + QR_DIMM1 + SR_DIMM2 + DR_DIMM2, 3, DDR667_FREQUENCY, DDR667_FREQUENCY, 0}

};
/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is the constructor platform specific settings for R DIMM-DDR3 C32 DDR3
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in,out]   *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in,out]   *PsPtr       Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_SUCCESS
 *
 */

AGESA_STATUS
MemPConstructPsRC32_3 (
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
  if (ChannelPtr->RegDimmPresent != ChannelPtr->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }
  PsPtr->MemPDoPs = MemPDoPsRC32_3;
  PsPtr->MemPGetPORFreqLimit = MemPGetPORFreqLimitRC32_3;
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for R-DDR3 C32 DDR3
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *     @return          TRUE - Find settings for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find settings for corresponding platform and dimm population.
 *
 */

BOOLEAN
STATIC
MemPDoPsRC32_3 (
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

  if ((MaxDimmsPerChannel == 1) || (MaxDimmsPerChannel == 2)) {
    DramTermSize = GET_SIZE_OF (C32RDdr3DramTerm2D);
    DramTermPtr = C32RDdr3DramTerm2D;
  } else if (MaxDimmsPerChannel == 3) {
    DramTermSize = GET_SIZE_OF (C32RDdr3DramTerm3D);
    DramTermPtr = C32RDdr3DramTerm3D;
  } else {
    IDS_ERROR_TRAP;
  }

  if (!MemPGetDramTerm (NBPtr, DramTermSize, DramTermPtr)) {
    return FALSE;
  }
  //
  // Special Cases for certain configs not covered by the table
  //
  // SR-SR-SR 1.5v @1066 (Currently only 3DPCH config at 1066)
  if ((MaxDimmsPerChannel == 3) && (NBPtr->ChannelPtr->Dimms == 3) &&
      (NBPtr->DCTPtr->Timings.Speed == DDR1066_FREQUENCY)) {
    NBPtr->PsPtr->DramTerm = 5; //30 Ohms
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function gets the POR speed limit for R-DDR3 C32 DDR3
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *
 */
VOID
STATIC
MemPGetPORFreqLimitRC32_3 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 *DimmsPerChPtr;
  UINT8 MaxDimmPerCH;
  UINT8 FreqLimitSize;
  UINT16 SpeedLimit;
  CONST POR_SPEED_LIMIT *FreqLimitPtr;
  DCT_STRUCT *DCTPtr;

  DCTPtr = NBPtr->DCTPtr;
  DimmsPerChPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmPerCH = *DimmsPerChPtr;
  } else {
    MaxDimmPerCH = 2;
  }

  if (MaxDimmPerCH == 4) {
    DCTPtr->Timings.DimmExclude |= DCTPtr->Timings.DctDimmValid;
    PutEventLog (AGESA_CRITICAL, MEM_ERROR_UNSUPPORTED_DIMM_CONFIG, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_CRITICAL, NBPtr->MCTPtr);
    // Change target speed to highest value so it won't affect other channels when leveling frequency across the node.
    NBPtr->DCTPtr->Timings.TargetSpeed = UNSUPPORTED_DDR_FREQUENCY;
    return;
  } else if (MaxDimmPerCH == 3) {
    FreqLimitPtr = C32RDdr3PSPorFreqLimit3D;
    FreqLimitSize = GET_SIZE_OF (C32RDdr3PSPorFreqLimit3D);
  } else if (MaxDimmPerCH == 2) {
    FreqLimitPtr = C32RDdr3PSPorFreqLimit2D;
    FreqLimitSize = GET_SIZE_OF (C32RDdr3PSPorFreqLimit2D);
  } else {
    FreqLimitPtr = C32RDdr3PSPorFreqLimit1D;
    FreqLimitSize = GET_SIZE_OF (C32RDdr3PSPorFreqLimit1D);
  }

  SpeedLimit = MemPGetPorFreqLimit (NBPtr, FreqLimitSize, FreqLimitPtr);

  if (SpeedLimit != 0) {
    if (DCTPtr->Timings.TargetSpeed > SpeedLimit) {
      DCTPtr->Timings.TargetSpeed = SpeedLimit;
    }
  } else {
    DCTPtr->Timings.DimmExclude |= DCTPtr->Timings.DctDimmValid;
    PutEventLog (AGESA_CRITICAL, MEM_ERROR_UNSUPPORTED_DIMM_CONFIG, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_CRITICAL, NBPtr->MCTPtr);
    // Change target speed to highest value so it won't affect other channels when leveling frequency across the node.
    NBPtr->DCTPtr->Timings.TargetSpeed = UNSUPPORTED_DDR_FREQUENCY;
  }
}