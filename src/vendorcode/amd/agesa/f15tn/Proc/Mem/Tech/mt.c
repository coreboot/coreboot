/* $NoKeywords:$ */
/**
 * @file
 *
 * mt.c
 *
 * Common Technology file
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
#include "amdlib.h"
#include "mport.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_TECH_MT_FILECODE
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
MemTDefaultTechnologyHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  );
/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function is the default return for non-training technology features
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
BOOLEAN
MemTFeatDef (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the TestFail bit for all CS that fail training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 */
VOID
MemTMarkTrainFail (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  MEM_NB_BLOCK *NBPtr;
  UINT8 Dct;
  UINT8 ChipSel;

  NBPtr = TechPtr->NBPtr;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
    NBPtr->SwitchDCT (NBPtr, Dct);
    NBPtr->DCTPtr->Timings.CsEnabled &= ~NBPtr->DCTPtr->Timings.CsTrainFail;
    for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel ++) {
      if ((NBPtr->DCTPtr->Timings.CsTrainFail & ((UINT16)1 << ChipSel)) != 0) {
        NBPtr->SetBitField (NBPtr, (BFCSBaseAddr0Reg + ChipSel), (UINT32)1 << BFTestFail);
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the initial controller environment before training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTBeginTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdReadCpuReg (CR4_REG, &TechPtr->CR4reg);
  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg | ((UINT32)1 << 9)); // enable SSE2

  LibAmdMsrRead (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);            // HWCR
  TechPtr->HwcrLo = SMsr.lo;
  SMsr.lo |= 0x00020000;                  // turn on HWCR.wrap32dis
  SMsr.lo &= 0xFFFF7FFF;                  // turn off HWCR.SSEDIS
  LibAmdMsrWrite (HWCR, (UINT64 *) (&SMsr), &MemPtr->StdHeader);

  TechPtr->DramEcc = (UINT8) NBPtr->GetBitField (NBPtr, BFDramEccEn);
  NBPtr->SetBitField (NBPtr, BFDramEccEn, 0); // Disable ECC
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets the final controller environment after training.
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *
 */

VOID
MemTEndTraining (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  S_UINT64 SMsr;
  MEM_DATA_STRUCT *MemPtr;
  MEM_NB_BLOCK  *NBPtr;

  NBPtr = TechPtr->NBPtr;
  MemPtr = NBPtr->MemPtr;

  LibAmdWriteCpuReg (CR4_REG, TechPtr->CR4reg);

  LibAmdMsrRead (HWCR, (UINT64 *)&SMsr, &MemPtr->StdHeader);
  SMsr.lo = TechPtr->HwcrLo;
  LibAmdMsrWrite (HWCR, (UINT64 *)&SMsr, &MemPtr->StdHeader);

  NBPtr->SetBitField (NBPtr, BFDramEccEn, TechPtr->DramEcc);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *      This function sets all the bytelanes/nibbles to the same delay value
 *
 *     @param[in,out]   *TechPtr   - Pointer to the MEM_TECH_BLOCK
 *     @param[in]       Dly - Delay value to set
 *
 */

VOID
MemTSetDQSDelayAllCSR (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN       UINT8 Dly
  )
{
  UINT8 i;
  UINT8 MaxBytelanes;
  MaxBytelanes = (TechPtr->NBPtr->MCTPtr->Status[SbEccDimms] && TechPtr->NBPtr->IsSupported[EccByteTraining]) ? 9 : 8;

  for (i = 0; i <  MaxBytelanes; i++) {
    TechPtr->SetDQSDelayCSR (TechPtr, i, Dly);
  }
  TechPtr->NBPtr->FamilySpecificHook[RegAccessFence] (TechPtr->NBPtr, NULL);
}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function is used to intialize common technology functions
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_TECH_BLOCK
 * ----------------------------------------------------------------------------
 */
VOID
MemTCommonTechInit (
  IN OUT   MEM_TECH_BLOCK *TechPtr
  )
{
  UINT8 i;
  for (i = 0; i < NumberOfTechHooks; i++) {
    TechPtr->TechnologySpecificHook[i] = MemTDefaultTechnologyHook;
  }
}
/*-----------------------------------------------------------------------------
 *
 *
 *     This function is an empty function used to intialize TechnologySpecificHook array
 *
 *     @param[in,out]  *TechPtr     - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]  OptParam   - Optional parameter
 *
 *     @return  FALSE - always
 * ----------------------------------------------------------------------------
 */
BOOLEAN
STATIC
MemTDefaultTechnologyHook (
  IN OUT   MEM_TECH_BLOCK *TechPtr,
  IN OUT   VOID *OptParam
  )
{
  return FALSE;
}
