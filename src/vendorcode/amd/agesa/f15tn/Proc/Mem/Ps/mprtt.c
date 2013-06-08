/* $NoKeywords:$ */
/**
 * @file
 *
 * mprtt.c
 *
 * A sub-engine which extracts RttNom and RttWr (Dram Term and Dynamic Dram Term) value.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
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

#define FILECODE PROC_MEM_PS_MPRTT_FILECODE


/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define _DONT_CARE 0xFF

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
MemPGetRttNomWr (
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
 *    A sub-function which extracts RttNom and RttWr value from a input table and stores extracted
 *    value to a specific address.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Table values can be extracted for all present dimms/ranks
 *     @return          FALSE - Table values cannot be extracted for all present dimms/ranks
 *
 */
BOOLEAN
MemPGetRttNomWr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 MaxDimmPerCh;
  UINT8 MaxDimmSlotPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  UINT32 CurDDRrate;
  UINT8 DDR3Voltage;
  UINT16 RankTypeOfPopulatedDimm;
  UINT16 RankTypeInTable;
  DIMM_TYPE DimmType;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  UINT8 TgtDimmType;
  UINT8 TgtRank;
  UINT8 Chipsel;
  UINT8 PsoCsMaskRtt;
  UINT16 PsoCsMaskRtt16;
  UINT8 NoEntryCsMask;
  PSCFG_RTT_ENTRY *TblPtr;
  PSCFG_RTT_ENTRY *OrgTblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  PsoCsMaskRtt = 0;
  NoEntryCsMask = 0;
  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;

  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  MaxDimmSlotPerCh = MaxDimmPerCh - GetMaxSolderedDownDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration,
                                                                       NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);

  if (CurrentChannel->RegDimmPresent != 0) {
    DimmType = RDIMM_TYPE;
  } else if (CurrentChannel->SODimmPresent != 0) {
    DimmType = SODIMM_TYPE;
  } else if (CurrentChannel->LrDimmPresent != 0) {
    DimmType = LRDIMM_TYPE;
  } else {
    DimmType = UDIMM_TYPE;
  }

  // Check if it is "SODIMM plus soldered-down DRAM" or "Soldered-down DRAM only" configuration,
  // DimmType is changed to 'SODWN_SODIMM_TYPE' if soldered-down DRAM exist
  if (MaxDimmSlotPerCh != MaxDimmPerCh) {
    // SODIMM plus soldered-down DRAM
    DimmType = SODWN_SODIMM_TYPE;
  } else if (FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_SOLDERED_DOWN_SODIMM_TYPE, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID, 0, NULL, NULL) != NULL) {
    // Soldered-down DRAM only
    DimmType = SODWN_SODIMM_TYPE;
    MaxDimmSlotPerCh = 0;
  }
  NOD = (UINT8) (MaxDimmSlotPerCh != 0) ? (1 << (MaxDimmSlotPerCh - 1)) : _DIMM_NONE;

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
  while (EntryOfTables->TblEntryOfDramTerm[i] != NULL) {
    if (((EntryOfTables->TblEntryOfDramTerm[i])->Header.DimmType & DimmType) != 0) {
      if (((EntryOfTables->TblEntryOfDramTerm[i])->Header.NumOfDimm & NOD) != 0) {
        //
        // Determine if this is the expected NB Type
        //
        LogicalCpuid = (EntryOfTables->TblEntryOfDramTerm[i])->Header.LogicalCpuid;
        PackageType = (EntryOfTables->TblEntryOfDramTerm[i])->Header.PackageType;
        if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
          TblPtr = (PSCFG_RTT_ENTRY *) ((EntryOfTables->TblEntryOfDramTerm[i])->TBLPtr);
          TableSize = (EntryOfTables->TblEntryOfDramTerm[i])->TableSize;
          break;
        }
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfDramTerm[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo RTT table\n");
    return FALSE;
  }

  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));
  DDR3Voltage = (UINT8) (1 << CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage));
  RankTypeOfPopulatedDimm = MemPGetPsRankType (CurrentChannel);

  OrgTblPtr = TblPtr;
  for (Chipsel = 0; Chipsel < MAX_CS_PER_CHANNEL; Chipsel++) {
    TblPtr = OrgTblPtr;
    if ((NBPtr->DCTPtr->Timings.CsPresent & (UINT16) (1 << Chipsel)) != 0) {
      if ((CurrentChannel->DimmQrPresent & (UINT8) (1 << (Chipsel >> 1))) != 0) {
        TgtDimmType = DIMM_QR;
        TgtRank = (UINT8) ((Chipsel < 4) ? 1 << (Chipsel & 1) : 4 << (Chipsel & 1));
      } else if ((CurrentChannel->DimmDrPresent & (UINT8) (1 << (Chipsel >> 1))) != 0) {
        TgtDimmType = DIMM_DR;
        TgtRank = (UINT8) 1 << (Chipsel & 1);
      } else {
        TgtDimmType = DIMM_SR;
        TgtRank = (UINT8) 1 << (Chipsel & 1);
      }

      if (DimmType == LRDIMM_TYPE) {
        TgtDimmType = _DONT_CARE;
        TgtRank = _DONT_CARE;
      }

      for (i = 0; i < TableSize; i++) {
        MemPConstructRankTypeMap ((UINT16) TblPtr->Dimm0, (UINT16) TblPtr->Dimm1, (UINT16) TblPtr->Dimm2, &RankTypeInTable);
        if ((TblPtr->DimmPerCh & NOD) != 0) {
          if ((TblPtr->DDRrate & CurDDRrate) != 0) {
            if ((TblPtr->VDDIO & DDR3Voltage) != 0) {
              if ((RankTypeInTable & RankTypeOfPopulatedDimm) == RankTypeOfPopulatedDimm) {
                if (((TblPtr->Dimm & TgtDimmType) != 0) || (TgtDimmType == _DONT_CARE)) {
                  if (((TblPtr->Rank & TgtRank) != 0) || (TgtRank == _DONT_CARE)) {
                    NBPtr->PsPtr->RttNom[Chipsel] = (UINT8) TblPtr->RttNom;
                    NBPtr->PsPtr->RttWr[Chipsel] = (UINT8) TblPtr->RttWr;
                    break;
                  }
                }
              }
            }
          }
        }
        TblPtr++;
      }
      // Record which Cs(s) have no entries. Later on, we will check if there are overriding values for them.
      if ((i == TableSize) && (NBPtr->SharedPtr->VoltageMap == VDDIO_DETERMINED)) {
        NoEntryCsMask |= (UINT8) 1 << Chipsel;
      }
    }
  }

  PsoCsMaskRtt16 = MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_ODT_RTTNOM);
  PsoCsMaskRtt16 &= MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_ODT_RTTWR);
  //
  // Check to see if invalid entry exist ?
  //
  if ((PsoCsMaskRtt16 & INVALID_CONFIG_FLAG) != 0) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nInvalid entry is found\n\n");
    return FALSE;
  }

  //
  // If there are no entries for certain Cs(s), we need to check if overriding values (both RttNom and RttWr) existed for them.
  // Otherwise, return FALSE.
  //
  PsoCsMaskRtt = (UINT8) PsoCsMaskRtt16;
  if (NoEntryCsMask != 0) {
    if ((PsoCsMaskRtt & NoEntryCsMask) != NoEntryCsMask) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nNo Rtt entries\n");
      PutEventLog (AGESA_ERROR, MEM_ERROR_RTT_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
      if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
        ASSERT (FALSE);
      }
      return FALSE;
    }
  }

  return TRUE;
}
