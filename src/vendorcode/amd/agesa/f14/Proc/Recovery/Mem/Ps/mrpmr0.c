/* $NoKeywords:$ */
/**
 * @file
 *
 * mpmr0.c
 *
 * A sub-engine which extracts MR0[WR] and MR0[CL] value.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Proc/Recovery/Mem/Ps)
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
/*
 *----------------------------------------------------------------------------
 *                                MODULES USED
 *
 *----------------------------------------------------------------------------
 */



#include "AGESA.h"
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mu.h"
#include "ma.h"
#include "mp.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_PS_MRPMR0_FILECODE


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
 *    A sub-function which extracts MR0[WR] or MR0[CL] value from a input table and store the
 *    value to a specific address.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Succeed in extracting the table value
 *     @return          FALSE - Fail to extract the table value
 *
 */
BOOLEAN
MemPRecGetMR0WrCL (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 p;
  UINT32 Value32;
  UINT8 TableSize;
  PSCFG_TYPE Type;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  PSCFG_MR0CL_ENTRY *TblPtr;
  PSC_TBL_ENTRY **ptr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;
  TblPtr = NULL;
  TableSize = 0;

  // Extract MR0[WR] value, then MR0[CL] value
  for (i = 0; i < 2; i++) {
    if (i == 0) {
      ptr = EntryOfTables->TblEntryOfMR0WR;
      Type = PSCFG_MR0WR;
    } else {
      ptr = EntryOfTables->TblEntryOfMR0CL;
      Type = PSCFG_MR0CL;
    }

    p = 0;
    // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
    while (ptr[p] != NULL) {
      //
      // Determine if this is the expected NB Type
      //
      LogicalCpuid = (ptr[p])->Header.LogicalCpuid;
      PackageType = (ptr[p])->Header.PackageType;
      if (MemPRecIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
        TblPtr = (PSCFG_MR0CL_ENTRY *) ((ptr[p])->TBLPtr);
        TableSize = (ptr[p])->TableSize;
        break;
      }
      p++;
    }

    // Check whether no table entry is found.
    if (ptr[p] == NULL) {
      return FALSE;
    }

    Value32 = (Type == PSCFG_MR0WR) ? NBPtr->GetBitField (NBPtr, BFTwrDDR3) : NBPtr->GetBitField (NBPtr, BFTcl);

    for (j = 0; j < TableSize; j++, TblPtr++) {
      if (Value32 == (UINT32) TblPtr->Timing) {
        if (Type == PSCFG_MR0WR) {
          NBPtr->PsPtr->MR0WR = (UINT8) TblPtr->Value;
          break;
        } else {
          NBPtr->PsPtr->MR0CL31 = (UINT8) TblPtr->Value;
          NBPtr->PsPtr->MR0CL0 = (UINT8) TblPtr->Value1;
          break;
        }
      }
    }
    if (j == TableSize) {
      return FALSE;
    }
  }

  return TRUE;
}
