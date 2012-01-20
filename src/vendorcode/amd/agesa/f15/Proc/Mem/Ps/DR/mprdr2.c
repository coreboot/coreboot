/* $NoKeywords:$ */
/**
 * @file
 *
 * mprdr2.c
 *
 * Platform specific settings for DR DDR2 R-DIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 02:16:51 -0700 (Wed, 22 Dec 2010) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
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
#include "PlatformMemoryConfiguration.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_DR_MPRDR2_FILECODE
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
MemPDoPsRDr2 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
STATIC CONST DRAM_TERM_ENTRY DrUDdr2DramTerm[] = {
  {DDR400 + DDR533 + DDR667, ONE_DIMM, ANY_NUM, 1, 0, 0},
  {DDR400 + DDR533, TWO_DIMM + THREE_DIMM + FOUR_DIMM, ANY_NUM, 1, 0, 0},
  {DDR667, TWO_DIMM + THREE_DIMM, ANY_NUM, 1, 0, 0},
  {DDR667, FOUR_DIMM, ANY_NUM, 3, 0, 0},
  {DDR800, ONE_DIMM, ANY_NUM, 1, 0, 0},
  {DDR800, TWO_DIMM + THREE_DIMM + FOUR_DIMM, ANY_NUM, 3, 0, 0},
  {DDR1066, ONE_DIMM, ANY_NUM, 1, 0, 0}
};

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function is the constructor platform specific settings for R DIMM-DDR2 DR DDR2
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in,out]   *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in,out]   *PsPtr       Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_SUCCESS
 *
 */

AGESA_STATUS
MemPConstructPsRDr2 (
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
  if (ChannelPtr->TechType != DDR2_TECHNOLOGY) {
    return AGESA_UNSUPPORTED;
  }
  if (ChannelPtr->RegDimmPresent != ChannelPtr->ChDimmValid) {
    return AGESA_UNSUPPORTED;
  }
  PsPtr->MemPDoPs = MemPDoPsRDr2;
  PsPtr->MemPGetPORFreqLimit = MemPGetPORFreqLimitDef;
  return AGESA_SUCCESS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This is function sets the platform specific settings for R-DDR2 DR DDR2
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *     @return          TRUE - Find settings for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find settings for corresponding platform and dimm population.
 *
 */

BOOLEAN
STATIC
MemPDoPsRDr2 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if (!MemPGetDramTerm (NBPtr, GET_SIZE_OF (DrUDdr2DramTerm), DrUDdr2DramTerm)) {
    return FALSE;
  }

  return TRUE;
}

