/* $NoKeywords:$ */
/**
 * @file
 *
 * mntn.h
 *
 * Northbridge TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/TN)
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

#ifndef _MNTN_H_
#define _MNTN_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */
#define MAX_DCTS_PER_NODE_TN          2
#define MAX_CHANNELS_PER_DCT_TN       1
#define MAX_NODES_SUPPORTED_TN        1
#define MAX_CS_PER_CHANNEL_TN         4

#define DEFAULT_WR_ODT_TN 6
#define DEFAULT_RD_ODT_TN 6
#define DEFAULT_RD_ODT_TRNONDLY_TN 0
/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

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
BOOLEAN
MemConstructNBBlockTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   MEM_DATA_STRUCT *MemPtr,
  IN       MEM_FEAT_BLOCK_NB *FeatPtr,
  IN       MEM_SHARED_DATA *SharedPtr,
  IN       UINT8 NodeID
  );

VOID
MemNInitNBDataTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitDefaultsTN (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  );

BOOLEAN
MemNInitializeMctTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAutoConfigTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNOtherTimingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNInitPhyCompTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNWritePatternTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT32 Address,
  IN       UINT8 Pattern[],
  IN       UINT16 ClCount
  );

VOID
MemNReadPatternTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 Buffer[],
  IN       UINT32 Address,
  IN       UINT16 ClCount
  );

VOID
MemNInitNBRegTableTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   TSEFO NBRegTable[]
  );

BOOLEAN
MemNIsIdSupportedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       CPU_LOGICAL_ID *LogicalIdPtr
  );

UINT32
MemNCmnGetSetFieldTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT8 IsSet,
  IN       BIT_FIELD_NAME FieldName,
  IN       UINT32 Field
  );

BOOLEAN
memNEnableTrainSequenceTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNTechBlockSwitchTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNAfterDQSTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNCapSpeedBatteryLifeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNGetMaxLatParamsTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly,
  IN OUT   UINT16 *MinDlyPtr,
  IN OUT   UINT16 *MaxDlyPtr,
  IN OUT   UINT16 *DlyBiasPtr
  );

VOID
MemNSetMaxLatencyTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN       UINT16 MaxRcvEnDly
  );

BOOLEAN
MemNExitPhyAssistedTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNOverrideRcvEnSeedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

VOID
MemNBeforeDQSTrainingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNProgramNbPstateDependentRegistersTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNAdjustPllLockTimeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *PllLockTime
  );

BOOLEAN
MemNOverrideWLSeedTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *SeedPtr
  );

BOOLEAN
MemNFinalizeMctTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNHtMemMapInitTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

UINT32
MemNGetUmaSizeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforeDramInitTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNCSIntLvLowAddrAdjTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *LowBit
  );

VOID
MemNAllocateC6StorageTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPFenceAdjustTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   INT16 *Value16
  );

BOOLEAN
MemNReleaseNbPstateTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNMemPstateStageChangeTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  );

BOOLEAN
MemNProgramFence2RxDllTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Fence2Data
  );

VOID
MemNAdjustNBPstateVolTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNRdDqsDlyRestartChkTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Center
  );

BOOLEAN
MemNHookBfWrDatTrnTN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *ChipSel
  );

VOID
MemNSetOtherTimingTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNPowerDownCtlTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

VOID
MemNBeforePlatformSpecTN (
  IN OUT   MEM_NB_BLOCK *NBPtr
  );

BOOLEAN
MemNWLMR1TN (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *Value
  );
#endif  /* _MNTN_H_ */
