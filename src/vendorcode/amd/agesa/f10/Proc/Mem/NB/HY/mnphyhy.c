/**
 * @file
 *
 * mnphyHy.c
 *
 * Northbridge Phy support for Hydra
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/HY)
 * @e \$Revision: 6789 $ @e \$Date: 2008-07-17 15:56:25 -0500 (Thu, 17 Jul 2008) $
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
#include "amdlib.h"
#include "Ids.h"
#include "ma.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "mnhy.h"
#include "OptionMemory.h"
#include "PlatformMemoryConfiguration.h"
#include "Filecode.h"
#define FILECODE PROC_MEM_NB_HY_MNPHYHY_FILECODE
/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
#define UNUSED_CLK 4

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



/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
/* -----------------------------------------------------------------------------*/


/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function initializes the DDR phy compensation logic
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNInitPhyCompHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT8 TableCompRiseSlew20x[] = {7, 3, 2, 2};
  CONST UINT8 TableCompRiseSlew15x[] = {7, 7, 3, 2};
  CONST UINT8 TableCompFallSlew20x[] = {7, 5, 3, 2};
  CONST UINT8 TableCompFallSlew15x[] = {7, 7, 5, 3};
  UINT8 i;
  UINT8 j;
  UINT8 CurrDct;
  UINT8 MaxDimmsPerChannel;
  UINT8 *DimmsPerChPtr;

  CurrDct = NBPtr->Dct;

  //
  // Get Platform Information
  //
  DimmsPerChPtr = FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_MAX_DIMMS, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID);
  if (DimmsPerChPtr != NULL) {
    MaxDimmsPerChannel = *DimmsPerChPtr;
  } else {
    MaxDimmsPerChannel = 2;
  }

  // 1. BIOS disables the phy compensation register by programming F2x9C_x08[DisAutoComp]=1
  // 2. BIOS waits 5 us for the disabling of the compensation engine to complete.
  // DisAutoComp will be cleared after Dram init has completed
  //
  MemNSwitchDCTNb (NBPtr, 0);
  MemNSetBitFieldNb (NBPtr, BFDisAutoComp, 1);
  MemUWait10ns (500, NBPtr->MemPtr);
  MemNSwitchDCTNb (NBPtr, CurrDct);

  // 3. For each normalized driver strength code read from
  // F2x[1, 0]9C_x00[AddrCmdDrvStren], program the
  // corresponding 3 bit predriver code in F2x9C_x0A[D3Cmp1NCal, D3Cmp1PCal].
  //
  // 4. For each normalized driver strength code read from
  // F2x[1, 0]9C_x00[DataDrvStren], program the corresponding
  // 3 bit predriver code in F2x9C_x0A[D3Cmp0NCal, D3Cmp0PCal, D3Cmp2NCal,
  // D3Cmp2PCal].
  //
  j = (UINT8) MemNGetBitFieldNb (NBPtr, BFAddrCmdDrvStren);
  i = (UINT8) MemNGetBitFieldNb (NBPtr, BFDataDrvStren);

  MemNSwitchDCTNb (NBPtr, 0);
  ASSERT (j <= 3);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp1NCal, TableCompRiseSlew20x[j]);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp1PCal, TableCompFallSlew20x[j]);

  ASSERT (i <= 3);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, TableCompRiseSlew15x[i]);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, TableCompFallSlew15x[i]);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, TableCompRiseSlew15x[i]);
  MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, TableCompFallSlew15x[i]);

  //
  // Special Case for certain configs
  //
  // 3DPCH Fully populated.
  if ((MaxDimmsPerChannel == 3) && (NBPtr->ChannelPtr->Dimms == 3)) {
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, 3);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, 5);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, 3);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, 5);
  }

  MemNSwitchDCTNb (NBPtr, CurrDct);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a general purpose function that executes before DRAM training
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDQSTrainingHy (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  UINT8 ChipSel;
  UINT32 TestAddrRJ16;
  UINT32 RealAddr;

  MemTBeginTraining (NBPtr->TechPtr);

  for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      for (ChipSel = 0; ChipSel < MAX_CS_PER_CHANNEL; ChipSel += 2) {
        if (MemNGetMCTSysAddrNb (NBPtr, ChipSel, &TestAddrRJ16)) {

          RealAddr = MemUSetUpperFSbase (TestAddrRJ16, NBPtr->MemPtr);

          MemUDummyCLRead (RealAddr);

          MemNSetBitFieldNb (NBPtr, BFErr350, 0x8000);
          MemUWait10ns (60, NBPtr->MemPtr);   // Wait 300ns
          MemNSetBitFieldNb (NBPtr, BFErr350, 0x0000);
          MemUWait10ns (400, NBPtr->MemPtr);  // Wait 2us
          MemUProcIOClFlush (TestAddrRJ16, 1, NBPtr->MemPtr);
          break;
        }
      }
    }
    if (NBPtr->IsSupported[CheckEccDLLPwrDnConfig]) {
      if (!NBPtr->MCTPtr->Status[SbEccDimms]) {
        MemNSetBitFieldNb (NBPtr, BFEccDLLPwrDnConf, 0x0010);
      }
      if (NBPtr->DCTPtr->Timings.Dimmx4Present == 0) {
        MemNSetBitFieldNb (NBPtr, BFEccDLLConf, 0x0080);
      }
    }
  }

  MemTEndTraining (NBPtr->TechPtr);

  MemNSetBitFieldNb (NBPtr, BFDisDatMsk, 1);
}
