/* $NoKeywords:$ */
/**
 * @file
 *
 * mmflow.c
 *
 * Main Memory Flow Entrypoint file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Main)
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
#include "AdvancedApi.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_MAIN_MMFLOW_FILECODE
/* features */

extern MEM_NB_SUPPORT memNBInstalled[];
extern MEM_TECH_CONSTRUCTOR* memTechInstalled[];
extern MEM_FEAT_BLOCK_MAIN MemFeatMain;
extern MEM_FLOW_CFG* memFlowControlInstalled[];

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
MemSPDDataProcess (
  IN OUT   MEM_DATA_STRUCT *MemPtr
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
 *      This function is the main memory configuration function for DR DDR3
 *
 *      Requirements:
 *
 *      Run-Time Requirements:
 *      1. Complete Hypertransport Bus Configuration
 *      2. AmdMemInitDataStructDef must be run to set default values
 *      3. MSR bit to allow access to high PCI regs set on all nodes
 *      4. BSP in Big Real Mode
 *      5. Stack available
 *      6. MCG_CTL=-1, MC4_EN=0 for all CPUs
 *      7. MCi_STS from shutdown/warm reset recorded (if desired) prior to entry
 *      8. All var MTRRs reset to zero
 *      9. State of NB_CFG.DisDatMsk set properly on all CPUs
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *
 *     @return          AGESA_STATUS
 *                          - AGESA_ALERT
 *                          - AGESA_FATAL
 *                          - AGESA_SUCCESS
 *                          - AGESA_WARNING
 */
AGESA_STATUS
AmdMemAuto (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  MEM_SHARED_DATA  mmSharedData;
  MEM_MAIN_DATA_BLOCK mmData;
  MEM_NB_BLOCK *NBPtr;
  MEM_TECH_BLOCK *TechPtr;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  AGESA_STATUS Retval;
  UINT8 i;
  UINT8 Die;
  UINT8 DieCount;
  UINT8 Tab;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  ASSERT (MemPtr != NULL);

  AGESA_TESTPOINT (TpProcMemAmdMemAuto, &MemPtr->StdHeader);

  IDS_HDT_CONSOLE (MEM_FLOW, "MEM PARAMS:\n");
  IDS_HDT_CONSOLE (MEM_FLOW, "\tBottomIo : %04x\n", MemPtr->ParameterListPtr->BottomIo);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemHoleRemap : %d\n", MemPtr->ParameterListPtr->MemHoleRemapping);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tLimitBelow1TB : %d\n", MemPtr->ParameterListPtr->LimitMemoryToBelow1Tb);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tUserTimingMode : %d\n", MemPtr->ParameterListPtr->UserTimingMode);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemClockValue : %d\n", MemPtr->ParameterListPtr->MemClockValue);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tBankIntlv : %d\n", MemPtr->ParameterListPtr->EnableBankIntlv);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tNodeIntlv : %d\n", MemPtr->ParameterListPtr->EnableNodeIntlv);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tChannelIntlv : %d\n", MemPtr->ParameterListPtr->EnableChannelIntlv);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tEccFeature : %d\n", MemPtr->ParameterListPtr->EnableEccFeature);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tPowerDown : %d\n", MemPtr->ParameterListPtr->EnablePowerDown);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tOnLineSpare : %d\n", MemPtr->ParameterListPtr->EnableOnLineSpareCtl);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tParity : %d\n", MemPtr->ParameterListPtr->EnableParity);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tBankSwizzle : %d\n", MemPtr->ParameterListPtr->EnableBankSwizzle);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemClr : %d\n", MemPtr->ParameterListPtr->EnableMemClr);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tUmaMode : %d\n", MemPtr->ParameterListPtr->UmaMode);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tUmaSize : %d\n", MemPtr->ParameterListPtr->UmaSize);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tMemRestoreCtl : %d\n", MemPtr->ParameterListPtr->MemRestoreCtl);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tSaveMemContextCtl : %d\n", MemPtr->ParameterListPtr->SaveMemContextCtl);
  IDS_HDT_CONSOLE (MEM_FLOW, "\tExternalVrefCtl : %d\n", MemPtr->ParameterListPtr->ExternalVrefCtl );
  IDS_HDT_CONSOLE (MEM_FLOW, "\tForceTrainMode : %d\n\n", MemPtr->ParameterListPtr->ForceTrainMode );

  //----------------------------------------------------------------------------
  // Get TSC rate, which will be used later in Wait10ns routine
  //----------------------------------------------------------------------------
  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, &MemPtr->StdHeader);
  FamilySpecificServices->GetTscRate (FamilySpecificServices, &MemPtr->TscRate, &MemPtr->StdHeader);

  //----------------------------------------------------------------------------
  // Read In SPD Data
  //----------------------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemBeforeSpdProcessing, &MemPtr->StdHeader);
  MemSPDDataProcess (MemPtr);

  //----------------------------------------------------------------
  // Initialize Main Data Block
  //----------------------------------------------------------------
  mmData.MemPtr = MemPtr;
  mmData.mmSharedPtr = &mmSharedData;
  LibAmdMemFill (&mmSharedData, 0, sizeof (mmSharedData), &MemPtr->StdHeader);
  mmSharedData.DimmExcludeFlag = NORMAL;
  mmSharedData.NodeIntlv.IsValid = FALSE;
  //----------------------------------------------------------------
  // Discover populated CPUs
  //
  //----------------------------------------------------------------
  Retval = MemSocketScan (&mmData);
  if (Retval == AGESA_FATAL) {
    return Retval;
  }
  DieCount = mmData.DieCount;
  //----------------------------------------------------------------
  //
  // Allocate Memory for NB and Tech Blocks
  //
  //  NBPtr[Die]----+
  //                |
  //                V
  //  +---+---+---+---+---+---+---+---+
  //  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |   NB Blocks
  //  +---+---+---+---+---+---+---+---+
  //    |   |   |   |   |   |   |   |
  //    |   |   |   |   |   |   |   |
  //    v   v   v   v   v   v   v   v
  //  +---+---+---+---+---+---+---+---+
  //  | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |   Tech Blocks
  //  +---+---+---+---+---+---+---+---+
  //
  //
  //----------------------------------------------------------------
  AllocHeapParams.RequestedBufferSize = (DieCount * (sizeof (MEM_NB_BLOCK) + sizeof (MEM_TECH_BLOCK)));
  AllocHeapParams.BufferHandle = AMD_MEM_AUTO_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (AGESA_SUCCESS != HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader)) {
    ASSERT(FALSE); // NB and Tech Block Heap allocate error
    return AGESA_FATAL;
  }
  NBPtr = (MEM_NB_BLOCK *)AllocHeapParams.BufferPtr;
  TechPtr = (MEM_TECH_BLOCK *) (&NBPtr[DieCount]);
  mmData.NBPtr = NBPtr;
  mmData.TechPtr = TechPtr;

  //----------------------------------------------------------------
  // Create NB Blocks
  //
  //----------------------------------------------------------------
  for (Die = 0 ; Die < DieCount ; Die++ ) {
    i = 0;
    while (memNBInstalled[i].MemConstructNBBlock != 0) {
      if (memNBInstalled[i].MemConstructNBBlock (&NBPtr[Die], MemPtr, memNBInstalled[i].MemFeatBlock, &mmSharedData, Die) == TRUE) {
        break;
      }
      i++;
    }
    // Couldn't find a NB which supported this family
    if (memNBInstalled[i].MemConstructNBBlock == 0) {
      return AGESA_FATAL;
    }
  }
  //----------------------------------------------------------------
  // Create Technology Blocks
  //
  //----------------------------------------------------------------
  for (Die = 0 ; Die < DieCount ; Die++ ) {
    i = 0;
    while (memTechInstalled[i] != NULL) {
      if (memTechInstalled[i] (&TechPtr[Die], &NBPtr[Die])) {
        NBPtr[Die].TechPtr = &TechPtr[Die];
        break;
      }
      i++;
    }
    // Couldn't find a Tech block which supported this family
    if (memTechInstalled[i] == NULL) {
      return AGESA_FATAL;
    }
  }
  //----------------------------------------------------------------
  //
  //                 MEMORY INITIALIZATION TASKS
  //
  //----------------------------------------------------------------
  i = 0;
  while (memFlowControlInstalled[i] != NULL) {
    Retval = memFlowControlInstalled[i] (&mmData);
    if (MemPtr->IsFlowControlSupported == TRUE) {
      break;
    }
    i++;
  }

  //----------------------------------------------------------------
  // Deallocate NB register tables
  //----------------------------------------------------------------
  for (Tab = 0; Tab < NumberOfNbRegTables; Tab++) {
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_NB_REG_TABLE, Tab, 0, 0), &MemPtr->StdHeader);
  }

  //----------------------------------------------------------------
  // Check for errors and return
  //----------------------------------------------------------------
  AGESA_TESTPOINT (TpProcMemEnd, &MemPtr->StdHeader);
  for (Die = 0; Die < DieCount; Die++) {
    if (NBPtr[Die].MCTPtr->ErrCode > Retval) {
      Retval = NBPtr[Die].MCTPtr->ErrCode;
    }
  }
  return Retval;
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function fills a default SPD buffer with SPD values for all DIMMs installed in the system
 *
 *    The SPD Buffer is populated with a Socket-Channel-Dimm centric view of the Dimms.  At this
 *  point, the Memory controller type is not known, and the platform BIOS does not know the anything
 *  about which DIMM is on which DCT.  So the DCT relationship is abstracted from the arrangement
 *  of SPD information here.  We use the utility functions GetSpdSocketIndex(), GetMaxChannelsPerSocket(),
 *  and GetMaxDimmsPerChannel() to Map the SPD data according to which Socket-relative channel the DIMMs
 *  are connected to.  The functions rely on either the maximum values in the
 *  PlatformSpecificOverridingTable or if unspecified, the absolute maximums in AGESA.H.
 *
 *  This mapping is translated in the Northbridge object Constructor and the Technology block constructor.
 *
 *     @param[in,out]   *MemPtr   - Pointer to the MEM_DATA_STRUCT
 *
 */

VOID
STATIC
MemSPDDataProcess (
  IN OUT   MEM_DATA_STRUCT *MemPtr
  )
{
  UINT8 Socket;
  UINT8 Channel;
  UINT8 Dimm;
  UINT8 DimmIndex;
  UINT32 AgesaStatus;
  UINT8 MaxSockets;
  UINT8 MaxChannelsPerSocket;
  UINT8 MaxDimmsPerChannel;
  SPD_DEF_STRUCT *DimmSPDPtr;
  PSO_TABLE *PsoTable;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  AGESA_READ_SPD_PARAMS SpdParam;

  ASSERT (MemPtr != NULL);
  MaxSockets = (UINT8) (0x000000FF & GetPlatformNumberOfSockets ());
  PsoTable = MemPtr->ParameterListPtr->PlatformMemoryConfiguration;
  //
  // Allocate heap for the table
  //
  AllocHeapParams.RequestedBufferSize = (GetSpdSocketIndex (PsoTable, MaxSockets, &MemPtr->StdHeader) * sizeof (SPD_DEF_STRUCT));
  AllocHeapParams.BufferHandle = AMD_MEM_SPD_HANDLE;
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, &MemPtr->StdHeader) == AGESA_SUCCESS) {
    MemPtr->SpdDataStructure = (SPD_DEF_STRUCT *) AllocHeapParams.BufferPtr;
    //
    // Initialize SpdParam Structure
    //
    LibAmdMemCopy ((VOID *)&SpdParam, (VOID *)MemPtr, (UINTN)sizeof (SpdParam.StdHeader), &MemPtr->StdHeader);
    //
    // Populate SPDDataBuffer
    //
    SpdParam.MemData = MemPtr;
    DimmIndex = 0;
    for (Socket = 0; Socket < (UINT16)MaxSockets; Socket++) {
      MaxChannelsPerSocket = GetMaxChannelsPerSocket (PsoTable, Socket, &MemPtr->StdHeader);
      SpdParam.SocketId = Socket;
      for (Channel = 0; Channel < MaxChannelsPerSocket; Channel++) {
        SpdParam.MemChannelId = Channel;
        MaxDimmsPerChannel = GetMaxDimmsPerChannel (PsoTable, Socket, Channel);
        for (Dimm = 0; Dimm < MaxDimmsPerChannel; Dimm++) {
          SpdParam.DimmId = Dimm;
          DimmSPDPtr = &(MemPtr->SpdDataStructure[DimmIndex++]);
          SpdParam.Buffer = DimmSPDPtr->Data;
          AGESA_TESTPOINT (TpProcMemBeforeAgesaReadSpd, &MemPtr->StdHeader);
          AgesaStatus = AgesaReadSpd (0, &SpdParam);
          AGESA_TESTPOINT (TpProcMemAfterAgesaReadSpd, &MemPtr->StdHeader);
          if (AgesaStatus == AGESA_SUCCESS) {
            DimmSPDPtr->DimmPresent = TRUE;
            IDS_HDT_CONSOLE (MEM_FLOW, "SPD Socket %d Channel %d Dimm %d: %08x\n", Socket, Channel, Dimm, SpdParam.Buffer);
          } else {
            DimmSPDPtr->DimmPresent = FALSE;
          }
        }
      }
    }
  } else {
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_SPD, 0, 0, 0, 0, &MemPtr->StdHeader);
    //
    // Assert here if unable to allocate heap for SPDs
    //
    IDS_ERROR_TRAP;
  }
}
