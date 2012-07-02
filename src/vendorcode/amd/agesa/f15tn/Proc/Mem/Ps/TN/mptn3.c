/* $NoKeywords:$ */
/**
 * @file
 *
 * mptn3.c
 *
 * Platform specific settings for TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps/TN)
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
  *
  * Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
  *
  * AMD is granting you permission to use this software (the Materials)
  * pursuant to the terms and conditions of your Software License Agreement
  * with AMD.  This header does *NOT* give you permission to use the Materials
  * or any rights under AMD's intellectual property.  Your use of any portion
  * of these Materials shall constitute your acceptance of those terms and
  * conditions.  If you do not agree to the terms and conditions of the Software
  * License Agreement, please do not use any portion of these Materials.
  *
  * CONFIDENTIALITY:  The Materials and all other information, identified as
  * confidential and provided to you by AMD shall be kept confidential in
  * accordance with the terms and conditions of the Software License Agreement.
  *
  * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
  * PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
  * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
  * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
  * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
  * IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
  * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
  * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
  * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
  * RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
  * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
  * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
  * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
  *
  * AMD does not assume any responsibility for any errors which may appear in
  * the Materials or any other related information provided to you by AMD, or
  * result from use of the Materials or any related information.
  *
  * You agree that you will not reverse engineer or decompile the Materials.
  *
  * NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
  * further information, software, technical information, know-how, or show-how
  * available to you.  Additionally, AMD retains the right to modify the
  * Materials at any time, without notice, and is not obligated to provide such
  * modified Materials to you.
  *
  * U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
  * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
  * subject to the restrictions as set forth in FAR 52.227-14 and
  * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
  * Government constitutes acknowledgement of AMD's proprietary rights in them.
  *
  * EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
  * direct product thereof will be exported directly or indirectly, into any
  * country prohibited by the United States Export Administration Act and the
  * regulations thereunder, without the required authorization from the U.S.
  * government nor will be used for any purpose prohibited by the same.
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



#define FILECODE PROC_MEM_PS_TN_MPTN3_FILECODE
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
// Common tables of TN platform specific configuration
//

// MR0[WR]
// Format :
// D18F2x22C_dct[1:0][Twr], MR0[WR]
//
CONST PSCFG_MR0WR_ENTRY TNMR0WR[] = {
  {0x10, 0},
  {0x05, 1},
  {0x06, 2},
  {0x07, 3},
  {0x08, 4},
  {0x0A, 5},
  {0x0C, 6},
  {0x0E, 7}
};
CONST PSC_TBL_ENTRY TNMR0WrTblEntry = {
   {PSCFG_MR0WR, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (TNMR0WR) / sizeof (PSCFG_MR0WR_ENTRY),
   (VOID *)&TNMR0WR
};

// MR0[CL]
// Format :
// D18F2x200_dct[1:0][Tcl], MR0[CL][3:1], MR0[CL][0]
//
CONST PSCFG_MR0CL_ENTRY TNMR0CL[] = {
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
CONST PSC_TBL_ENTRY TNMR0CLTblEntry = {
   {PSCFG_MR0CL, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (TNMR0CL) / sizeof (PSCFG_MR0CL_ENTRY),
   (VOID *)&TNMR0CL
};

// ODT pattern
// Format:
//  Dimm0, Dimm1  RdODTCSHigh,   RdODTCSLow,   WrODTCSHigh,   WrODTCSLow
//
STATIC CONST PSCFG_2D_ODTPAT_ENTRY TNOdtPat[] = {
  {NP, DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00040000},
  {NP, DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x08040000},
  {DIMM_SR, NP, 0x00000000, 0x00000000, 0x00000000, 0x00000001},
  {DIMM_DR, NP, 0x00000000, 0x00000000, 0x00000000, 0x00000201},
  {DIMM_SR + DIMM_DR, DIMM_SR + DIMM_DR, 0x00000000, 0x01010404, 0x00000000, 0x09050605}
};
CONST PSC_TBL_ENTRY TNOdtPatTblEnt = {
   {PSCFG_ODT_PAT_2D, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (TNOdtPat) / sizeof (PSCFG_2D_ODTPAT_ENTRY),
   (VOID *)&TNOdtPat
};

//
// CKE tri-state
//
STATIC CONST UINT8 ROMDATA TNDdr3CKETri[] = {0xFF, 0xFF};
CONST PSC_TBL_ENTRY TNDdr3CKETriEnt = {
   {PSCFG_CKETRI, DT_DONT_CARE, NOD_DONT_CARE, {AMD_FAMILY_15_TN, (AMD_F15_OR_ALL | AMD_F15_TN_ALL | 0x0000000000100000ull)  }, PT_DONT_CARE, DDR3_TECHNOLOGY},
   sizeof (TNDdr3CKETri) / sizeof (UINT8),
   (VOID *)&TNDdr3CKETri
};
