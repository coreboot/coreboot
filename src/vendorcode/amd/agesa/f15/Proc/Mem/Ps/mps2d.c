/* $NoKeywords:$ */
/**
 * @file
 *
 * mps2d.c
 *
 * A sub-engine determine which configs should use training.
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

#define FILECODE PROC_MEM_PS_MPS___FILECODE


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
MemPGetS__ (
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
 *    A sub-function which determine if training should be run
 *    from a input table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Table values can be extracted per dimm population and ranks type.
 *     @return          FALSE - Table values cannot be extracted per dimm population and ranks type.
 *
 */
BOOLEAN
MemPGetS__ (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{

  UINT8 i;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  UINT32 CurDDRrate;
  UINT8 DDR3Voltage;
  UINT16 RankTypeOfPopulatedDimm;
  UINT16 RankTypeInTable;
  BOOLEAN FoundValue;
  DIMM_TYPE DimmType;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  PSCFG_S___ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;
  UINT16 P__TraingOveride;

  CurrentChannel = NBPtr->ChannelPtr;

  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);

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
  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
  while (EntryOfTables->TblEntryOfS__[i] != NULL) {
    if (((EntryOfTables->TblEntryOfS__[i])->Header.DimmType & DimmType) != 0) {
      if (((EntryOfTables->TblEntryOfS__[i])->Header.NumOfDimm & NOD) != 0) {
        //
        // Determine if this is the expected NB Type
        //
        LogicalCpuid = (EntryOfTables->TblEntryOfS__[i])->Header.LogicalCpuid;
        PackageType = (EntryOfTables->TblEntryOfS__[i])->Header.PackageType;
        if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
          TblPtr = (PSCFG_S___ENTRY *) ((EntryOfTables->TblEntryOfS__[i])->TBLPtr);
          TableSize = (EntryOfTables->TblEntryOfS__[i])->TableSize;
          break;
        }
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfS__[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo training Config table\n");
    return FALSE;
  }
  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));
  DDR3Voltage = (UINT8) (1 << CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage));
  RankTypeOfPopulatedDimm = MemPGetPsRankType (CurrentChannel);
  FoundValue = FALSE;
  for (i = 0; i < TableSize; i++) {
    MemPConstructRankTypeMap ((UINT16) TblPtr->Dimm0, (UINT16) TblPtr->Dimm1, (UINT16) TblPtr->Dimm2, &RankTypeInTable);
    if (TblPtr->DimmPerCh == MaxDimmPerCh) {
      if ((TblPtr->DDRrate & CurDDRrate) != 0) {
        if ((TblPtr->VDDIO & DDR3Voltage) != 0) {
          if ((RankTypeInTable & RankTypeOfPopulatedDimm) == RankTypeOfPopulatedDimm) {
            if (TblPtr->Enable__ == 1) {
              FoundValue = TRUE;
              break;
            }
          }
        }
      }
    }
    TblPtr++;
  }
  P__TraingOveride = MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV____TRAINING);
  if (P__TraingOveride != 0) {
    if (NBPtr->Override__Training) {
      FoundValue = TRUE;
    } else {
      FoundValue = FALSE;
    }
  }
  //
  // If there is no entry, check if overriding training existed. If not, show no entry found.
  //
  if (FoundValue == FALSE || ((P__TraingOveride & INVALID_CONFIG_FLAG) != 0)) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo training config entries\n");
    return FALSE;
  } else {
    return TRUE;
  }
}