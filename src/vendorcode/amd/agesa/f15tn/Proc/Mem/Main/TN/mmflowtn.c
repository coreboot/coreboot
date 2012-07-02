/* $NoKeywords:$ */
/**
 * @file
 *
 * mmflowtn.c
 *
 * Main Memory initialization sequence for TN
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main/TN)
 * @e \$Revision: 64574 $ @e \$Date: 2012-01-25 01:01:51 -0600 (Wed, 25 Jan 2012) $
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mntn.h"
#include "mt.h"
#include "mmlvddr3.h"
#include "cpuFamilyTranslation.h"
#include "Filecode.h"
#include "GeneralServices.h"
CODE_GROUP (G3_DXE)
RDATA_GROUP (G3_DXE)

#define FILECODE PROC_MEM_MAIN_TN_MMFLOWTN_FILECODE
/* features */
#include "mftds.h"

extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
extern OPTION_MEM_FEATURE_MAIN MemMS3Save;

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
AGESA_STATUS
MemMFlowTN (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  );

/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function defines the memory initialization flow for
 *      systems that only support TN processors.
 *
 *     @param[in,out]   *MemMainPtr   - Pointer to the MEM_MAIN_DATA_BLOCK
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_FATAL
 *                          - AGESA_CRITICAL
 *                          - AGESA_SUCCESS
 */
AGESA_STATUS
MemMFlowTN (
  IN OUT   MEM_MAIN_DATA_BLOCK *MemMainPtr
  )
{
  MEM_NB_BLOCK  *NBPtr;
  MEM_TECH_BLOCK *TechPtr;
  MEM_DATA_STRUCT *MemPtr;
  ID_INFO CallOutIdInfo;

  NBPtr = MemMainPtr->NBPtr;
  TechPtr = MemMainPtr->TechPtr;
  MemPtr = MemMainPtr->MemPtr;

  GetLogicalIdOfSocket (MemPtr->DiesPerSystem[BSP_DIE].SocketId, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid), &(MemPtr->StdHeader));
  if (!MemNIsIdSupportedTN (NBPtr, &(MemPtr->DiesPerSystem[BSP_DIE].LogicalCpuid))) {
    MemPtr->IsFlowControlSupported = FALSE;
    return AGESA_FATAL;
  } else {
    MemPtr->IsFlowControlSupported = TRUE;
  }

  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeInitializeMCT);

  //----------------------------------------------------------------
  // Initialize MCT
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitializeMCT, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].InitializeMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Low voltage DDR3
  //----------------------------------------------------------------
  // Levelize DDR3 voltage based on socket, as each socket has its own voltage for dimms.
  AGESA_TESTPOINT (TpProcMemLvDdr3, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.LvDDR3 (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Initialize DRAM and DCTs
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemInitMCT, &(MemMainPtr->MemPtr->StdHeader));
  // Initialize Memory Controller and Dram
  if (!NBPtr[BSP_DIE].InitMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL; //fatalexit
  }

  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeDInit);

  //------------------------------------------------
  // Finalize target frequency
  //------------------------------------------------
  if (!MemMLvDdr3PerformanceEnhFinalize (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //------------------------------------------------
  // Callout before Dram Init
  //------------------------------------------------
  AGESA_TESTPOINT (TpProcMemBeforeAgesaHookBeforeDramInit, &(MemMainPtr->MemPtr->StdHeader));
  CallOutIdInfo.IdField.SocketId = NBPtr[BSP_DIE].MCTPtr->SocketId;
  CallOutIdInfo.IdField.ModuleId = NBPtr[BSP_DIE].MCTPtr->DieId;
  IDS_HDT_CONSOLE (MEM_FLOW, "\nCalling out to Platform BIOS on Socket %d, Module %d...\n", CallOutIdInfo.IdField.SocketId, CallOutIdInfo.IdField.ModuleId);
  AgesaHookBeforeDramInit ((UINTN) CallOutIdInfo.IdInformation, MemMainPtr->MemPtr);
  IDS_HDT_CONSOLE (MEM_FLOW, "\nVDDIO = 1.%dV\n", (NBPtr[BSP_DIE].RefPtr->DDR3Voltage == VOLT1_5) ? 5 :
                                        (NBPtr[BSP_DIE].RefPtr->DDR3Voltage == VOLT1_35) ? 35 :
                                        (NBPtr[BSP_DIE].RefPtr->DDR3Voltage == VOLT1_25) ? 25 : 999);
  AGESA_TESTPOINT (TpProcMemAfterAgesaHookBeforeDramInit, &(NBPtr->MemPtr->StdHeader));

  //-------------------------------------------------
  // Do dram init and create memory map
  //-------------------------------------------------
  IDS_OPTION_HOOK (IDS_BEFORE_DRAM_INIT, &NBPtr[BSP_DIE], &(MemMainPtr->MemPtr->StdHeader));
  NBPtr[BSP_DIE].StartupDCT (&NBPtr[BSP_DIE]);

  // Create memory map
  AGESA_TESTPOINT (TpProcMemSystemMemoryMapping, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].HtMemMapInit (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------
  // If there is no dimm on the system, do fatal exit
  //----------------------------------------------------
  if (NBPtr[BSP_DIE].RefPtr->SysLimit == 0) {
    PutEventLog (AGESA_FATAL, MEM_ERROR_NO_DIMM_FOUND_ON_SYSTEM, 0, 0, 0, 0, &(MemMainPtr->MemPtr->StdHeader));
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // CpuMemTyping
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemMtrrConfiguration, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].CpuMemTyping (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Before Training Table values
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTBeforeTrn);

  //----------------------------------------------------------------
  // Memory Context Restore
  //----------------------------------------------------------------
  if (!MemFeatMain.MemRestore (MemMainPtr)) {
    // Do DQS training only if memory context restore fails

    //----------------------------------------------------------------
    // Training
    //----------------------------------------------------------------
    MemMainPtr->mmSharedPtr->DimmExcludeFlag = TRAINING;
    AGESA_TESTPOINT (TpProcMemDramTraining, &(MemMainPtr->MemPtr->StdHeader));
    IDS_SKIP_HOOK (IDS_BEFORE_DQS_TRAINING, MemMainPtr, &(MemMainPtr->MemPtr->StdHeader)) {
      if (!MemFeatMain.Training (MemMainPtr)) {
        return AGESA_FATAL;
      }
    }
    IDS_HDT_CONSOLE (MEM_FLOW, "\nEnd DQS training\n\n");
  }

  //----------------------------------------------------------------
  // Disable chipselects that fail training
  //----------------------------------------------------------------
  MemMainPtr->mmSharedPtr->DimmExcludeFlag = END_TRAINING;
  MemFeatMain.ExcludeDIMM (MemMainPtr);
  MemMainPtr->mmSharedPtr->DimmExcludeFlag = NORMAL;

  //----------------------------------------------------------------
  // OtherTiming
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemOtherTiming, &(MemMainPtr->MemPtr->StdHeader));
  if (!NBPtr[BSP_DIE].OtherTiming (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // After Training Table values
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTAfterTrn);

  //----------------------------------------------------------------
  // Interleave channels
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->InterleaveChannels (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // Interleave banks
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->InterleaveBanks (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // After Programming Interleave registers
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTAfterInterleave);

  //----------------------------------------------------------------
  // Memory Clear
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemMemClr, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.MemClr (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // C6 Storage Allocation
  //----------------------------------------------------------------
  NBPtr[BSP_DIE].AllocateC6Storage (&NBPtr[BSP_DIE]);

  //----------------------------------------------------------------
  // UMA Allocation & UMAMemTyping
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemUMAMemTyping, &(MemMainPtr->MemPtr->StdHeader));
  if (!MemFeatMain.UmaAllocation (MemMainPtr)) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // Interleave region
  //----------------------------------------------------------------
  NBPtr[BSP_DIE].FeatPtr->InterleaveRegion (&NBPtr[BSP_DIE]);

  //----------------------------------------------------------------
  // OnDimm Thermal
  //----------------------------------------------------------------
  if (NBPtr[BSP_DIE].FeatPtr->OnDimmThermal (&NBPtr[BSP_DIE])) {
    if (NBPtr[BSP_DIE].MCTPtr->ErrCode == AGESA_FATAL) {
      return AGESA_FATAL;
    }
  }

  //----------------------------------------------------------------
  // Finalize MCT
  //----------------------------------------------------------------
  if (!NBPtr[BSP_DIE].FinalizeMCT (&NBPtr[BSP_DIE])) {
    return AGESA_FATAL;
  }

  //----------------------------------------------------------------
  // After Finalize MCT
  //----------------------------------------------------------------
  MemFInitTableDrive (&NBPtr[BSP_DIE], MTAfterFinalizeMCT);

  //----------------------------------------------------------------
  // Memory Context Save
  //----------------------------------------------------------------
  MemFeatMain.MemSave (MemMainPtr);

  //----------------------------------------------------------------
  // Memory DMI support
  //----------------------------------------------------------------
  if (!MemFeatMain.MemDmi (MemMainPtr)) {
    return AGESA_CRITICAL;
  }


  //----------------------------------------------------------------
  // Save memory S3 data
  //----------------------------------------------------------------
  if (!MemMS3Save (MemMainPtr)) {
    return AGESA_CRITICAL;
  }

  //----------------------------------------------------------------
  // Switch back to DCT 0 before sending control back
  //----------------------------------------------------------------
  NBPtr[BSP_DIE].SwitchDCT (&NBPtr[BSP_DIE], 0);

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
