/* $NoKeywords:$ */
/**
 * @file
 *
 * mfParallelTraining.c
 *
 * This is the parallel training feature
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/Feat/PARTRN)
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
#include "OptionMemory.h"
#include "mm.h"
#include "mn.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "mfParallelTraining.h"
#include "heapManager.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G2_PEI)
RDATA_GROUP (G2_PEI)

#define FILECODE PROC_MEM_FEAT_PARTRN_MFPARALLELTRAINING_FILECODE

/*-----------------------------------------------------------------------------
 *                                EXPORTED FUNCTIONS
 *
 *-----------------------------------------------------------------------------
 */
extern MEM_TECH_CONSTRUCTOR* memTechInstalled[];

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *    This is the main function to perform parallel training on all nodes.
 *    This is the routine which will run on the remote AP.
 *
 *     @param[in,out]   *EnvPtr   - Pointer to the Training Environment Data
 *     @param[in,out]   *StdHeader   - Pointer to the Standard Header of the AP
 *
 *     @return          TRUE -  This feature is enabled.
 *     @return          FALSE - This feature is not enabled.
 */
BOOLEAN
MemFParallelTraining (
  IN OUT   REMOTE_TRAINING_ENV *EnvPtr,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  MEM_PARAMETER_STRUCT ParameterList;
  MEM_NB_BLOCK NB;
  MEM_TECH_BLOCK TB;
  ALLOCATE_HEAP_PARAMS AllocHeapParams;
  MEM_DATA_STRUCT *MemPtr;
  DIE_STRUCT *MCTPtr;
  UINT8 p;
  UINT8 i;
  UINT8 Dct;
  UINT8 Channel;
  UINT8 *BufferPtr;
  UINT8 DctCount;
  UINT8 ChannelCount;
  UINT8 RowCount;
  UINT8 ColumnCount;
  UINT16 SizeOfNewBuffer;
  AP_DATA_TRANSFER ReturnData;

  //
  // Initialize Parameters
  //
  ReturnData.DataPtr = NULL;
  ReturnData.DataSizeInDwords = 0;
  ReturnData.DataTransferFlags = 0;

  ASSERT (EnvPtr != NULL);
  //
  // Replace Standard header of a AP
  //
  LibAmdMemCopy (StdHeader, &(EnvPtr->StdHeader), sizeof (AMD_CONFIG_PARAMS), &(EnvPtr->StdHeader));


  //
  //  Allocate buffer for training data
  //
  BufferPtr = (UINT8 *) (&EnvPtr->DieStruct);
  DctCount = EnvPtr->DieStruct.DctCount;
  BufferPtr += sizeof (DIE_STRUCT);
  ChannelCount = ((DCT_STRUCT *) BufferPtr)->ChannelCount;
  BufferPtr += DctCount * sizeof (DCT_STRUCT);
  RowCount = ((CH_DEF_STRUCT *) BufferPtr)->RowCount;
  ColumnCount = ((CH_DEF_STRUCT *) BufferPtr)->ColumnCount;

  SizeOfNewBuffer = sizeof (DIE_STRUCT) +
                    DctCount * (
                      sizeof (DCT_STRUCT) + (
                        ChannelCount * (
                          sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK) + (
                            RowCount * ColumnCount * NUMBER_OF_DELAY_TABLES +
                            (MAX_BYTELANES_PER_CHANNEL * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES) +
                            (MAX_DIMMS_PER_CHANNEL * MAX_NUMBER_LANES)
                          )
                        )
                      )
                    );
  AllocHeapParams.RequestedBufferSize = SizeOfNewBuffer;
  AllocHeapParams.BufferHandle = GENERATE_MEM_HANDLE (ALLOC_PAR_TRN_HANDLE, 0, 0, 0);
  AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
  if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
    BufferPtr = AllocHeapParams.BufferPtr;
    LibAmdMemCopy ( BufferPtr,
                    &(EnvPtr->DieStruct),
                    sizeof (DIE_STRUCT) + DctCount * (sizeof (DCT_STRUCT) + ChannelCount * (sizeof (CH_DEF_STRUCT) + sizeof (MEM_PS_BLOCK))),
                    StdHeader
                  );

    //
    //  Fix up pointers
    //
    MCTPtr = (DIE_STRUCT *) BufferPtr;
    BufferPtr += sizeof (DIE_STRUCT);
    MCTPtr->DctData = (DCT_STRUCT *) BufferPtr;
    BufferPtr += MCTPtr->DctCount * sizeof (DCT_STRUCT);
    for (Dct = 0; Dct < MCTPtr->DctCount; Dct++) {
      MCTPtr->DctData[Dct].ChData = (CH_DEF_STRUCT *) BufferPtr;
      BufferPtr += MCTPtr->DctData[Dct].ChannelCount * sizeof (CH_DEF_STRUCT);
      for (Channel = 0; Channel < MCTPtr->DctData[Dct].ChannelCount; Channel++) {
        MCTPtr->DctData[Dct].ChData[Channel].MCTPtr = MCTPtr;
        MCTPtr->DctData[Dct].ChData[Channel].DCTPtr = &MCTPtr->DctData[Dct];
      }
    }
    NB.PSBlock = (MEM_PS_BLOCK *) BufferPtr;
    BufferPtr += DctCount * ChannelCount * sizeof (MEM_PS_BLOCK);

    ReturnData.DataPtr = AllocHeapParams.BufferPtr;
    ReturnData.DataSizeInDwords = (SizeOfNewBuffer + 3) / 4;
    ReturnData.DataTransferFlags = 0;

    //
    // Allocate Memory for the MEM_DATA_STRUCT we will use
    //
    AllocHeapParams.RequestedBufferSize = sizeof (MEM_DATA_STRUCT);
    AllocHeapParams.BufferHandle = AMD_MEM_DATA_HANDLE;
    AllocHeapParams.Persist = HEAP_LOCAL_CACHE;
    if (HeapAllocateBuffer (&AllocHeapParams, StdHeader) == AGESA_SUCCESS) {
      MemPtr = (MEM_DATA_STRUCT *)AllocHeapParams.BufferPtr;

      LibAmdMemCopy (&(MemPtr->StdHeader), &(EnvPtr->StdHeader), sizeof (AMD_CONFIG_PARAMS), StdHeader);

      //
      // Copy Parameters from environment
      //
      ParameterList.HoleBase = EnvPtr->HoleBase;
      ParameterList.BottomIo = EnvPtr->BottomIo;
      ParameterList.UmaSize = EnvPtr->UmaSize;
      ParameterList.SysLimit = EnvPtr->SysLimit;
      ParameterList.TableBasedAlterations = EnvPtr->TableBasedAlterations;
      ParameterList.PlatformMemoryConfiguration = EnvPtr->PlatformMemoryConfiguration;
      MemPtr->ParameterListPtr = &ParameterList;

      for (p = 0; p < MAX_PLATFORM_TYPES; p++) {
        MemPtr->GetPlatformCfg[p] = EnvPtr->GetPlatformCfg[p];
      }

      MemPtr->ErrorHandling = EnvPtr->ErrorHandling;
      //
      // Create Local NBBlock and Tech Block
      //
      EnvPtr->NBBlockCtor (&NB, MCTPtr, EnvPtr->FeatPtr);
      NB.RefPtr = &ParameterList;
      NB.MemPtr = MemPtr;
      i = 0;
      while (memTechInstalled[i] != NULL) {
        if (memTechInstalled[i] (&TB, &NB)) {
          break;
        }
        i++;
      }
      NB.TechPtr = &TB;
      NB.TechBlockSwitch (&NB);

      //
      // Setup CPU Mem Type MSRs on the AP
      //
      NB.CpuMemTyping (&NB);

      IDS_HDT_CONSOLE (MEM_STATUS, "Node %d\n", NB.Node);
      //
      // Call Technology Specific Training routine
      //
      NB.TrainingFlow (&NB);
      //
      // Copy training data to ReturnData buffer
      //
      LibAmdMemCopy ( BufferPtr,
                      MCTPtr->DctData[0].ChData[0].RcvEnDlys,
                      ((DctCount * ChannelCount) * (
                         (RowCount * ColumnCount * NUMBER_OF_DELAY_TABLES) +
                         (MAX_BYTELANES_PER_CHANNEL * MAX_CS_PER_CHANNEL * NUMBER_OF_FAILURE_MASK_TABLES) +
                         (MAX_DIMMS_PER_CHANNEL * MAX_NUMBER_LANES)
                        )
                      ),
                      StdHeader);

      HeapDeallocateBuffer (AMD_MEM_DATA_HANDLE, StdHeader);
      //
      // Restore pointers
      //
      for (Dct = 0; Dct < MCTPtr->DctCount; Dct++) {
        for (Channel = 0; Channel < MCTPtr->DctData[Dct].ChannelCount; Channel++) {
          MCTPtr->DctData[Dct].ChData[Channel].MCTPtr = &EnvPtr->DieStruct;
          MCTPtr->DctData[Dct].ChData[Channel].DCTPtr = &EnvPtr->DieStruct.DctData[Dct];

          MCTPtr->DctData[Dct].ChData[Channel].RcvEnDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RcvEnDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDqsDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDqsDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDatDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDatDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqs2dDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqs2dDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsMinDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsMinDlys;
          MCTPtr->DctData[Dct].ChData[Channel].RdDqsMaxDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].RdDqsMaxDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDatMinDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDatMinDlys;
          MCTPtr->DctData[Dct].ChData[Channel].WrDatMaxDlys = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].WrDatMaxDlys;
          MCTPtr->DctData[Dct].ChData[Channel].FailingBitMask = EnvPtr->DieStruct.DctData[Dct].ChData[Channel].FailingBitMask;
        }
        MCTPtr->DctData[Dct].ChData = EnvPtr->DieStruct.DctData[Dct].ChData;
      }
      MCTPtr->DctData = EnvPtr->DieStruct.DctData;
    }

    //
    // Signal to BSP that training is complete and Send Results
    //
    ASSERT (ReturnData.DataPtr != NULL);
    ApUtilTransmitBuffer (EnvPtr->BspSocket, EnvPtr->BspCore, &ReturnData, StdHeader);

    //
    // Clean up and exit.
    //
    HeapDeallocateBuffer (GENERATE_MEM_HANDLE (ALLOC_PAR_TRN_HANDLE, 0, 0, 0), StdHeader);
  } else {
    MCTPtr = &EnvPtr->DieStruct;
    PutEventLog (AGESA_FATAL, MEM_ERROR_HEAP_ALLOCATE_FOR_TRAINING_DATA, MCTPtr->NodeId, 0, 0, 0, StdHeader);
    SetMemError (AGESA_FATAL, MCTPtr);
    ASSERT(FALSE); // Could not allocate heap for buffer for parallel training data
  }
  return TRUE;
}
