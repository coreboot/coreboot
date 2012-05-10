/* $NoKeywords:$ */
/**
 * @file
 *
 * mp.c
 *
 * Common platform specific configuration.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Ps)
 * @e \$Revision: 49545 $ @e \$Date: 2011-03-25 05:58:58 +0800 (Fri, 25 Mar 2011) $
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

#define FILECODE PROC_MEM_PS_MP_FILECODE


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
STATIC
MemPPSCGen (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
extern MEM_PSC_FLOW_BLOCK* memPlatSpecFlowArray[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This is the default return function of the Platform Specific block. The function always
 *    returns   AGESA_UNSUPPORTED
 *
 *     @param[in,out]   *MemPtr           Pointer to MEM_DATA_STRUCTURE
 *     @param[in]       *ChannelPtr       Pointer to CH_DEF_STRUCT
 *     @param[in]       *PsPtr            Pointer to MEM_PS_BLOCK
 *
 *     @return          AGESA_UNSUPPORTED  AGESA status indicating that default is unsupported
 *
 */

AGESA_STATUS
MemPConstructPsUDef (
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN OUT   CH_DEF_STRUCT *ChannelPtr,
  IN OUT   MEM_PS_BLOCK *PsPtr
  )
{
  return AGESA_UNSUPPORTED;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function will set the DramTerm and DramTermDyn in the structure of a channel.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *     @param[in]       ArraySize        Size of the array of DramTerm
 *     @param[in]       *DramTermPtr     Address the array of DramTerm
 *
 *     @return          TRUE - Find DramTerm and DramTermDyn for corresponding platform and dimm population.
 *     @return          FALSE - Fail to find DramTerm and DramTermDyn for corresponding platform and dimm population.
 *
 */
BOOLEAN
MemPGetDramTerm (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 ArraySize,
  IN       CONST DRAM_TERM_ENTRY *DramTermPtr
  )
{
  UINT8 Dimms;
  UINT8 QR_Dimms;
  UINT8 i;
  Dimms = NBPtr->ChannelPtr->Dimms;
  QR_Dimms = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i ++) {
    if (((NBPtr->ChannelPtr->DimmQrPresent & (UINT16) (1 << i)) != 0) && (i < 2)) {
      QR_Dimms ++;
    }
  }

  for (i = 0; i < ArraySize; i ++) {
    if ((DramTermPtr[i].Speed & ((UINT32) 1 << (NBPtr->DCTPtr->Timings.Speed / 66))) != 0) {
      if ((((UINT8) (1 << (Dimms - 1)) & DramTermPtr[i].Dimms) != 0) || (DramTermPtr[i].Dimms == ANY_NUM)) {
        if (((QR_Dimms == 0) && (DramTermPtr[i].QR_Dimms == NO_DIMM)) ||
            ((QR_Dimms > 0) && (((UINT8) (1 << (QR_Dimms - 1)) & DramTermPtr[i].QR_Dimms) != 0)) ||
            (DramTermPtr[i].QR_Dimms == ANY_NUM)) {
          NBPtr->PsPtr->DramTerm = DramTermPtr[i].DramTerm;
          NBPtr->PsPtr->QR_DramTerm = DramTermPtr[i].QR_DramTerm;
          NBPtr->PsPtr->DynamicDramTerm = DramTermPtr[i].DynamicDramTerm;
          break;
        }
      }
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function gets the highest POR supported speed.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *     @param[in]       FreqLimitSize    Size of the array of Frequency Limit
 *     @param[in]       *FreqLimitPtr     Address the array of Frequency Limit
 *
 *     @return          UINT8 - frequency limit
 *
 */
UINT16
MemPGetPorFreqLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 FreqLimitSize,
  IN       CONST POR_SPEED_LIMIT *FreqLimitPtr
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 DimmTpMatch;
  UINT16 SpeedLimit;
  UINT16 DIMMRankType;
  UINT16 _DIMMRankType;

  SpeedLimit = 0;
  DIMMRankType = MemAGetPsRankType (NBPtr->ChannelPtr);
  for (i = 0; i < FreqLimitSize; i++, FreqLimitPtr++) {
    if (NBPtr->ChannelPtr->Dimms != FreqLimitPtr->Dimms) {
      continue;
    }
    DimmTpMatch = 0;
    _DIMMRankType = DIMMRankType & FreqLimitPtr->DIMMRankType;
    for (j = 0; j < MAX_DIMMS_PER_CHANNEL; j ++) {
      if ((_DIMMRankType & (UINT16) 0x0F << (j << 2)) != 0) {
        DimmTpMatch++;
      }
    }
    if (DimmTpMatch == FreqLimitPtr->Dimms) {
      if (NBPtr->RefPtr->DDR3Voltage == VOLT1_5) {
        SpeedLimit = FreqLimitPtr->SpeedLimit_1_5V;
        break;
      } else if (NBPtr->RefPtr->DDR3Voltage == VOLT1_25) {
        SpeedLimit = FreqLimitPtr->SpeedLimit_1_25V;
        break;
      } else {
        SpeedLimit = FreqLimitPtr->SpeedLimit_1_35V;
        break;
      }
    }
  }

  return SpeedLimit;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function is the default function for getting POR speed limit. When a
 *    package does not need to cap the speed, it should use this function to initialize
 *    the corresponding function pointer.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 */
VOID
MemPGetPORFreqLimitDef (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function gets platform specific configuration such as Max Freq., Slow Mode, Dram Term,
 *    and so on.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - Successfully execute platform specific configuration flow.
 *     @return          FALSE - Fail to execute platform specific configuration flow.
 *
 */
BOOLEAN
MemPPSCFlow (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  BOOLEAN Result;

  Result = TRUE;
  i = 0;
  while (memPlatSpecFlowArray[i] != NULL) {
    if ((memPlatSpecFlowArray[i])->DramTerm (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
      if ((memPlatSpecFlowArray[i])->ODTPattern (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
        if ((memPlatSpecFlowArray[i])->SAO (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
          if ((memPlatSpecFlowArray[i])->MR0WrCL (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
            if ((memPlatSpecFlowArray[i])->RC2IBT (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
              if ((memPlatSpecFlowArray[i])->RC10OpSpeed (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
                if ((memPlatSpecFlowArray[i])->LRIBT (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
                  if ((memPlatSpecFlowArray[i])->LRNPR (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
                    if ((memPlatSpecFlowArray[i])->LRNLR (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
                      if (MemPPSCGen (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
                        break;
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
    i++;
  }

  IDS_SKIP_HOOK (IDS_ENFORCE_PLAT_TABLES, NBPtr, &(NBPtr->MemPtr->StdHeader)) {
    if (memPlatSpecFlowArray[i] == NULL) {
      Result = FALSE;
    }
  }
  return Result;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *     This function constructs the rank type map of Dimm0, Dimm1, Dimm2. Also it counts the number
 *     of dimm in the table.
 *
 *     @param[in]       Dimm0       Rank type of Dimm0
 *     @param[in]       Dimm1       Rank type of Dimm1
 *     @param[in]       Dimm2       Rank type of Dimm2
 *     @param[in, out]       *RankTypeInTable       Pointer to RankTypeInTable variable
 *
 *
 */
VOID
MemPConstructRankTypeMap (
  IN       UINT16 Dimm0,
  IN       UINT16 Dimm1,
  IN       UINT16 Dimm2,
  IN OUT   UINT16 *RankTypeInTable
  )
{
  UINT8 i;
  UINT16 RT;
  UINT8 BitShift;

  *RankTypeInTable = 0;
  RT = 0;
  BitShift = 0;

  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    switch (i) {
    case 0:
      RT = (Dimm0 == 0) ? NP : Dimm0;
      BitShift = 0;
      break;
    case 1:
      RT = (Dimm1 == 0) ? NP : Dimm1;
      BitShift = 4;
      break;
    case 2:
      RT = (Dimm2 == 0) ? NP : Dimm2;
      BitShift = 8;
      break;
    default:
      // dimm3 is not used, fills nibble3 with "NP"
      RT = NP;
      BitShift = 12;
    }
    *RankTypeInTable |= RT << BitShift;
  }
}

/*-----------------------------------------------------------------------------*/
/**
 *     MemPIsIdSupported
 *      This function matches the CPU_LOGICAL_ID and PackageType with certain criteria to
 *      determine if it is supported by this NB type.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       LogicalId - CPU_LOGICAL_ID
 *     @param[in]       PackageType - Package Type
 *
 *     @return          TRUE -  NB type is matched !
 *     @return          FALSE - NB type is not matched !
 *
 */
BOOLEAN
MemPIsIdSupported (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID LogicalId,
  IN       UINT8 PackageType
  )
{
  CPUID_DATA CpuId;
  UINT8 PkgType;

  LibAmdCpuidRead (AMD_CPUID_FMF, &CpuId, &(NBPtr->MemPtr->StdHeader));
  PkgType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28

  if (((NBPtr->MCTPtr->LogicalCpuid.Family & LogicalId.Family) != 0)
      && ((NBPtr->MCTPtr->LogicalCpuid.Revision & LogicalId.Revision) != 0)) {
    if ((PackageType == PT_DONT_CARE) || (PackageType == PkgType)) {
      return TRUE;
    }
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function returns the rank type map of a channel.
 *
 *     @param[in]       *CurrentChannel       Pointer to CH_DEF_STRUCT
 *
 *     @return          UINT16 - The map of rank type.
 *
 */
UINT16
MemPGetPsRankType (
  IN       CH_DEF_STRUCT *CurrentChannel
  )
{
  UINT8 i;
  UINT16 DIMMRankType;

  DIMMRankType = 0;
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if (CurrentChannel->MCTPtr->Status[SbLrdimms]) {
      // For LrDimm, we construct the map according to Dimm present bits rather than rank type bits
      if ((CurrentChannel->LrDimmPresent & (UINT8) 1 << i) != 0) {
        DIMMRankType |= (UINT16) DIMM_LR << (i << 2);
      } else {
        DIMMRankType |= (UINT16) NP << (i << 2);
      }
    } else {
      if ((CurrentChannel->DimmQrPresent & (UINT8) 1 << i) != 0) {
        if (i < 2) {
          DIMMRankType |= (UINT16) DIMM_QR << (i << 2);
        }
      } else if ((CurrentChannel->DimmDrPresent & (UINT8) 1 << i) != 0) {
        DIMMRankType |= (UINT16) DIMM_DR << (i << 2);
      } else if ((CurrentChannel->DimmSRPresent & (UINT8) 1 << i) != 0) {
        DIMMRankType |= (UINT16) DIMM_SR << (i << 2);
      } else {
        DIMMRankType |= (UINT16) NP << (i << 2);
      }
    }
  }

  return DIMMRankType;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function performs the action for the rest of platform specific configuration such as
 *    tri-state stuff
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *EntryOfTables       Pointer to MEM_PSC_TABLE_BLOCK
 *
 *     @return          TRUE - No error occurred.
 *     @return          FALSE - Error occurred.
 *
 */
BOOLEAN
STATIC
MemPPSCGen (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_PSC_TABLE_BLOCK *EntryOfTables
  )
{
  UINT8 i;
  PSCFG_TYPE PSCType;
  DIMM_TYPE DimmType;
  UINT8 MaxDimmPerCh;
  UINT8 NOD;
  CPU_LOGICAL_ID LogicalCpuid;
  UINT8 PackageType;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  PackageType = 0;
  LogicalCpuid.Family = (UINT64) AMD_FAMILY_UNKNOWN;
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  NOD = (UINT8) 1 << (MaxDimmPerCh - 1);

  if (CurrentChannel->RegDimmPresent != 0) {
    DimmType = RDIMM_TYPE;
  } else if (CurrentChannel->SODimmPresent != 0) {
    DimmType = SODIMM_TYPE;
  } else if (CurrentChannel->LrDimmPresent != 0) {
    DimmType = LRDIMM_TYPE;
  } else {
    DimmType = UDIMM_TYPE;
  }

  for (PSCType = PSCFG_GEN_START + 1; PSCType < PSCFG_GEN_END; PSCType++) {
    i = 0;
    while (EntryOfTables->TblEntryOfGen[i] != NULL) {
      if ((EntryOfTables->TblEntryOfGen[i])->Header.PSCType == PSCType) {
        if (((EntryOfTables->TblEntryOfGen[i])->Header.DimmType & DimmType) != 0) {
          if (((EntryOfTables->TblEntryOfGen[i])->Header.NumOfDimm & NOD) != 0) {
            //
            // Determine if this is the expected NB Type
            //
            LogicalCpuid = (EntryOfTables->TblEntryOfGen[i])->Header.LogicalCpuid;
            PackageType = (EntryOfTables->TblEntryOfGen[i])->Header.PackageType;
            if (MemPIsIdSupported (NBPtr, LogicalCpuid, PackageType)) {
              break;
            }
          }
        }
      }
      i++;
    }

    // Check whether no table entry is found.
    if (EntryOfTables->TblEntryOfGen[i] == NULL) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nNo %s Table\n", (PSCType == PSCFG_CLKDIS) ? "ClkDis" : ((PSCType == PSCFG_CKETRI) ? "CkeTri" : ((PSCType == PSCFG_ODTTRI) ? "OdtTri" : "CsTri")));
      return FALSE;
    }

    // Perform the action for specific PSCType.
    if (PSCType == PSCFG_CLKDIS) {
      CurrentChannel->MemClkDisMap = (UINT8 *) (EntryOfTables->TblEntryOfGen[i])->TBLPtr;
    } else if (PSCType == PSCFG_CKETRI) {
      CurrentChannel->CKETriMap = (UINT8 *) (EntryOfTables->TblEntryOfGen[i])->TBLPtr;
    } else if (PSCType == PSCFG_ODTTRI) {
      CurrentChannel->ODTTriMap = (UINT8 *) (EntryOfTables->TblEntryOfGen[i])->TBLPtr;
    } else if (PSCType == PSCFG_CSTRI) {
      CurrentChannel->ChipSelTriMap = (UINT8 *) (EntryOfTables->TblEntryOfGen[i])->TBLPtr;
    }
  }

  CurrentChannel->DctEccDqsLike = 0x0403;
  CurrentChannel->DctEccDqsScale = 0x70;

  return TRUE;
}

