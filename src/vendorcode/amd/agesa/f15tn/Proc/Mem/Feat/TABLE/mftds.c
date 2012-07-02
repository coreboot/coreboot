/* $NoKeywords:$ */
/**
 * @file
 *
 * mftds.c
 *
 * Northbridge table drive support file for DR
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/TABLE)
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



#include "AGESA.h"
#include "amdlib.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mftds.h"
#include "Ids.h"
#include "OptionMemory.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_TABLE_MFTDS_FILECODE
/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */
#define MAX_BYTELANES_PER_CHANNEL   (8 + 1)  ///< Max Bytelanes per channel

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

VOID
SetTableValues (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS MTPtr
  );

VOID
SetTableValuesLoop (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS *MTPtr,
  IN       UINT8     time
  );

/*-----------------------------------------------------------------------------
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_TABLE_ALIAS structure
 *     @param[in]       time     - Indicate the timing for the register which is written.
 *
 *     @return          None
 * ----------------------------------------------------------------------------
 */
VOID
MemFInitTableDrive (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8     time
  )
{
  MEM_TABLE_ALIAS *MTPtr;
  MEM_TABLE_ALIAS *IdsMTPtr;

  ASSERT (NBPtr != NULL);
  IdsMTPtr = NULL;
  IDS_HDT_CONSOLE (MEM_FLOW, "MemFInitTableDrive [%X] Start\n", time);
  MTPtr = (MEM_TABLE_ALIAS *) NBPtr->RefPtr->TableBasedAlterations;

  IDS_SKIP_HOOK (IDS_GET_DRAM_TABLE, &IdsMTPtr, &(NBPtr->MemPtr->StdHeader)) {
    IDS_OPTION_HOOK (IDS_INIT_DRAM_TABLE, NBPtr, &(NBPtr->MemPtr->StdHeader));
    IDS_OPTION_HOOK (IDS_GET_DRAM_TABLE, &IdsMTPtr, &(NBPtr->MemPtr->StdHeader));
  }

  SetTableValuesLoop (NBPtr, MTPtr, time);
  SetTableValuesLoop (NBPtr, IdsMTPtr, time);

  IDS_OPTION_HOOK (IDS_MT_BASE + time, NBPtr, &(NBPtr->MemPtr->StdHeader));
  IDS_HDT_CONSOLE (MEM_FLOW, "MemFInitTableDrive End\n");
}

/*-----------------------------------------------------------------------------
 *
 *   This function initializes bit field translation table
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MTPtr   - Pointer to the MEM_TABLE_ALIAS structure
 *     @param[in]       time     - Indicate the timing for the register which is written.
 *
 *     @return          None
 * ----------------------------------------------------------------------------
 */
VOID
SetTableValuesLoop (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS *MTPtr,
  IN       UINT8     time
  )
{
  UINT8 i;
  UINT8 CurDct;

  if (MTPtr != NULL) {
    CurDct = NBPtr->Dct;
    for (i = 0; MTPtr[i].time != MTEnd; i++) {
      if ((MTPtr[i].attr != MTAuto) && (MTPtr[i].time == time)) {
        SetTableValues (NBPtr, MTPtr[i]);
      }
    }
    NBPtr->SwitchDCT (NBPtr, CurDct);
  }
}

/*-----------------------------------------------------------------------------
 *
 *   Engine for setting Table Value.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in]       MTPtr    - Pointer to the MEM_TABLE_ALIAS structure
 *
 *     @return      None
 * ----------------------------------------------------------------------------
 */
VOID
SetTableValues (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       MEM_TABLE_ALIAS MTPtr
  )
{
  UINT8 AccessType;
  UINT16 ByteLane;
  UINT8 Dct;
  UINT8 i;
  UINT8 j;
  UINT32 TempVal[36];
  UINT8 *DqsSavePtr;
  UINT8  DqsOffset;
  BOOLEAN SaveDqs;

  AccessType = 0;
  DqsSavePtr = NULL;
  SaveDqs = TRUE;

  ASSERT (MTPtr.time <= MTValidTimePointLimit);
  ASSERT (MTPtr.attr <= MTOr);
  ASSERT (MTPtr.node <= MTNodes);
  ASSERT (MTPtr.dct <= MTDcts);
  ASSERT (MTPtr.dimm <= MTDIMMs);
  ASSERT (MTPtr.data.s.bytelane <= MTBLs);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    if ((MTPtr.dct == MTDcts) || (MTPtr.dct == Dct)) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      switch (MTPtr.bfindex) {
      case BFRcvEnDly:
        AccessType = AccessRcvEnDly;
        DqsSavePtr = NULL;
        break;
      case BFWrDatDly:
        AccessType = AccessWrDatDly;
        DqsSavePtr = NBPtr->ChannelPtr->WrDatDlys;
        break;
      case BFRdDqsDly:
        AccessType = AccessRdDqsDly;
        DqsSavePtr = NBPtr->ChannelPtr->RdDqsDlys;
        break;
      case BFWrDqsDly:
        AccessType = AccessWrDqsDly;
        DqsSavePtr = NBPtr->ChannelPtr->WrDqsDlys;
        break;
      case BFPhRecDly:
        AccessType = AccessPhRecDly;
        SaveDqs = FALSE;
        break;
      default:
        AccessType = 0xFF;
        break;
      }
      if (AccessType == 0xFF) {
        if (MTPtr.attr == MTOverride) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, MTPtr.data.s.value);
        }
        if (MTPtr.attr == MTSubtract) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, NBPtr->GetBitField (NBPtr, MTPtr.bfindex) - MTPtr.data.s.value);
        }
        if (MTPtr.attr == MTAdd) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, NBPtr->GetBitField (NBPtr, MTPtr.bfindex) + MTPtr.data.s.value);
        }
        if (MTPtr.attr == MTAnd) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, (NBPtr->GetBitField (NBPtr, MTPtr.bfindex) & MTPtr.data.s.value));
        }
        if (MTPtr.attr == MTOr) {
          NBPtr->SetBitField (NBPtr, MTPtr.bfindex, (NBPtr->GetBitField (NBPtr, MTPtr.bfindex) | MTPtr.data.s.value));
        }
      } else {
        // Store the DQS data first
        for (i = 0; i < NBPtr->CsPerChannel; i = i + NBPtr->CsPerDelay) {
            for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
              TempVal[i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j] = NBPtr->GetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j));
            }
        }
        //
        // Single Value with Bytleane mask option
        //   Indicated by the vtype flag
        //
        if (MTPtr.vtype == VT_MSK_VALUE) {
          // set the value which defined in Memory table.
          for (i = 0; i < NBPtr->CsPerChannel; i = i + NBPtr->CsPerDelay) {
            ByteLane = MTPtr.data.s.bytelane;
            if ((MTPtr.dimm == MTDIMMs) || ((MTPtr.dimm * NBPtr->CsPerDelay) == i)) {
                for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                  DqsOffset = (i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j);
                  if ((ByteLane & (UINT16)1) != 0) {
                    if (MTPtr.attr == MTOverride) {
                      TempVal[DqsOffset] = (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTSubtract) {
                      TempVal[DqsOffset] -= (UINT16)MTPtr.data.s.value;
                    }
                    if (MTPtr.attr == MTAdd) {
                      TempVal[DqsOffset] += (UINT16)MTPtr.data.s.value;
                    }
                    NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)TempVal[DqsOffset]);
                    if (SaveDqs) {
                      if (DqsSavePtr == NULL) {
                        NBPtr->ChannelPtr->RcvEnDlys[DqsOffset] = (UINT16)TempVal[DqsOffset];
                      } else {
                        DqsSavePtr[DqsOffset] = (UINT8)TempVal[DqsOffset];
                      }
                    }
                  }
                  ByteLane = ByteLane >> (UINT16)1;
                }
            }
          }
        } else {
          // Multiple values specified in a byte array
          for (i = 0; i < NBPtr->CsPerChannel; i = i + NBPtr->CsPerDelay) {
            if ((MTPtr.dimm == MTDIMMs) || ((MTPtr.dimm * NBPtr->CsPerDelay) == i)) {
                for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                  DqsOffset = (i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j);
                  if (MTPtr.attr == MTOverride) {
                    TempVal[DqsOffset] = MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTSubtract) {
                    TempVal[DqsOffset] -= MTPtr.data.bytelanevalue[j];
                  }
                  if (MTPtr.attr == MTAdd) {
                    TempVal[DqsOffset] += MTPtr.data.bytelanevalue[j];
                  }
                  NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)TempVal[DqsOffset]);
                  if (SaveDqs) {
                    if (DqsSavePtr == NULL) {
                      NBPtr->ChannelPtr->RcvEnDlys[DqsOffset] = (UINT16)TempVal[DqsOffset];
                    } else {
                      DqsSavePtr[DqsOffset] = (UINT8)TempVal[DqsOffset];
                    }
                  }
                }
            }
          }
        }
        // set the DQS value to left DIMMs.
        i = MTPtr.dimm;
        if (i != MTDIMMs) {
          i = i * NBPtr->CsPerDelay + NBPtr->CsPerDelay;
          while (i < NBPtr->CsPerChannel) {
              for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i / NBPtr->CsPerDelay, j), (UINT16)TempVal[i / NBPtr->CsPerDelay * MAX_BYTELANES_PER_CHANNEL + j]);
              }
            i = i + NBPtr->CsPerDelay;
          }
        }
      }
    }
  }
}






