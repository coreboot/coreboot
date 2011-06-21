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
 * @e \$Revision: 37655 $ @e \$Date: 2010-09-09 11:15:05 +0800 (Thu, 09 Sep 2010) $
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
#include "mu.h"
#include "ma.h"
#include "mp.h"
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
  UINT16 DimmPerCh:3;
  UINT16 Dimms:3;
  UINT16 SR:3;
  UINT16 DR:3;
  UINT16 QR:4;
} CDNMaxFreq;

typedef struct {
  UINT16 DimmPerCh:3;
  UINT16 Dimms:3;
  UINT16 LR:10;
} CDNLMaxFreq;
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
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  UINT8 TableSize;
  PSCFG_TYPE Type;
  UINT16 CDN;
  UINT16 MaxFreqSupported;
  UINT16 *SpeedArray;
  DIMM_VOLTAGE DDR3Voltage;
  DIMM_VOLTAGE CurrentVoltage;
  DIMM_VOLTAGE VoltageHighestFreq;
  DIMM_TYPE DimmType;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  PSCFG_MAXFREQ_ENTRY *TblPtr;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  Type = PSCFG_MAXFREQ;
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
  //@todo LRDIMM
  //} else if (CurrentChannel->LrDimmPresent) {
  //  DimmType = LRDIMM_TYPE;
  } else {
    DimmType = UDIMM_TYPE;
  }

  i = 0;
  // Obtain table pointer, table size, Logical Cpuid and PSC type according to Dimm, NB and package type.
  while (EntryOfTables->TblEntryOfMaxFreq[i] != NULL) {
    if (((EntryOfTables->TblEntryOfMaxFreq[i])->Header.DimmType & DimmType) != 0) {
      if (((EntryOfTables->TblEntryOfMaxFreq[i])->Header.NumOfDimm & NOD) != 0) {
        //
        // Determine if this is the expected NB Type
        //
        LogicalCpuid = (EntryOfTables->TblEntryOfMaxFreq[i])->Header.LogicalCpuid;
        PackageType = (EntryOfTables->TblEntryOfMaxFreq[i])->Header.PackageType;
        if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
          TblPtr = (PSCFG_MAXFREQ_ENTRY *) ((EntryOfTables->TblEntryOfMaxFreq[i])->TBLPtr);
          TableSize = (EntryOfTables->TblEntryOfMaxFreq[i])->TableSize;
          Type = (EntryOfTables->TblEntryOfMaxFreq[i])->Header.PSCType;
          break;
        }
      }
    }
    i++;
  }

  // Check whether no table entry is found.
  if (EntryOfTables->TblEntryOfMaxFreq[i] == NULL) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\nNo MaxFreq table\n");
    return FALSE;
  }

  MaxFreqSupported = DDR1866_FREQUENCY;
  CDN = 0;
  DDR3Voltage = NBPtr->RefPtr->DDR3Voltage;
  VoltageHighestFreq = DDR3Voltage;

  // Construct the condition value
  ((CDNMaxFreq *)&CDN)->DimmPerCh = MaxDimmPerCh;
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
        ((CDNMaxFreq *)&CDN)->QR += 1;
      }
    }
  } else {
    ((CDNLMaxFreq *)&CDN)->LR = CurrentChannel->Dimms;
  }

  for (i = 0; i < TableSize; i++) {
    if (CDN == ((Type == PSCFG_MAXFREQ) ? TblPtr->MAXFREQ_ENTRY.CDN :
      ((PSCFG_LR_MAXFREQ_ENTRY *)TblPtr)->LR_MAXFREQ_ENTRY.CDN)) {
      if (Type == PSCFG_MAXFREQ) {
        SpeedArray = TblPtr->MAXFREQ_ENTRY.Speed;
      } else {
        SpeedArray = ((PSCFG_LR_MAXFREQ_ENTRY *)TblPtr)->LR_MAXFREQ_ENTRY.Speed;
      }
      if (NBPtr->SharedPtr->VoltageMap != VDDIO_DETERMINED) {
        for (CurrentVoltage = VOLT1_5; CurrentVoltage <= VOLT1_25; CurrentVoltage ++) {
          if (NBPtr->SharedPtr->VoltageMap & (1 << CurrentVoltage)) {
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
      }
      MaxFreqSupported = SpeedArray[DDR3Voltage];
      break;
    }
    TblPtr++;
  }

  if (NBPtr->DCTPtr->Timings.TargetSpeed > MaxFreqSupported) {
    NBPtr->DCTPtr->Timings.TargetSpeed = MaxFreqSupported;
  }
  return TRUE;
}