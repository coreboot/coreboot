/* $NoKeywords:$ */
/**
 * @file
 *
 * mpseeds.c
 *
 * A sub-engine extracts WL and HW RxEn seeds from PSCFG tables.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 45233 $ @e \$Date: 2011-01-13 21:58:29 -0600 (Thu, 13 Jan 2011) $
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
#include "AdvancedApi.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mu.h"
#include "ma.h"
#include "mp.h"
#include "merrhdl.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_MPSEEDS_FILECODE


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
MemPGetTrainingSeeds (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *    A sub-function extracts WL and HW RxEn seeds from PSCFG tables
 *    from a input table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          NBPtr->PsPtr->WLSeedVal
 *     @return          NBPtr->PsPtr->HWRxENSeedVal
 *
 */
BOOLEAN
MemPGetTrainingSeeds (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{

  UINT8 i;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  DIMM_TYPE DimmType;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  UINT8 Seedloop;
  UINT8 CH;
  PSC_TBL_ENTRY **TblEntryPtr;
  PSCFG_SEED_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;
  TblEntryPtr = NULL;
  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);
  CH = 1 << (CurrentChannel->ChannelID);

  if (CurrentChannel->RegDimmPresent != 0) {
    DimmType = RDIMM_TYPE;
  } else if (CurrentChannel->SODimmPresent != 0) {
    DimmType = SODIMM_TYPE;
    if (FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_SOLDERED_DOWN_SODIMM_TYPE, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID, 0, NULL, NULL) != NULL) {
      DimmType = SODWN_SODIMM_TYPE;
    }
  } else if (CurrentChannel->LrDimmPresent != 0) {
    DimmType = LRDIMM_TYPE;
  } else {
    DimmType = UDIMM_TYPE;
  }

  // Get seed value of WL, then HW RxEn
  for (Seedloop = 0; Seedloop < 2; Seedloop++) {
    TblEntryPtr = (Seedloop == 0) ? EntryOfTables->TblEntryOfWLSeed : EntryOfTables->TblEntryOfHWRxENSeed;

    i = 0;
    // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
    while (TblEntryPtr[i] != NULL) {
      if (((TblEntryPtr[i])->Header.DimmType & DimmType) != 0) {
        //
        // Determine if this is the expected NB Type
        //
        LogicalCpuid = (TblEntryPtr[i])->Header.LogicalCpuid;
        PackageType = (TblEntryPtr[i])->Header.PackageType;
        if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
          TblPtr = (PSCFG_SEED_ENTRY *) ((TblEntryPtr[i])->TBLPtr);
          TableSize = (TblEntryPtr[i])->TableSize;
          break;
        }
      }
      i++;
    }

    // Check whether no table entry is found.
    if (TblEntryPtr[i] == NULL) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nNo %s training seeds Config table\n", (Seedloop == 0) ? "WL" : "HW RxEn");
      return FALSE;
    }

    for (i = 0; i < TableSize; i++) {
      if ((TblPtr->DimmPerCh & NOD) != 0) {
        if ((TblPtr->Channel & CH) != 0) {
          if (Seedloop == 0) {
            NBPtr->PsPtr->WLSeedVal = (UINT8) TblPtr->SeedVal;
          } else {
            NBPtr->PsPtr->HWRxENSeedVal = TblPtr->SeedVal;
          }
          break;
        }
      }
      TblPtr++;
    }

    if (i == TableSize) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nNo %s seed entries\n\n", (Seedloop == 0) ? "WL" : "HW RxEn");
      PutEventLog (AGESA_ERROR, MEM_ERROR_TRAINING_SEED_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
      if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
        ASSERT (FALSE);
      }
      return FALSE;
    }
  }

  return TRUE;
}