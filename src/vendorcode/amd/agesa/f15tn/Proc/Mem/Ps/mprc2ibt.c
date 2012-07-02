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
        if ((TblPtr->DimmPerCh & NOD) != 0) {
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