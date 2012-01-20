/* $NoKeywords:$ */
/**
 * @file
 *
 * mrnmctdr.c
 *
 * Northbridge DR MCT supporting functions Recovery
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem)
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mrport.h"
#include "mm.h"
#include "mn.h"
#include "mrnhy.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_NB_HY_MRNMCTHY_FILECODE
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
 *   This function sets final values in BUCFG and BUCFG2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemRecNFinalizeMctHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  S_UINT64 SMsr;

  MemPtr = NBPtr->MemPtr;

  // Recommended settings for F2x11C
  MemRecNSetBitFieldNb (NBPtr, BFMctCfgHiReg, 0x2CE00F60);

  LibAmdMsrRead (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  if (!NBPtr->ClToNbFlag) {
    SMsr.lo &= ~((UINT32) 1 << 15);                // ClLinesToNbDis
  }
  LibAmdMsrWrite (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  LibAmdMsrRead (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.hi &= ~((UINT32) 1 << (48 - 32));               // WbEnhWsbDis
  LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets initial values in BUCFG and BUCFG2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemRecNInitializeMctHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  S_UINT64 SMsr;

  MemPtr = NBPtr->MemPtr;

  LibAmdMsrRead (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  if (SMsr.lo & ((UINT32) 1 << 15)) {
    NBPtr->ClToNbFlag = TRUE;
  }
  SMsr.lo |= (UINT32) 1 << 15;                   // ClLinesToNbDis
  LibAmdMsrWrite (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  LibAmdMsrRead (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.hi |= (UINT32) 1 << (48 - 32);              // WbEnhWsbDis
  LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
