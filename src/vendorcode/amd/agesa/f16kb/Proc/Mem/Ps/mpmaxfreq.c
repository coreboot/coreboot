/* $NoKeywords:$ */
/**
 * @file
 *
 * mpmaxfreq.c
 *
 * A sub-engine which extracts max. frequency limit value.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
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
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_MEM_PS_MPMAXFREQ_FILECODE


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
typedef struct {
  UINT16 Dimms:4;
  UINT16 SR:4;
  UINT16 DR:4;
  UINT16 QR:4;
} CDNMaxFreq;

typedef struct {
  UINT16 Dimms:4;
  UINT16 LR:12;
} CDNLMaxFreq;
/*----------------------------------------------------------------------------
 *                        PROTOTYPES OF LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
BOOLEAN
MemPGetMaxFreqSupported (
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
 *    A sub-function which extracts the value of max frequency supported from a input table and
 *    compares it with DCTPtr->Timings.TargetSpeed
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables     - Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - Succeed in extracting the table value
 *     @return          FALSE - Fail to extract the table value
 *
 */
BOOLEAN
MemPGetMaxFreqSupported (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  UINT8 MaxDimmSlotPerCh;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  PSCFG_TYPE Type;
  UINT16 CDN;
  UINT16 MaxFreqSupported;
  UINT16 *SpeedArray;
  UINT8 DDR3Voltage;
  UINT8 CurrentVoltage;
  DIMM_TYPE DimmType;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  BOOLEAN DisDct;
  UINT8 PsoMaskMaxFreq;
  UINT16 PsoMaskMaxFreq16;
  UINT8 NumDimmSlotInTable;
  UINT16 DimmPopInTable;
  PSCFG_MAXFREQ_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;
  PSC_TBL_ENTRY **TblEntryOfMaxFreq;

  CurrentChannel = NBPtr->ChannelPtr;

  DisDct = FALSE;
  Type = PSCFG_MAXFREQ;
  TblPtr = NULL;
  TableSize = 0;
  PackageType = 0;
  NumDimmSlotInTable = 0;
  DimmPopInTable = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
  SpeedArray = NULL;

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

  TblEntryOfMaxFreq = EntryOfTables->TblEntryOfMaxFreq;
  IDS_OPTION_HOOK (IDS_GET_STRETCH_FREQUENCY_LIMIT, &TblEntryOfMaxFreq, &NBPtr->MemPtr->StdHeader);

  NBPtr->FamilySpecificHook[RelocatePscTblEntryByMotherBoardLayer] (NBPtr, (VOID *) &TblEntryOfMaxFreq);

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
  while (TblEntryOfMaxFreq[i] != NULL) {
    if (((TblEntryOfMaxFreq[i])->Header.DimmType & DimmType) != 0) {
      if (((TblEntryOfMaxFreq[i])->Header.NumOfDimm & NOD) != 0) {
        //
        // Determine if this is the expected NB Type
        //
        LogicalCpuid = (TblEntryOfMaxFreq[i])->Header.LogicalCpuid;
        PackageType = (TblEntryOfMaxFreq[i])->Header.PackageType;
        if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
          TblPtr = (PSCFG_MAXFREQ_ENTRY *) ((TblEntryOfMaxFreq[i])->TBLPtr);
          TableSize = (TblEntryOfMaxFreq[i])->TableSize;
          Type = (TblEntryOfMaxFreq[i])->Header.PSCType;
          break;
        }
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (TblEntryOfMaxFreq[i] == NULL) {
    return FALSE;
  }

  MaxFreqSupported = UNSUPPORTED_DDR_FREQUENCY;
  CDN = 0;
  DDR3Voltage = (UINT8) CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage);

  // Construct the condition value
  ((CDNMaxFreq *)&CDN)->Dimms = CurrentChannel->Dimms;
  if (Type == PSCFG_MAXFREQ) {
    for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
      if ((CurrentChannel->DimmSRPresent & (UINT8) (1 << i)) != 0) {
        ((CDNMaxFreq *)&CDN)->SR += 1;
      }
      if ((CurrentChannel->DimmDrPresent & (UINT16) (1 << i)) != 0) {
        ((CDNMaxFreq *)&CDN)->DR += 1;
      }
      if ((CurrentChannel->DimmQrPresent & (UINT16) (1 << i)) != 0) {
        if (i < 2) {
          ((CDNMaxFreq *)&CDN)->QR += 1;
        }
      }
    }
  } else {
    ((CDNLMaxFreq *)&CDN)->LR = CurrentChannel->Dimms;
  }

  for (i = 0; i < TableSize; i++) {
    NumDimmSlotInTable = TblPtr->MAXFREQ_ENTRY.DimmSlotPerCh;
    DimmPopInTable = (Type == PSCFG_MAXFREQ) ? TblPtr->MAXFREQ_ENTRY.CDN : ((PSCFG_LR_MAXFREQ_ENTRY *)TblPtr)->LR_MAXFREQ_ENTRY.CDN;
    if (((NumDimmSlotInTable & NOD) != 0) && (CDN == DimmPopInTable)) {
      if (Type == PSCFG_MAXFREQ) {
        SpeedArray = TblPtr->MAXFREQ_ENTRY.Speed;
      } else {
        SpeedArray = ((PSCFG_LR_MAXFREQ_ENTRY *)TblPtr)->LR_MAXFREQ_ENTRY.Speed;
      }
      break;
    }
    TblPtr++;
  }

  PsoMaskMaxFreq16 = MemPProceedTblDrvOverride (NBPtr, NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_TBLDRV_SPEEDLIMIT);
  if ((PsoMaskMaxFreq16 & INVALID_CONFIG_FLAG) == 0) {
    PsoMaskMaxFreq = (UINT8) PsoMaskMaxFreq16;
    if (PsoMaskMaxFreq != 0) {
      SpeedArray = NBPtr->PsPtr->SpeedLimit;
    }
  } else {
    SpeedArray = NULL;
  }

  if (SpeedArray != NULL) {
    if (NBPtr->SharedPtr->VoltageMap != VDDIO_DETERMINED) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nCheck speed supported for each VDDIO for Node%d DCT%d: ", NBPtr->Node, NBPtr->Dct);
      for (CurrentVoltage = VOLT1_5_ENCODED_VAL; CurrentVoltage <= VOLT1_25_ENCODED_VAL; CurrentVoltage ++) {
        if (NBPtr->SharedPtr->VoltageMap & (1 << CurrentVoltage)) {
          IDS_HDT_CONSOLE (MEM_FLOW, "%s -> %dMHz ", (CurrentVoltage == VOLT1_5_ENCODED_VAL) ? "1.5V" : ((CurrentVoltage == VOLT1_35_ENCODED_VAL) ? "1.35V" : "1.25V"), SpeedArray[CurrentVoltage]);
          if (NBPtr->DCTPtr->Timings.TargetSpeed > SpeedArray[CurrentVoltage]) {
            MaxFreqSupported = SpeedArray[CurrentVoltage];
          } else {
            MaxFreqSupported = NBPtr->DCTPtr->Timings.TargetSpeed;
          }
          if (NBPtr->MaxFreqVDDIO[CurrentVoltage] > MaxFreqSupported) {
            NBPtr->MaxFreqVDDIO[CurrentVoltage] = MaxFreqSupported;
          }
        } else {
          NBPtr->MaxFreqVDDIO[CurrentVoltage] = 0;
        }
      }
      IDS_HDT_CONSOLE (MEM_FLOW, "\n");
    }
    ASSERT (DDR3Voltage <= VOLT1_25_ENCODED_VAL);
    for (CurrentVoltage = DDR3Voltage; CurrentVoltage >= VOLT1_5_ENCODED_VAL; CurrentVoltage --) {
      if (NBPtr->SharedPtr->VoltageMap & (1 << CurrentVoltage)) {
        MaxFreqSupported = SpeedArray[CurrentVoltage];
        if (MaxFreqSupported != UNSUPPORTED_DDR_FREQUENCY) {
          NBPtr->RefPtr->DDR3Voltage = CONVERT_ENCODED_TO_VDDIO (CurrentVoltage);
          IDS_HDT_CONSOLE (MEM_FLOW, "\nVDDIO leveraged to %s\n", (CurrentVoltage == VOLT1_5_ENCODED_VAL) ? "1.5V" : ((CurrentVoltage == VOLT1_35_ENCODED_VAL) ? "1.35V" : "1.25V"));
          break;
        }
      }
    }
  }

  if (MaxFreqSupported == UNSUPPORTED_DDR_FREQUENCY) {
    // No entry in the table for current dimm population is found
    IDS_HDT_CONSOLE (MEM_FLOW, "\nDCT %d: No entry is found in the Max Frequency table\n", NBPtr->Dct);
    DisDct = TRUE;
  } else if (MaxFreqSupported != 0) {
    if ((NBPtr->DCTPtr->Timings.TargetSpeed > MaxFreqSupported) && !NBPtr->IsSupported[AMPIsEnabled]) {
      NBPtr->DCTPtr->Timings.TargetSpeed = MaxFreqSupported;
    }
  } else if (NBPtr->SharedPtr->VoltageMap == VDDIO_DETERMINED) {
    // Dimm population is not supported at current voltage
    // Also if there is no performance optimization, disable the DCT
    DisDct = TRUE;
  }

  if (DisDct) {
    NBPtr->DCTPtr->Timings.DimmExclude |= NBPtr->DCTPtr->Timings.DctDimmValid;
    PutEventLog (AGESA_ERROR, MEM_ERROR_UNSUPPORTED_DIMM_CONFIG, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
    SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
    // Change target speed to highest value so it won't affect other channels when leveling frequency across the node.
    NBPtr->DCTPtr->Timings.TargetSpeed = UNSUPPORTED_DDR_FREQUENCY;
  }

  return TRUE;
}
