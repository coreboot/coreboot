/* $NoKeywords:$ */
/**
 * @file
 *
 * mprc2ibt.c
 *
 * A sub-engine which extracts RC2[IBT] value for RDIMM configuration.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 52114 $ @e \$Date: 2011-05-02 13:21:20 -0600 (Mon, 02 May 2011) $
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
#define FILECODE PROC_MEM_PS_MPRC2IBT_FILECODE


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
MemPGetRC2IBT (
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
 *    A sub-function which extracts RC2[IBT] value from a input table and stores extracted
 *    value to a specific address.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables       Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Table values can be extracted for all present dimms/ranks
 *     @return          FALSE - Table values cannot be extracted for all present dimms/ranks
 *
 */
BOOLEAN
MemPGetRC2IBT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 DimmIndex;
  UINT8 TableSize;
  UINT32 CurDDRrate;
  UINT8 DDR3Voltage;
  UINT16 RankTypeOfPopulatedDimm;
  UINT16 RankTypeInTable;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  UINT8 TgtDimmType;
  UINT8 NumOfReg;
  UINT8 PsoDimmMaskRc2Ibt;
  UINT8 NoEntryDimmMask;
  PSCFG_MR2IBT_ENTRY *TblPtr;
  PSCFG_MR2IBT_ENTRY *OrgTblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  if (CurrentChannel->RegDimmPresent == 0) {
    return TRUE;
  }

  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  NoEntryDimmMask = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to NB type and package type.
  while (EntryOfTables->TblEntryOfRC2IBT[i] != NULL) {
    if (((EntryOfTables->TblEntryOfRC2IBT[i])->Header.NumOfDimm & NOD) != 0) {
      LogicalCpuid = (EntryOfTables->TblEntryOfRC2IBT[i])->Header.LogicalCpuid;
      PackageType = (EntryOfTables->TblEntryOfRC2IBT[i])->Header.PackageType;
      //
      // Determine if this is the expected NB Type
      //
      if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
        TblPtr = (PSCFG_MR2IBT_ENTRY *) ((EntryOfTables->TblEntryOfRC2IBT[i])->TBLPtr);
        TableSize = (EntryOfTables->TblEntryOfRC2IBT[i])->TableSize;
        break;
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfRC2IBT[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo RC2 IBT table\n");
    return FALSE;
  }

  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));
  DDR3Voltage = (UINT8) (1 << CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage));
  RankTypeOfPopulatedDimm = MemPGetPsRankType (CurrentChannel);

  OrgTblPtr = TblPtr;
  for (DimmIndex = 0; DimmIndex < MAX_DIMMS_PER_CHANNEL; DimmIndex++) {
    TblPtr = OrgTblPtr;
    NumOfReg = NBPtr->PsPtr->NumOfReg[DimmIndex];
    if ((CurrentChannel->ChDimmValid & (UINT8) (1 << DimmIndex)) != 0) {
      if ((CurrentChannel->DimmQrPresent & (UINT8) (1 << DimmIndex)) != 0) {
        TgtDimmType = DIMM_QR;
      } else if ((CurrentChannel->DimmDrPresent & (UINT8) (1 << DimmIndex)) != 0) {
        TgtDimmType = DIMM_DR;
      } else {
        TgtDimmType = DIMM_SR;
      }

      for (i = 0; i < TableSize; i++) {
        MemPConstructRankTypeMap ((UINT16) TblPtr->Dimm0, (UINT16) TblPtr->Dimm1, (UINT16) TblPtr->Dimm2, &RankTypeInTable);
        if (TblPtr->DimmPerCh == MaxDimmPerCh) {
          if ((TblPtr->DDRrate & CurDDRrate) != 0) {
            if ((TblPtr->VDDIO & DDR3Voltage) != 0) {
              if ((RankTypeInTable & RankTypeOfPopulatedDimm) == RankTypeOfPopulatedDimm) {
                if ((TblPtr->Dimm & TgtDimmType) != 0) {
                  // If TblPtr->NumOfReg == 0x0F, that means the condition will be TRUE regardless of NumRegisters in DIMM
                  if ((TblPtr->NumOfReg == 0xF) || (TblPtr->NumOfReg == NumOfReg)) {
                    CurrentChannel->CtrlWrd02[DimmIndex] = (UINT8) ((TblPtr->IBT & 0x1) << 2);
                    CurrentChannel->CtrlWrd08[DimmIndex] = (UINT8) ((TblPtr->IBT & 0xE) >> 1);
                    break;
                  }
                }
              }
            }
          }
        }
        TblPtr++;
      }

      if (i == TableSize) {
        NoEntryDimmMask |= (UINT8) 1 << DimmIndex;
      }
    }
  }

  //
  // If there are no entries for certain Dimm(s), check if overriding value existed for them. If not, return FALSE.
  //
  PsoDimmMaskRc2Ibt = (UINT8) MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_RC2_IBT);
  if (NoEntryDimmMask != 0) {
    if ((NoEntryDimmMask & PsoDimmMaskRc2Ibt) != NoEntryDimmMask) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nNo RC2 IBT entries\n");
      PutEventLog (AGESA_ERROR, MEM_ERROR_RC2_IBT_NOT_FOUND, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
      if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
        ASSERT (FALSE);
      }
      return FALSE;
    }
  }

  return TRUE;
}