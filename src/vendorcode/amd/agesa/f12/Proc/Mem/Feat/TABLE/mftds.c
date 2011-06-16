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
 * @e \$Revision: 47683 $ @e \$Date: 2011-02-25 10:06:08 +0800 (Fri, 25 Feb 2011) $
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
        for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
          for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
            TempVal[i * MAX_BYTELANES_PER_CHANNEL + j] = NBPtr->GetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i, j));
          }
        }
        //
        // Single Value with Bytleane mask option
        //   Indicated by the vtype flag
        //
        if (MTPtr.vtype == VT_MSK_VALUE) {
          // set the value which defined in Memory table.
          for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
            ByteLane = MTPtr.data.s.bytelane;
            if ((MTPtr.dimm == MTDIMMs) || (MTPtr.dimm == i)) {
              for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                DqsOffset = (i * MAX_BYTELANES_PER_CHANNEL + j);
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
                  NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i, j), (UINT16)TempVal[DqsOffset]);
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
          for (i = 0; i < MAX_DIMMS_PER_CHANNEL; i++) {
            if ((MTPtr.dimm == MTDIMMs) || (MTPtr.dimm == i)) {
              for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
                DqsOffset = (i * MAX_BYTELANES_PER_CHANNEL + j);
                if (MTPtr.attr == MTOverride) {
                  TempVal[DqsOffset] = MTPtr.data.bytelanevalue[j];
                }
                if (MTPtr.attr == MTSubtract) {
                  TempVal[DqsOffset] -= MTPtr.data.bytelanevalue[j];
                }
                if (MTPtr.attr == MTAdd) {
                  TempVal[DqsOffset] += MTPtr.data.bytelanevalue[j];
                }
                NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i, j), (UINT16)TempVal[DqsOffset]);
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
        while ((i != MTDIMMs) && ((++i) < MAX_DIMMS_PER_CHANNEL)) {
          for (j = 0; j < MAX_BYTELANES_PER_CHANNEL; j++) {
            NBPtr->SetTrainDly (NBPtr, AccessType, DIMM_BYTE_ACCESS (i, j), (UINT16)TempVal[i * MAX_BYTELANES_PER_CHANNEL + j]);
          }
        }
      }
    }
  }
}






