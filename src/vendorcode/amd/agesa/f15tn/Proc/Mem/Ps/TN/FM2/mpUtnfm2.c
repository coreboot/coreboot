/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUtnfm2.c
 *
 * Platform specific settings for TN DDR3 UDIMM FM2 system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/TN/FM2)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2012, Advanced Micro Devices, Inc.
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
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "mu.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_TN_FM2_MPUTNFM2_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define SOCKET_FM2_TN   2

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
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
//
// MemClkDis
//
STATIC CONST UINT8 ROMDATA TNUDdr3CLKDisFM2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY TNClkDisMapEntUFM2 = {
   {PSCFG_CLKDIS, UDIMM_TYPE + SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, SOCKET_FM2_TN, DDR3_TECHNOLOGY},
   sizeof (TNUDdr3CLKDisFM2) / sizeof (UINT8),
   (VOID *)&TNUDdr3CLKDisFM2
};

//
// ODT tri-state
//
STATIC CONST UINT8 ROMDATA TNUDdr3ODTTriFM2[] = {0xFF, 0xFF, 0xFF, 0xFF};
CONST PSC_TBL_ENTRY TNUDdr3ODTTriEntFM2 = {
   {PSCFG_ODTTRI, UDIMM_TYPE + SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, SOCKET_FM2_TN, DDR3_TECHNOLOGY},
   sizeof (TNUDdr3ODTTriFM2) / sizeof (UINT8),
   (VOID *)&TNUDdr3ODTTriFM2
};

//
// ChipSel tri-state [UDIMM]
//
STATIC CONST UINT8 ROMDATA TNUDdr3CSTriFM2[] = {0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00};
CONST PSC_TBL_ENTRY TNUDdr3CSTriEntFM2 = {
   {PSCFG_CSTRI, UDIMM_TYPE + SODIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, SOCKET_FM2_TN, DDR3_TECHNOLOGY},
   sizeof (TNUDdr3CSTriFM2) / sizeof (UINT8),
   (VOID *)&TNUDdr3CSTriFM2
};

STATIC CONST PSCFG_S2D_ENTRY ex891_0 [] = {
  // DimmPerCh,Frequency,VDDIO,DIMM0,DIMM1,DIMM2,Enable2DTraining
  {1, DDR1333 + DDR1600 + DDR1866 + DDR2133, VOLT_ALL, DIMM_SR + DIMM_DR, NP, NP, 1},
  {2, DDR1333 + DDR1600 + DDR1866 + DDR2133, VOLT_ALL, NP + DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, NP, 1}
  };
CONST PSC_TBL_ENTRY ex891_1  = {
   {PSCFG_S2D, UDIMM_TYPE + SODIMM_TYPE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, SOCKET_FM2_TN, DDR3_TECHNOLOGY},
   sizeof (ex891_0 ) / sizeof (PSCFG_S2D_ENTRY),
   (VOID *)&ex891_0
};
