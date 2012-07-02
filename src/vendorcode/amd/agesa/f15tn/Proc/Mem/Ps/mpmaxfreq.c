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
 * @e \$Revision: 63425 $ @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 **/
/*****************************************************************************
*
* Copyright 2008 - 2012 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
*
* AMD is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with AMD.  This header does *NOT* give you permission to use the Materials
* or any rights under AMD's intellectual property.  Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions.  If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY:  The Materials and all other information, identified as
* confidential and provided to you by AMD shall be kept confidential in
* accordance with the terms and conditions of the Software License Agreement.
*
* LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
* PROVIDED TO YOU BY AMD ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
* OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
* IN NO EVENT SHALL AMD OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
* (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
* INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF AMD'S NEGLIGENCE,
* GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
* RELATED INFORMATION PROVIDED TO YOU BY AMD, EVEN IF AMD HAS BEEN ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
* EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
* THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
*
* AMD does not assume any responsibility for any errors which may appear in
* the Materials or any other related information provided to you by AMD, or
* result from use of the Materials or any related information.
*
* You agree that you will not reverse engineer or decompile the Materials.
*
* NO SUPPORT OBLIGATION: AMD is not obligated to furnish, support, or make any
* further information, software, technical information, know-how, or show-how
* available to you.  Additionally, AMD retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
* U.S. GOVERNMENT RESTRICTED RIGHTS: The Materials are provided with
* "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
* subject to the restrictions as set forth in FAR 52.227-14 and
* DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
* Government constitutes acknowledgement of AMD's proprietary rights in them.
*
* EXPORT ASSURANCE:  You agree and certify that neither the Materials, nor any
* direct product thereof will be exported directly or indirectly, into any
* country prohibited by the United States Export Administration Act and the
* regulations thereunder, without the required authorization from the U.S.
* government nor will be used for any purpose prohibited by the same.
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
    IDS_HDT_CONSOLE (MEM_FLOW, "\nDCT %d: No MaxFreq table. This channel will be disabled.\n", NBPtr->Dct);
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
    MaxFreqSupported = SpeedArray[DDR3Voltage];
  }

  if (MaxFreqSupported == UNSUPPORTED_DDR_FREQUENCY) {
    // No entry in the table for current dimm population is found
    IDS_HDT_CONSOLE (MEM_FLOW, "\nDCT %d: No entry is found in the Max Frequency table\n", NBPtr->Dct);
    DisDct = TRUE;
  } else if (MaxFreqSupported != 0) {
    if (NBPtr->DCTPtr->Timings.TargetSpeed > MaxFreqSupported) {
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
