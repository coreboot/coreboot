/* $NoKeywords:$ */
/**
 * @file
 *
 * mnphyc32.c
 *
 * Northbridge Phy support for C32
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/C32)
 * @e \$Revision: 36462 $ @e \$Date: 2010-08-20 00:49:49 +0800 (Fri, 20 Aug 2010) $
 *
 **/
/*
 *****************************************************************************
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
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnc32.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_C32_MNPHYC32_FILECODE
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
MemNInitPhyCompC32 (
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
  CurrDct = NBPtr->Dct;
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

  if ((NBPtr->ChannelPtr->Dimms == 3) &&
      ((NBPtr->DCTPtr->Timings.Speed == DDR800_FREQUENCY) ||
       (NBPtr->DCTPtr->Timings.Speed == DDR1066_FREQUENCY))) {
    //
    // Special Case for 3 Dimms @ 800MHz or 1066MHz
    //
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, 1);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, 1);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, 1);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, 1);
  } else {
    ASSERT (i <= 3);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0NCal, TableCompRiseSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp0PCal, TableCompFallSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2NCal, TableCompRiseSlew15x[i]);
    MemNSetBitFieldNb (NBPtr, BFD3Cmp2PCal, TableCompFallSlew15x[i]);
  }
  MemNSwitchDCTNb (NBPtr, CurrDct);
}
