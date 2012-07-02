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
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

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
