/* $NoKeywords:$ */
/**
 * @file
 *
 * mpUkb3.c
 *
 * Platform specific settings for KB DDR3 UDIMM system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/KB)
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
  *
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "PlatformMemoryConfiguration.h"
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_KB_MPUKB3_FILECODE
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
/*
 *-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
// Max Freq. for UDIMM <6-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqKBUDIMM6L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 2, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 1, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 0, 2, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntU6L = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (MaxFreqKBUDIMM6L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqKBUDIMM6L
};

// Max Freq. for UDIMM <6-layer Motherboard Design> configuration for microserver
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqKBMicroSrvUDIMM6L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 2, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 1, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 0, 2, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntMicroSrvU6L = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (MaxFreqKBMicroSrvUDIMM6L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqKBMicroSrvUDIMM6L
};

// Max Freq. for UDIMM <4-layer Motherboard Design> configuration
// Format :
// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_5V,   Speed1_35V,   Speed1_25V
//
STATIC CONST PSCFG_MAXFREQ_ENTRY ROMDATA MaxFreqKBUDIMM4L[] = {
  {{_1DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_1DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 1, 1, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 1, 0, 1, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 2, 0, 0, DDR1600_FREQUENCY, DDR1600_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 1, 1, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
  {{_2DIMM, 2, 0, 2, 0, DDR1333_FREQUENCY, DDR1333_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY}},
};
CONST PSC_TBL_ENTRY KBMaxFreqTblEntU4L = {
   {PSCFG_MAXFREQ, UDIMM_TYPE, _1DIMM + _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (MaxFreqKBUDIMM4L) / sizeof (PSCFG_MAXFREQ_ENTRY),
   (VOID *)&MaxFreqKBUDIMM4L
};
