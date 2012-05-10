/* $NoKeywords:$ */
/**
 * @file
 *
 * mnmcthy.c
 *
 * Northbridge Hydra MCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/HY)
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

/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnhy.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_HY_MNMCTHY_FILECODE
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
extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets final values in BUCFG and BUCFG2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNFinalizeMctHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  S_UINT64 SMsr;
  UINT16 Speed;
  UINT32 ExtMctCfgLoRegVal;

  MemPtr = NBPtr->MemPtr;
  Speed = NBPtr->DCTPtr->Timings.Speed;
  MemNSetBitFieldNb (NBPtr, BFMctCfgHiReg, (!NBPtr->Ganged) ? 0x2CE00F60 : 0x2CE00F40);

  ExtMctCfgLoRegVal = MemNGetBitFieldNb (NBPtr, BFExtMctCfgLoReg);
  ExtMctCfgLoRegVal |= (NBPtr->Ganged) ? 0x0FC00001 : 0x0FC01001;

  ExtMctCfgLoRegVal &= 0x0FFFFFFF;
  if (Speed == DDR667_FREQUENCY) {
    ExtMctCfgLoRegVal |= 0x40000000;
  } else if (Speed == DDR800_FREQUENCY) {
    ExtMctCfgLoRegVal |= 0x50000000;
  } else if (Speed == DDR1066_FREQUENCY) {
    ExtMctCfgLoRegVal |= 0x60000000;
  } else if (Speed == DDR1333_FREQUENCY) {
    ExtMctCfgLoRegVal |= 0x80000000;
  } else {
    ExtMctCfgLoRegVal |= 0x90000000;
  }
  MemNSetBitFieldNb (NBPtr, BFExtMctCfgLoReg, ExtMctCfgLoRegVal);

  if (NBPtr->Node == BSP_DIE) {
    if (!NBPtr->ClToNbFlag) {
      LibAmdMsrRead (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
      SMsr.lo &= ~((UINT32)1 << 15);                // ClLinesToNbDis
      LibAmdMsrWrite (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    }

    LibAmdMsrRead (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    SMsr.hi &= ~((UINT32)1 << (48 - 32));               // WbEnhWsbDis
    LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  }

  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sets initial values in BUCFG and BUCFG2
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemNInitializeMctHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MEM_DATA_STRUCT *MemPtr;
  S_UINT64 SMsr;

  MemPtr = NBPtr->MemPtr;

  if (NBPtr->Node == BSP_DIE) {
    LibAmdMsrRead (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    if (SMsr.lo & ((UINT32)1 << 15)) {
      NBPtr->ClToNbFlag = 1;
    }
    SMsr.lo |= (UINT32)1 << 15;                   // ClLinesToNbDis
    LibAmdMsrWrite (BU_CFG2, (UINT64 *)&SMsr, &MemPtr->StdHeader);

    LibAmdMsrRead (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    SMsr.hi |= (UINT32)1 << (48 - 32);              // WbEnhWsbDis
    LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
    LibAmdMsrWrite (BU_CFG, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
