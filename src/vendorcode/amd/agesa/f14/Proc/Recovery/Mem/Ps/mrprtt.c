/* $NoKeywords:$ */
/**
 * @file
 *
 * mrprtt.c
 *
 * A sub-engine which extracts RttNom and RttWr (Dram Term and Dynamic Dram Term) value.
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
#include "cpuFamRegisters.h"
#include "cpuRegisters.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "mru.h"
#include "ma.h"
#include "mp.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_RECOVERY_MEM_PS_MRPRTT_FILECODE


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
MemPRecGetRttNomWr (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  UINT8 DDR3Voltage;
  UINT16 RankTypeOfPopulatedDimm;
  UINT16 RankTypeInTable;
  DIMM_TYPE DimmType;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  UINT8 TgtDimmType;
  UINT8 TgtRank;
  UINT8 Chipsel;
  PSCFG_RTT_ENTRY *TblPtr;
  PSCFG_RTT_ENTRY *OrgTblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = RecGetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);

  if (CurrentChannel->RegDimmPresent != 0) {
    DimmType = RDIMM_TYPE;
  } else if (CurrentChannel->SODimmPresent != 0) {
    DimmType = SODIMM_TYPE;
  //@todo LRDIMM
  //} else if (CurrentChannel->LrDimmPresent) {
  //  DimmType = LRDIMM_TYPE;
  } else {
    DimmType = UDIMM_TYPE;
  }

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
        if (MemPRecIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
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
    return FALSE;
  }

  DDR3Voltage = (UINT8) (1 << (NBPtr->RefPtr->DDR3Voltage));
  RankTypeOfPopulatedDimm = MemPRecGetPsRankType (CurrentChannel);  //@todo - LRDIMM ?

  OrgTblPtr = TblPtr;
  for (Chipsel = 0; Chipsel < MAX_CS_PER_CHANNEL; Chipsel++) {
    TblPtr = OrgTblPtr;
    if ((NBPtr->DCTPtr->Timings.CsEnabled & (UINT16) (1 << Chipsel)) != 0) {
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

      for (i = 0; i < TableSize; i++) {
        MemPRecConstructRankTypeMap ((UINT16) TblPtr->Dimm0, (UINT16) TblPtr->Dimm1, (UINT16) TblPtr->Dimm2, &RankTypeInTable);
        if (TblPtr->DimmPerCh == MaxDimmPerCh) {
          if ((TblPtr->VDDIO & DDR3Voltage) != 0) {
            if ((RankTypeInTable & RankTypeOfPopulatedDimm) == RankTypeOfPopulatedDimm) {
              if ((TblPtr->Dimm & TgtDimmType) != 0) {
                if ((TblPtr->Rank & TgtRank) != 0) {
                  NBPtr->PsPtr->RttNom[Chipsel] = (UINT8) TblPtr->RttNom;
                  NBPtr->PsPtr->RttWr[Chipsel] = (UINT8) TblPtr->RttWr;
                  break;
                }
              }
            }
          }
        }
        TblPtr++;
      }
      if (i == TableSize) {
        return FALSE;
      }
    }
  }

  return TRUE;
}
