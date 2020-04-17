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
 * @e \$Revision: 84150 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
 * Copyright (c) 2008 - 2013, Advanced Micro Devices, Inc.
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
#include "mtspd3.h"
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

#if (CONFIG(CPU_AMD_AGESA_OPENSOURCE_MEM_CUSTOM))

VOID
STATIC
AgesaCustomMemoryProfileSPD (
  IN OUT   UINT8 *Buffer
  );

#endif

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
  IDS_PERF_TIMESTAMP (TP_BEGINPROCAMDMEMAUTO, &MemPtr->StdHeader);

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
  IDS_HDT_CONSOLE (MEM_FLOW, "\tForceTrainMode : %d\n", MemPtr->ParameterListPtr->ForceTrainMode );
  IDS_HDT_CONSOLE (MEM_FLOW, "\tAMP : %d\n\n", MemPtr->ParameterListPtr->AmpEnable);

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
  // AMP initialization
  //
  //----------------------------------------------------------------
  for (Die = 0 ; Die < DieCount ; Die++ ) {
    NBPtr[Die].FeatPtr->InitAMP (&NBPtr[Die]);
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
  IDS_PERF_TIMESTAMP (TP_ENDPROCAMDMEMAUTO, &MemPtr->StdHeader);
  for (Die = 0; Die < DieCount; Die++) {
    if (NBPtr[Die].MCTPtr->ErrCode > Retval) {
      Retval = NBPtr[Die].MCTPtr->ErrCode;
    }
  }
  return Retval;
}

#if (CONFIG(CPU_AMD_AGESA_OPENSOURCE_MEM_CUSTOM))

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *  This function modifies a SPD buffer with the custom SPD values set up in coreboot's config.
 *
 *     @param[in,out]   *Buffer   - Pointer to the UINT8
 *
 */

VOID
STATIC
AgesaCustomMemoryProfileSPD (
  IN OUT   UINT8 *Buffer
  )
{
  UINT16 Offset;
  //
  // Modify a SPD buffer.
  //
  Buffer[SPD_DIVIDENT]   = CONFIG_CUSTOM_SPD_DIVIDENT;
  Buffer[SPD_DIVISOR]    = CONFIG_CUSTOM_SPD_DIVISOR;
  Buffer[SPD_TCK]        = CONFIG_CUSTOM_SPD_TCK;
  Buffer[SPD_CASLO]      = CONFIG_CUSTOM_SPD_CASLO;
  Buffer[SPD_CASHI]      = CONFIG_CUSTOM_SPD_CASHI;
  Buffer[SPD_TAA]        = CONFIG_CUSTOM_SPD_TAA;
  Buffer[SPD_TWR]        = CONFIG_CUSTOM_SPD_TWR;
  Buffer[SPD_TRCD]       = CONFIG_CUSTOM_SPD_TRCD;
  Buffer[SPD_TRRD]       = CONFIG_CUSTOM_SPD_TRRD;
  Buffer[SPD_TRP]        = CONFIG_CUSTOM_SPD_TRP;
  //
  // SPD_UPPER_TRC and SPD_UPPER_TRAS are the same index but different bits:
  // SPD_UPPER_TRC - [7:4], SPD_UPPER_TRAS - [3:0].
  //
  Buffer[SPD_UPPER_TRAS] = (CONFIG_CUSTOM_SPD_UPPER_TRC << 4) + CONFIG_CUSTOM_SPD_UPPER_TRAS;
  Buffer[SPD_TRAS]       = CONFIG_CUSTOM_SPD_TRAS;
  Buffer[SPD_TRC]        = CONFIG_CUSTOM_SPD_TRC;
  Buffer[SPD_TRFC_LO]    = CONFIG_CUSTOM_SPD_TRFC_LO;
  Buffer[SPD_TRFC_HI]    = CONFIG_CUSTOM_SPD_TRFC_HI;
  Buffer[SPD_TWTR]       = CONFIG_CUSTOM_SPD_TWTR;
  Buffer[SPD_TRTP]       = CONFIG_CUSTOM_SPD_TRTP;
  Buffer[SPD_UPPER_TFAW] = CONFIG_CUSTOM_SPD_UPPER_TFAW;
  Buffer[SPD_TFAW]       = CONFIG_CUSTOM_SPD_TFAW;
  //
  // Print a SPD buffer.
  //
  printk(BIOS_SPEW, "***** SPD BUFFER *****\n");
  for (Offset = 0; Offset < 256; Offset++) {
    printk(BIOS_SPEW, "Buffer[%d] = 0x%02X\n", Offset, Buffer[Offset]);
  }
  printk(BIOS_SPEW, "**********************\n");
}

#endif

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
            #if (CONFIG(CPU_AMD_AGESA_OPENSOURCE_MEM_CUSTOM))
                AgesaCustomMemoryProfileSPD(SpdParam.Buffer);
            #endif
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
