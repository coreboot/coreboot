/* $NoKeywords:$ */
/**
 * @file
 *
 * mttecc.c
 *
 * Technology ECC byte support
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Tech)
 * @e \$Revision: 56279 $ @e \$Date: 2011-07-11 13:11:28 -0600 (Mon, 11 Jul 2011) $
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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_TECH_MTTECC_FILECODE
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

VOID
STATIC
MemTCalcDQSEccTmg (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Type,
  IN OUT   VOID  *DlyArray
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function sets the DQS ECC timings
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 *     @return          TRUE -  No fatal error occurs.
 *     @return          FALSE - Fatal error occurs.
 */

BOOLEAN
MemTSetDQSEccTmgs (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 Dct;
  UINT8 Dimm;
  UINT8 i;

  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  if (NBPtr->MCTPtr->NodeMemSize) {
    for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
      NBPtr->SwitchDCT (NBPtr, Dct);
      if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
        ChannelPtr = NBPtr->ChannelPtr;
        for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
          if (NBPtr->DCTPtr->Timings.CsEnabled & ((UINT16)1 << (Dimm * 2))) {
            i = Dimm * TechPtr->DlyTableWidth ();
            MemTCalcDQSEccTmg (TechPtr, Dimm, AccessRcvEnDly, &ChannelPtr->RcvEnDlys[i]);
            MemTCalcDQSEccTmg (TechPtr, Dimm, AccessRdDqsDly, &ChannelPtr->RdDqsDlys[i]);
            MemTCalcDQSEccTmg (TechPtr, Dimm, AccessWrDatDly, &ChannelPtr->WrDatDlys[i]);
          }
        }
      }
    }
  }
  return (BOOLEAN) (NBPtr->MCTPtr->ErrCode < AGESA_FATAL);
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *   This function calculates the DQS ECC timings
 *
 *     @param[in,out]   *TechPtr - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dimm - Dimm number
 *     @param[in]       Type - Type of DQS timing
 *     @param[in,out]   *DlyArray - Pointer to the array of delays per this Dimm
 *
 */

VOID
STATIC
MemTCalcDQSEccTmg (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dimm,
  IN       UINT8 Type,
  IN OUT   VOID  *DlyArray
  )
{
  UINT8 i;
  UINT8 j;
  UINT8 Scale;
  UINT8 EccByte;
  UINT16 ByteiDly;
  UINT16 BytejDly;
  UINT16 EccDly;
  UINT8 *WrDqsDly;
  MEM_NB_BLOCK *NBPtr;
  CH_DEF_STRUCT *ChannelPtr;

  NBPtr = TechPtr->NBPtr;
  ChannelPtr = NBPtr->ChannelPtr;

  EccByte = TechPtr->MaxByteLanes ();
  i = (UINT8) (ChannelPtr->DctEccDqsLike & 0xFF);
  j = (UINT8) (ChannelPtr->DctEccDqsLike >> 8);
  Scale = ChannelPtr->DctEccDqsScale;
  WrDqsDly = &ChannelPtr->WrDqsDlys[Dimm * TechPtr->DlyTableWidth ()];

  if (Type == AccessRcvEnDly) {
    ByteiDly = ((UINT16 *) DlyArray)[i];
    BytejDly = ((UINT16 *) DlyArray)[j];
  } else {
    ByteiDly = ((UINT8 *) DlyArray)[i];
    BytejDly = ((UINT8 *) DlyArray)[j];
  }

  //
  // For WrDatDly, Subtract TxDqs Delay to get
  //  TxDq-TxDqs Delta, which is what should be averaged.
  //
  if (Type == AccessWrDatDly) {
    ByteiDly = ByteiDly - WrDqsDly[i];
    BytejDly = BytejDly - WrDqsDly[j];
  }

  if (BytejDly > ByteiDly) {
    EccDly = ByteiDly + (UINT8) (((UINT16) (BytejDly - ByteiDly) * Scale + 0x77) / 0xFF);
    //                                                            Round up --^
  } else {
    EccDly = BytejDly + (UINT8) (((UINT16) (ByteiDly - BytejDly) * (0xFF - Scale) + 0x77) / 0xFF);
    //                                                                     Round up --^
  }

  if (Type == AccessRcvEnDly) {
    ((UINT16 *) DlyArray)[EccByte] = EccDly;
  } else {
    ((UINT8 *) DlyArray)[EccByte] = (UINT8) EccDly;
  }

  //
  // For WrDatDly, Add back the TxDqs value for ECC bytelane
  //
  if (Type == AccessWrDatDly) {
    EccDly = EccDly + WrDqsDly[EccByte];
  }

  NBPtr->SetTrainDly (NBPtr, Type, DIMM_BYTE_ACCESS (Dimm, EccByte), EccDly);
}
