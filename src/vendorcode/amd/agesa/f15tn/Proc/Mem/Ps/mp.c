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
#include "merrhdl.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_PS_MP_FILECODE


/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define PSO_TYPE    0
#define PSO_LENGTH  1
#define PSO_DATA    2

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

BOOLEAN
STATIC
MemPCheckTblDrvOverrideConfig (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

BOOLEAN
STATIC
MemPCheckTblDrvOverrideConfigSpeedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

VOID
STATIC
MemPTblDrvOverrideSpeedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

UINT8
STATIC
MemPTblDrvOverrideODT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

VOID
STATIC
MemPTblDrvOverrideODTPattern (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

UINT8
STATIC
MemPTblDrvOverrideRC2IBT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer,
  IN       UINT8 NumOfReg
  );

BOOLEAN
STATIC
MemPTblDrvOverrideMR0WR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

BOOLEAN
STATIC
MemPTblDrvOverrideMR0CL (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  );

BOOLEAN
STATIC
MemPTblDrvOverrideMR10OpSpeed (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
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
 *      This function gets the seed value of WL and HW RxEn pass 1 training.
 *
 *     @param[in,out]   *NBPtr           Pointer to MEM_NB_BLOCK
 *
 *     @return          TRUE - Entries are found
 *     @return          FALSE - Entries are not found
 *
 */
BOOLEAN
MemPGetPSCPass1Seed (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 i;
  UINT8 Dct;

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    i = 0;
    while (memPlatSpecFlowArray[i] != NULL) {
      if (!(memPlatSpecFlowArray[i])->TrainingSeedVal (NBPtr, (memPlatSpecFlowArray[i])->EntryOfTables)) {
        return FALSE;
      }
      i++;
    }
  }

  return TRUE;
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
  UINT32 EventInfo;

  CurrentChannel = NBPtr->ChannelPtr;

  PackageType = 0;
  LogicalCpuid.Family = AMD_FAMILY_UNKNOWN;
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
      EventInfo = (PSCType == PSCFG_CLKDIS) ? MEM_ERROR_CLK_DIS_MAP_NOT_FOUND : ((PSCType == PSCFG_CKETRI) ? MEM_ERROR_CKE_TRI_MAP_NOT_FOUND : ((PSCType == PSCFG_ODTTRI) ? MEM_ERROR_ODT_TRI_MAP_NOT_FOUND : MEM_ERROR_CS_TRI_MAP_NOT_FOUND));
      PutEventLog (AGESA_ERROR, EventInfo, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      SetMemError (AGESA_ERROR, NBPtr->MCTPtr);
      if (!NBPtr->MemPtr->ErrorHandling (NBPtr->MCTPtr, NBPtr->Dct, EXCLUDE_ALL_CHIPSEL, &NBPtr->MemPtr->StdHeader)) {
        ASSERT (FALSE);
      }
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


 /* -----------------------------------------------------------------------------*/
/**
 *
 *    This function proceeds Table Driven Overriding.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in] PlatformMemoryConfiguration - Pointer to Platform config table
 *     @param[in] ProceededPSOType - Proceeded PSO type
 *
 *     @return          bit0 ~ bit7 - Overriding CS or DIMM map.
 *     @return          bit15         - Invalid entry found if set.
 *
 */
UINT16
MemPProceedTblDrvOverride (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       PSO_TABLE *PlatformMemoryConfiguration,
  IN       UINT8 ProceededPSOType
  )
{
  UINT8 *Buffer;
  UINT8 *PSOStartPtr;
  UINT8 NumOfReg;
  UINT8 RetVal;
  UINT16 RetVal16;
  BOOLEAN ConfigMatched;
  BOOLEAN FirstGoThrough;
  BOOLEAN FindNewConfig;
  BOOLEAN InvertRetVal;
  BOOLEAN InvalidConfigDetected;


  ASSERT (PlatformMemoryConfiguration != NULL);
  ASSERT ((ProceededPSOType >= PSO_TBLDRV_START) && (ProceededPSOType <= PSO_TBLDRV_END));

  NumOfReg = 0;
  RetVal = 0;
  RetVal16 = 0;
  FirstGoThrough = TRUE;
  InvertRetVal = FALSE;
  InvalidConfigDetected = FALSE;
  //
  //   << P E R S P E C T I V E >>
  //
  //   PlatformMemoryConfiguration [] = {
  //     . . . . . . . . . . . . . . . . . . .
  //     . . . . . . . . . . . . . . . . . . .
  //    TBLDRV_CONFIG_TO_OVERRIDE (2, DDR1600, VOLT1_5_ + VOLT1_35_, SR_DIMM0 + DR_DIMM1),
  //       TBLDRV_CONFIG_ENTRY_RTTNOM (CS2_ + CS3_, 2),
  //       TBLDRV_CONFIG_ENTRY_RTTWR (CS2_, 2),
  //       TBLDRV_CONFIG_ENTRY_RTTWR (CS3_, 1),
  //       TBLDRV_CONFIG_ENTRY_ADDRTMG (0x003C3C3C),
  //       TBLDRV_CONFIG_ENTRY_ODCCTRL (0x20112222),
  //
  //     TBLDRV_SPEEDLIMIT_CONFIG_TO_OVERRIDE (2, 2, 0, 0)
  //       TBLDRV_CONFIG_ENTRY_SPEEDLIMIT (DDR1600_FREQUENCY, DDR1333_FREQUENCY, DDR1066_FREQUENCY),
  //
  //     TBLDRV_CONFIG_TO_OVERRIDE (2, DDR1333, VOLT1_5_ + VOLT1_35_, SR_DIMM0 + DR_DIMM1),
  //       TBLDRV_CONFIG_ENTRY_RTTNOM (CS2_ + CS3_, 3),
  //       TBLDRV_CONFIG_ENTRY_RTTWR (CS2_ + CS3_, 0),
  //
  //     TBLDRV_OVERRIDE_MR0_WR (3, 5)
  //     TBLDRV_OVERRIDE_MR0_WR (4, 6)
  //
  //     TBLDRV_OVERRIDE_MR0_CL (3, 5)
  //     TBLDRV_OVERRIDE_MR0_CL (4, 6)
  //     . . . . . . . . . . . . . . . . . . .
  //     . . . . . . . . . . . . . . . . . . .
  //
  //     PSO_END
  //   }
  //
  Buffer = PlatformMemoryConfiguration;
  // Look for configuration descriptor and its sub-descriptor.
  while (Buffer[PSO_TYPE] != PSO_END) {
    FindNewConfig = FALSE;
    ConfigMatched = FALSE;
    if (Buffer[PSO_TYPE] == PSO_TBLDRV_CONFIG) {
      //
      // Config. descriptor is found, check its sub-descriptor to execute different checking routine.
      //
      if ((Buffer[PSO_DATA] == CONFIG_SPEEDLIMIT) && (ProceededPSOType == PSO_TBLDRV_SPEEDLIMIT)) {
        if (MemPCheckTblDrvOverrideConfigSpeedLimit (NBPtr, &Buffer[PSO_DATA + 1])) {
          ConfigMatched = TRUE;
        }
      } else if (Buffer[PSO_DATA] == CONFIG_DONT_CARE) {
        ConfigMatched = TRUE;
      } else {
        if (MemPCheckTblDrvOverrideConfig (NBPtr, &Buffer[PSO_DATA + 1])) {
          ConfigMatched = TRUE;
          if ((Buffer[PSO_DATA] == CONFIG_RC2IBT) && (ProceededPSOType == PSO_TBLDRV_RC2_IBT)) {
            NumOfReg = Buffer[PSO_DATA + 9];
          }
        }
      }
    }

    if (ConfigMatched) {
      //
      // If config. is matched, parsing "Table Driven PSO" macros behinds this config. macro until PSO_END is reached.
      //
      PSOStartPtr = Buffer + (Buffer[PSO_LENGTH] + 2);
      // Look for the current proceeded PSO type in PlatformMemoryConfiguration array.
      while ((PSOStartPtr[PSO_TYPE] != PSO_END)) {
        if (PSOStartPtr[PSO_TYPE] == PSO_TBLDRV_CONFIG) {
          //
          // If there is an additional config. macro existed, break this while loop,
          // then check its content with real platform config. again.
          // If matched, parsing "Table Driven PSO" macros behind it.
          //
          Buffer = PSOStartPtr;
          FindNewConfig = TRUE;
          break;
        } else if (PSOStartPtr[PSO_TYPE] == PSO_TBLDRV_INVALID_TYPE) {
          InvalidConfigDetected = TRUE;
          break;
        }

        if (PSOStartPtr[PSO_TYPE] == ProceededPSOType) {
          //
          // Pre-set overriding Cs/Dimm map to "0xFF" for the types which are regardless of Cs/Dimm
          // for the first time going through the overriding routines.
          //
          if (FirstGoThrough) {
            RetVal = 0xFF;
          }
          switch (ProceededPSOType) {
          case PSO_TBLDRV_SPEEDLIMIT :
            MemPTblDrvOverrideSpeedLimit (NBPtr, &PSOStartPtr[PSO_DATA]);
            break;

          case PSO_TBLDRV_ODT_RTTNOM :
          case PSO_TBLDRV_ODT_RTTWR :
            // Mask off Cs overridng map to record which Cs has been overridden.
            RetVal &= ~ MemPTblDrvOverrideODT (NBPtr, &PSOStartPtr[PSO_TYPE]);
            // Indicate RetVal is inverted.
            InvertRetVal = TRUE;
            break;

          case PSO_TBLDRV_ODTPATTERN :
            MemPTblDrvOverrideODTPattern (NBPtr, &PSOStartPtr[PSO_DATA]);
            break;

          case PSO_TBLDRV_ADDRTMG :
            NBPtr->ChannelPtr->DctAddrTmg = *(UINT32 *)&PSOStartPtr[PSO_DATA];
            IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: AddrTmg = 0x%x for Dct%d\n\n", *(UINT32 *)&PSOStartPtr[PSO_DATA], NBPtr->Dct);
            break;

          case PSO_TBLDRV_ODCCTRL :
            NBPtr->ChannelPtr->DctOdcCtl = *(UINT32 *)&PSOStartPtr[PSO_DATA];
            IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: OdcCtl = 0x%x for Dct%d\n\n", *(UINT32 *)&PSOStartPtr[PSO_DATA], NBPtr->Dct);
            break;

          case PSO_TBLDRV_SLOWACCMODE :
            NBPtr->ChannelPtr->SlowMode = (PSOStartPtr[PSO_DATA] == 1) ? TRUE : FALSE;
            IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: Slow Access Mode = %d for Dct%d\n\n", PSOStartPtr[PSO_DATA], NBPtr->Dct);
            break;

          case PSO_TBLDRV_RC2_IBT :
            // Mask off Dimm overridng map to record which Dimm has been overridden.
            RetVal &= ~ MemPTblDrvOverrideRC2IBT (NBPtr, &PSOStartPtr[PSO_DATA], NumOfReg);
            // Indicate RetVal is inverted.
            InvertRetVal = TRUE;
            break;

          case PSO_TBLDRV_MR0_CL :
            RetVal = 0;
            if (MemPTblDrvOverrideMR0WR (NBPtr, &PSOStartPtr[PSO_DATA])) {
              RetVal = 0xFF;
            }
            break;

          case PSO_TBLDRV_MR0_WR :
            RetVal = 0;
            if (MemPTblDrvOverrideMR0CL (NBPtr, &PSOStartPtr[PSO_DATA])) {
              RetVal = 0xFF;
            }
            break;

          case PSO_TBLDRV_RC10_OPSPEED :
            RetVal = 0;
            if (MemPTblDrvOverrideMR10OpSpeed (NBPtr, &PSOStartPtr[PSO_DATA])) {
              RetVal = 0xFF;
            }
            break;

          case PSO_TBLDRV_LRDIMM_IBT :
            NBPtr->PsPtr->F0RC8 = PSOStartPtr[PSO_DATA];
            NBPtr->PsPtr->F1RC0 = PSOStartPtr[PSO_DATA + 1];
            NBPtr->PsPtr->F1RC1 = PSOStartPtr[PSO_DATA + 2];
            NBPtr->PsPtr->F1RC2 = PSOStartPtr[PSO_DATA + 3];
            IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: LRDIMM IBT for Dct%d\n", NBPtr->Dct);
            IDS_HDT_CONSOLE (MEM_FLOW, "\nF0RC8 = %d, F1RC0 = %d, F1RC1 = %d, F1RC2 = %d", PSOStartPtr[PSO_DATA], PSOStartPtr[PSO_DATA + 1], \
                                        PSOStartPtr[PSO_DATA + 2], PSOStartPtr[PSO_DATA + 3]);
            break;


          default:
            ASSERT (FALSE);
          }
          FirstGoThrough = FALSE;
        }
         PSOStartPtr += (PSOStartPtr[PSO_LENGTH] + 2);
      }

      if (FindNewConfig) {
        continue;
      }
      RetVal = (InvertRetVal) ? ~RetVal : RetVal;
      RetVal16 = (UINT16) RetVal;
      if (InvalidConfigDetected) {
        RetVal16 |= INVALID_CONFIG_FLAG;
      }

      return RetVal16;
    }
     Buffer += (Buffer[PSO_LENGTH] + 2);
  }

  RetVal = (InvertRetVal) ? ~RetVal : RetVal;
  RetVal16 = (UINT16) RetVal;
  if (InvalidConfigDetected) {
    RetVal16 |= INVALID_CONFIG_FLAG;
  }
  return RetVal16;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides the speed limit.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 */
VOID
STATIC
MemPTblDrvOverrideSpeedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  UINT8 CurrentVoltage;

  IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: Max. Memory Speed for Dct%d\n", NBPtr->Dct);

  LibAmdMemCopy (NBPtr->PsPtr->SpeedLimit, Buffer, 6, &(NBPtr->MemPtr->StdHeader));

  for (CurrentVoltage = VOLT1_5_ENCODED_VAL; CurrentVoltage <= VOLT1_25_ENCODED_VAL; CurrentVoltage ++) {
    IDS_HDT_CONSOLE (MEM_FLOW, "%s -> %dMHz\t", (CurrentVoltage == VOLT1_5_ENCODED_VAL) ? "1.5V" : ((CurrentVoltage == VOLT1_35_ENCODED_VAL) ? "1.35V" : "1.25V"), NBPtr->PsPtr->SpeedLimit[CurrentVoltage]);
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\n");
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides the ODTs (RttNom and RttWr).
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 *     @return          Target CS overriding bit map
 *
 */
UINT8
STATIC
MemPTblDrvOverrideODT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  UINT16 i;
  UINT8 TgtCS;

  IDS_HDT_CONSOLE_DEBUG_CODE (
    IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: ODT for Dct%d\n", NBPtr->Dct);
    if (Buffer[0] == PSO_TBLDRV_ODT_RTTNOM) {
      IDS_HDT_CONSOLE (MEM_FLOW, "RttNom = %d for ", Buffer[3]);
    } else {
      IDS_HDT_CONSOLE (MEM_FLOW, "RttWr = %d for ", Buffer[3]);
    }
  );

  TgtCS = Buffer[2];
  for (i = 0; i < MAX_CS_PER_CHANNEL; i++) {
    if ((NBPtr->DCTPtr->Timings.CsEnabled & (UINT16) (1 << i)) != 0) {
      if ((TgtCS & (UINT8) 1 << i) != 0) {
        IDS_HDT_CONSOLE (MEM_FLOW, "CS%d  ", i);
        if (Buffer[0] == PSO_TBLDRV_ODT_RTTNOM) {
          NBPtr->PsPtr->RttNom[i] = Buffer[3];
        } else {
          NBPtr->PsPtr->RttWr[i] = Buffer[3];
        }
      }
    }
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\n");

  return TgtCS;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides the ODT patterns.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 */
VOID
STATIC
MemPTblDrvOverrideODTPattern (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: ODT pattern for Dct%d\n", NBPtr->Dct);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nRODTCSHigh = 0x%x\n", *(UINT32 *)&Buffer[0]);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nRODTCSLow = 0x%x\n", *(UINT32 *)&Buffer[4]);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nWODTCSHigh = 0x%x\n", *(UINT32 *)&Buffer[8]);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nWODTCSLow = 0x%x\n", *(UINT32 *)&Buffer[12]);

  CurrentChannel->PhyRODTCSHigh = *(UINT32 *)&Buffer[0];
  CurrentChannel->PhyRODTCSLow = *(UINT32 *)&Buffer[4];
  CurrentChannel->PhyWODTCSHigh = *(UINT32 *)&Buffer[8];
  CurrentChannel->PhyWODTCSLow = *(UINT32 *)&Buffer[12];

  //WL ODTs need to be modified as well while overriding...
  CurrentChannel->PhyWLODT[0] = (UINT8) (CurrentChannel->PhyWODTCSLow & 0x0F);
  CurrentChannel->PhyWLODT[1] = (UINT8) ((CurrentChannel->PhyWODTCSLow  >> 16) & 0x0F);
  CurrentChannel->PhyWLODT[2] = (UINT8) (CurrentChannel->PhyWODTCSHigh & 0x0F);
  CurrentChannel->PhyWLODT[3] = (UINT8) ((CurrentChannel->PhyWODTCSHigh >> 16) & 0x0F);

  IDS_HDT_CONSOLE (MEM_FLOW, "\n\n");
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides the Ctrl Word 2 and 8.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *     @param[in]       NumOfReg  - Number of registers
 *
 *     @return          Target DIMM overridng bit map
 *
 */
UINT8
STATIC
MemPTblDrvOverrideRC2IBT (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer,
  IN       UINT8 NumOfReg
  )
{
  UINT16 i;
  UINT8 TgtDimm;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: RC2[IBT] for Dct%d\n", NBPtr->Dct);
  IDS_HDT_CONSOLE (MEM_FLOW, "RC2[IBT] = %d for ", Buffer[1]);

  TgtDimm = Buffer[0];
  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if ((CurrentChannel->ChDimmValid & (UINT16) (1 << i)) != 0) {
      if (((TgtDimm & (UINT8) 1 << i) != 0) && (NBPtr->PsPtr->NumOfReg[i] == NumOfReg)) {
        IDS_HDT_CONSOLE (MEM_FLOW, "DIMM%d  ", i);
        CurrentChannel->CtrlWrd02[i] = (Buffer[1] & 0x1) << 2;
        CurrentChannel->CtrlWrd08[i] = (Buffer[1] & 0xE) >> 1;
      }
    }
  }
  IDS_HDT_CONSOLE (MEM_FLOW, "\n");

  return TgtDimm;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides MR0[WR].
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 *     @return          TRUE : Overridden
 *     @return          FALSE : Not overriden
 *
 */
BOOLEAN
STATIC
MemPTblDrvOverrideMR0WR (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  if (Buffer[0] == (UINT8) NBPtr->GetBitField (NBPtr, BFTcl)) {
    NBPtr->PsPtr->MR0CL31 = Buffer[1];
    NBPtr->PsPtr->MR0CL0 = Buffer[2];
    IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: MR0[CL][3:1] = %d,\tMR0[CL][0] = %d for Dct%d\n", \
                                     Buffer[1], Buffer[2], NBPtr->Channel);
    IDS_HDT_CONSOLE (MEM_FLOW, "Tcl = %d\n\n", (UINT8) NBPtr->GetBitField (NBPtr, BFTcl));
    return TRUE;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides MR0[WR].
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 *     @return          TRUE : Overridden
 *     @return          FALSE : Not overriden
 *
 */
BOOLEAN
STATIC
MemPTblDrvOverrideMR0CL (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  if (Buffer[0] == (UINT8) NBPtr->GetBitField (NBPtr, BFTwrDDR3)) {
    NBPtr->PsPtr->MR0WR = Buffer[1];
    IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: MR0[WR] = %d for Dct%d\n", Buffer[1], NBPtr->Dct);
    IDS_HDT_CONSOLE (MEM_FLOW, "Twr = %d\n\n", (UINT8) NBPtr->GetBitField (NBPtr, BFTwrDDR3));
    return TRUE;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function overrides MR10[OperatingSpeed].
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 *     @return          TRUE : Overridden
 *     @return          FALSE : Not overriden
 *
 */
BOOLEAN
STATIC
MemPTblDrvOverrideMR10OpSpeed (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  UINT32 CurDDRrate;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;
  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));

  if ((Buffer[0] & CurDDRrate) != 0) {
    NBPtr->PsPtr->RC10OpSpd = Buffer[1];
    IDS_HDT_CONSOLE (MEM_FLOW, "\nTable Driven Platform Override: MR10[OperatingSpeed] = %d for Dct%d\n", Buffer[1], NBPtr->Dct);
    return TRUE;
  }
  return FALSE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function checks if platform configuration is matched or not.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 *     @return          TRUE : Configuration is matched
 *     @return          FALSE : Configuration is not matched
 *
 */
BOOLEAN
STATIC
MemPCheckTblDrvOverrideConfig (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  UINT8 MaxDimmPerCh;
  UINT32 CurDDRrate;
  UINT8 DDR3Voltage;
  UINT16 RankTypeOfPopulatedDimm;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;

  // Get platform configuration.
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);
  CurDDRrate = (UINT32) (1 << (CurrentChannel->DCTPtr->Timings.Speed / 66));
  DDR3Voltage = (UINT8) (1 << CONVERT_VDDIO_TO_ENCODED (NBPtr->RefPtr->DDR3Voltage));
  RankTypeOfPopulatedDimm = MemAGetPsRankType (CurrentChannel);

  if ((MaxDimmPerCh == Buffer[0]) && ((DDR3Voltage & Buffer[1]) != 0) &&
    ((CurDDRrate & *(UINT32 *)&Buffer[2]) != 0) && ((RankTypeOfPopulatedDimm & *(UINT16 *)&Buffer[6]) != 0)) {
    return TRUE;
  }

  return FALSE;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *    This function checks if platform configuration is matched or not.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       *Buffer  - Pointer to Platform config table
 *
 *     @return          TRUE : Configuration is matched
 *     @return          FALSE : Configuration is not matched
 *
 */
BOOLEAN
STATIC
MemPCheckTblDrvOverrideConfigSpeedLimit (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 *Buffer
  )
{
  UINT8 MaxDimmPerCh;
  UINT8 NumOfSR;
  UINT8 NumOfDR;
  UINT8 NumOfQR;
  UINT8 NumOfLRDimm;
  UINT8 i;
  CH_DEF_STRUCT *CurrentChannel;

  CurrentChannel = NBPtr->ChannelPtr;
  NumOfSR = 0;
  NumOfDR = 0;
  NumOfQR = 0;
  NumOfLRDimm = 0;

  // Get platform configuration.
  MaxDimmPerCh = GetMaxDimmsPerChannel (NBPtr->RefPtr->PlatformMemoryConfiguration, NBPtr->MCTPtr->SocketId, CurrentChannel->ChannelID);

  for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
    if ((CurrentChannel->DimmSRPresent & (UINT8) (1 << i)) != 0) {
      NumOfSR += 1;
    } else if ((CurrentChannel->DimmDrPresent & (UINT16) (1 << i)) != 0) {
      NumOfDR += 1;
    } else if ((CurrentChannel->DimmQrPresent & (UINT16) (1 << i)) != 0) {
      if (i < 2) {
        NumOfQR += 1;
      }
    } else if ((CurrentChannel->LrDimmPresent & (UINT16) (1 << i))) {
      NumOfLRDimm += 1;
    }
  }

  if ((Buffer[0] == MaxDimmPerCh) && (Buffer[1] == CurrentChannel->Dimms)) {
    if (NBPtr->MCTPtr->Status[SbLrdimms] == TRUE) {
      if (Buffer[5] == NumOfLRDimm) {
        return TRUE;
      }
    } else {
      if ((Buffer[2] == NumOfSR) && (Buffer[3] == NumOfDR) && (Buffer[4] == NumOfQR)) {
        return TRUE;
      }
    }
  }

  return FALSE;
}

