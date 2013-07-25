/* $NoKeywords:$ */
/**
 * @file
 *
 * mpkb3.c
 *
 * Platform specific settings for KB
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
#include "mport.h"
#include "ma.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "mm.h"
#include "mn.h"
#include "mp.h"
#include "Filecode.h"

CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)



#define FILECODE PROC_MEM_PS_KB_MPKB3_FILECODE
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
//
// Common tables of KB platform specific configuration
//

// MR0[WR]
// Format :
// D18F2x22C_dct[1:0][Twr], MR0[WR]
//
CONST PSCFG_MR0WR_ENTRY KBMR0WR[] = {
  {0x10, 0},
  {0x05, 1},
  {0x06, 2},
  {0x07, 3},
  {0x08, 4},
  {0x0A, 5},
  {0x0C, 6},
  {0x0E, 7},
  {0x12, 9}
};
CONST PSC_TBL_ENTRY KBMR0WrTblEntry = {
   {PSCFG_MR0WR, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMR0WR) / sizeof (PSCFG_MR0WR_ENTRY),
   (VOID *)&KBMR0WR
};

// MR0[CL]
// Format :
// D18F2x200_dct[1:0][Tcl], MR0[CL][3:1], MR0[CL][0]
//
CONST PSCFG_MR0CL_ENTRY KBMR0CL[] = {
  {0x05, 1, 0},
  {0x06, 2, 0},
  {0x07, 3, 0},
  {0x08, 4, 0},
  {0x09, 5, 0},
  {0x0A, 6, 0},
  {0x0B, 7, 0},
  {0x0C, 0, 1},
  {0x0D, 1, 1},
  {0x0E, 2, 1},
  {0x0F, 3, 1},
  {0x10, 4, 1}
};
CONST PSC_TBL_ENTRY KBMR0CLTblEntry = {
   {PSCFG_MR0CL, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBMR0CL) / sizeof (PSCFG_MR0CL_ENTRY),
   (VOID *)&KBMR0CL
};

// ODT pattern for 1 DPC
// Format:
//  Dimm0,   RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_1D_ODTPAT_ENTRY KB1DOdtPat[] = {
  {DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00000001},
  {DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x00000201},
};
CONST PSC_TBL_ENTRY KB1DOdtPatTblEnt = {
   {PSCFG_ODT_PAT_1D, DT_DONT_CARE, _1DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KB1DOdtPat) / sizeof (PSCFG_1D_ODTPAT_ENTRY),
   (VOID *)&KB1DOdtPat
};

// ODT pattern for 2 DPC
// Format:
//  Dimm0, Dimm1  RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_2D_ODTPAT_ENTRY KB2DOdtPat[] = {
  {NP, DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00040000},
  {NP, DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x08040000},
  {DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, 0x00000000, 0x01010404, 0x00000000, 0x09050605}
};
CONST PSC_TBL_ENTRY KB2DOdtPatTblEnt = {
   {PSCFG_ODT_PAT_2D, DT_DONT_CARE, _2DIMM, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KB2DOdtPat) / sizeof (PSCFG_2D_ODTPAT_ENTRY),
   (VOID *)&KB2DOdtPat
};

//
// CKE tri-state
//
STATIC CONST UINT8 ROMDATA KBDdr3CKETri[] = {0xFF, 0xFF, 0xFF, 0xFF};
CONST PSC_TBL_ENTRY KBDdr3CKETriEnt = {
   {PSCFG_CKETRI, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_16_KB, AMD_F16_ALL}, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (KBDdr3CKETri) / sizeof (UINT8),
   (VOID *)&KBDdr3CKETri
};
