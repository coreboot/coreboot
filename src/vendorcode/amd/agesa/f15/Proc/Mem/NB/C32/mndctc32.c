/* $NoKeywords:$ */
/**
 * @file
 *
 * mndctc32.c
 *
 * Northbridge DCT support for C32
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/C32)
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
#include "amdlib.h"
#include "Ids.h"
#include "mm.h"
#include "mn.h"
#include "mt.h"
#include "mu.h"
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnc32.h"
#include "merrhdl.h"
#include "cpuFamRegisters.h"
#include "PlatformMemoryConfiguration.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G2_PEI)
#define FILECODE PROC_MEM_NB_C32_MNDCTC32_FILECODE

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

extern BUILD_OPT_CFG UserOptions;

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function programs the memory controller with configuration parameters
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_ERROR may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_ERROR may have occurred
 *     @return          NBPtr->MCTPtr->ErrCode - Contains detailed AGESA_STATUS value
 */

BOOLEAN
MemNAutoConfigC32 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 PowerDownMode;
  UINT32 Value32;
  DIE_STRUCT *MCTPtr;
  DCT_STRUCT *DCTPtr;
  MEM_PARAMETER_STRUCT *RefPtr;

  RefPtr = NBPtr->RefPtr;
  MCTPtr = NBPtr->MCTPtr;
  DCTPtr = NBPtr->DCTPtr;
  //======================================================================
  // Build Dram Control Register Value (F2x78)
  //======================================================================
  //
  //It is recommended that these bits remain in the default state.
  //MemNSetBitFieldNb (NBPtr, BFRdPtrInit, 5);

  MemNSetBitFieldNb (NBPtr, BFEarlyArbEn, 1);

  //======================================================================
  // Build Dram Config Lo Register Value
  //======================================================================
  //

  if (MCTPtr->Status[SbParDimms]) {
    //
    // SbParDimms should be set for all DDR3 RDIMMS
    // Cannot turn off ParEn for DDR3
    //
    //@attention - add debug option for parity control
    MemNSetBitFieldNb (NBPtr, BFParEn, 1);
  }

  if (MCTPtr->GangedMode) {
    MemNSetBitFieldNb (NBPtr, BFWidth128, 1);
  }

  MemNSetBitFieldNb (NBPtr, BFX4Dimm, DCTPtr->Timings.Dimmx4Present & 0xF);

  if (!MCTPtr->Status[SbRegistered]) {
    MemNSetBitFieldNb (NBPtr, BFUnBuffDimm, 1);
  }

  if (MCTPtr->Status[SbEccDimms]) {
    MemNSetBitFieldNb (NBPtr, BFDimmEccEn, 1);
  }

  //======================================================================
  // Build Dram Config Hi Register Value
  //======================================================================
  //

  MemNSetBitFieldNb (NBPtr, BFMemClkFreq, MemNGetMemClkFreqIdNb (NBPtr, DCTPtr->Timings.Speed));

  if (MCTPtr->Status[SbRegistered]) {
    if ((DCTPtr->Timings.Dimmx4Present != 0) && (DCTPtr->Timings.Dimmx8Present != 0)) {
      MemNSetBitFieldNb (NBPtr, BFRDqsEn, 1);
    }
  }

  if (RefPtr->EnableBankSwizzle) {
    MemNSetBitFieldNb (NBPtr, BFBankSwizzleMode, 1);
  }

  if (DCTPtr->Timings.DimmQrPresent) {
    if (UserOptions.CfgMemoryQuadrankType == QUADRANK_UNBUFFERED) {
      MemNSetBitFieldNb (NBPtr, BFFourRankSoDimm, 1);
    } else if (UserOptions.CfgMemoryQuadrankType == QUADRANK_REGISTERED) {
      MemNSetBitFieldNb (NBPtr, BFFourRankRDimm, 1);
    }
  }

  MemNSetBitFieldNb (NBPtr, BFDcqBypassMax, 0xF);
  MemNSetBitFieldNb (NBPtr, BFDcqArbBypassEn, 1);
  //======================================================================
  // Build Dram Config Misc Register Value
  //======================================================================
  //
  if (MCTPtr->Status[SbRegistered]) {
    if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode)!= 0) {
      MemNSetBitFieldNb (NBPtr, BFSubMemclkRegDly, 1);
    }
  }
  //======================================================================
  // Build Dram Config Misc 2 Register Value
  //======================================================================
  //
  //
  // Ddr3FourSocketCh - Must be the same for both DCTs if either of them have > 2 Dimms
  //
  if ((GetMaxDimmsPerChannel (RefPtr->PlatformMemoryConfiguration, MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID)) > 2) {
    MemNBrdcstSetNb (NBPtr, BFDdr3FourSocketCh, 1);
  }
  //
  // DTaxTxFifpWrDly
  //
  Value32 = MemNGetBitFieldNb (NBPtr, BFRdPtrInit);
  if ((Value32 >= 2) && (Value32 <= 5)) {
    MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, (6 - Value32));
  }

  //
  // ProgOdtEn
  //
  if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode) == 1) {
    MemNSetBitFieldNb (NBPtr, BFProgOdtEn, 1);
  } else {
    MemNSetBitFieldNb (NBPtr, BFProgOdtEn, 0);
  }
  //
  // OdtSwizzle
  //
  if ((MemNGetBitFieldNb (NBPtr, BFDdr3Mode) == 0) && (MemNGetBitFieldNb (NBPtr, BFFourRankRDimm) == 0) && (RefPtr->EnablePowerDown)) {
    PowerDownMode = (UINT8) ((UserOptions.CfgPowerDownMode == POWER_DOWN_MODE_AUTO) ? POWER_DOWN_BY_CHANNEL : UserOptions.CfgPowerDownMode);
    IDS_OPTION_HOOK (IDS_POWERDOWN_MODE, &PowerDownMode, &(NBPtr->MemPtr->StdHeader));
    if (PowerDownMode == 1) {
      MemNSetBitFieldNb (NBPtr, BFOdtSwizzle, 1);
    }
  }


  return (BOOLEAN) (MCTPtr->ErrCode < AGESA_FATAL);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function sends an MRS command
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNSendMrsCmdC32 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  MemNSwapBitsNb (NBPtr);

  IDS_HDT_CONSOLE (MEM_FLOW, "\t\t\tCS%d MR%d %04x\n",
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 20) & 0xF,
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) >> 16) & 0xF,
              (MemNGetBitFieldNb (NBPtr, BFDramInitRegReg) & 0xFFFF));

  // 1.Set SendMrsCmd=1
  MemNSetBitFieldNb (NBPtr, BFSendMrsCmd, 1);

  // 2.Wait for SendMrsCmd=0
  MemNPollBitFieldNb (NBPtr, BFSendMrsCmd, 0, PCI_ACCESS_TIMEOUT, FALSE);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This is a general purpose function that executes before DRAM init
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDramInitC32 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  for (Dct = 0; Dct < NBPtr->DctCount; Dct ++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //Set PllLockTime and DllLockTime to default.
      MemNSetBitFieldNb (NBPtr, BFPhyPLLLockTime, 0x000007D0);
      MemNSetBitFieldNb (NBPtr, BFPhyDLLLockTime, 0x00000190);
      MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 1);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Enable DLL Shut down
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNEnDLLShutDownC32 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  BOOLEAN DllShutDownEn;

  DllShutDownEn = TRUE;
  IDS_OPTION_HOOK (IDS_DLL_SHUT_DOWN, &DllShutDownEn, &(NBPtr->MemPtr->StdHeader));

  if (DllShutDownEn && NBPtr->IsSupported[SetDllShutDown]) {
    if ((NBPtr->ChannelPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_D1) != 0) {
      for (Dct = 0; Dct < NBPtr->DctCount; Dct++) {
        MemNSwitchDCTNb (NBPtr, Dct);
        if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
          MemNSetBitFieldNb (NBPtr, BFPhyPLLLockTime, 0x0000001C);
          MemNSetBitFieldNb (NBPtr, BFPhyDLLLockTime, 0x0000013D);
          MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 0);
        }
      }
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   Workaround for erratum 322 and 263
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNBeforePlatformSpecC32 (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  // Errata 263
  if ((NBPtr->DCTPtr->Timings.Speed == DDR533_FREQUENCY) || (NBPtr->DCTPtr->Timings.Speed == DDR667_FREQUENCY)) {
    MemNSetBitFieldNb (NBPtr, BFErr263, 0x0800);
  } else {
    MemNSetBitFieldNb (NBPtr, BFErr263, 0);
  }

  // Errata 322
  // 1.Write 00000000h to F2x[1,0]9C_xD08E000
  MemNSetBitFieldNb (NBPtr, BFErr322I, 0);
  // 2.If DRAM Configuration Register[MemClkFreq] (F2x[1,0]94[2:0]) is
  //    greater than or equal to 011b (DDR-800 and higher),
  //    then write 00000080h to F2x[1,0]9C_xD02E001,
  //    else write 00000090h to F2x[1,0]9C_xD02E001.
  MemNSetBitFieldNb (NBPtr, BFErr322II, (NBPtr->DCTPtr->Timings.Speed >= DDR800_FREQUENCY) ? 0x80 : 0x90);
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Initializes extended MMIO address space
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   OptParam - Optional parameter
 *
 *     @return    TRUE
 */
BOOLEAN
MemNInitExtMMIOAddrC32  (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *OptParam
  )
{
  UINT8  Index;
  UINT32 Value;
  PCI_ADDR PciAddr;

  if (NBPtr->RefPtr->SysLimit >= _1TB_RJ16) {
    // Initialize all indices of F1x114_x2 and F1x114_x3.
    for (Index = 0; Index < 32; Index++) {
      PciAddr = NBPtr->PciAddr;
      PciAddr.Address.Function = 1;

      PciAddr.Address.Register = 0x110;
      Value = 0x20000000 | Index;
      LibAmdPciWrite (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);

      PciAddr.Address.Register = 0x114;
      Value = 0;
      LibAmdPciWrite (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);

      PciAddr.Address.Register = 0x110;
      Value = 0x30000000 | Index;
      LibAmdPciWrite (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);

      PciAddr.Address.Register = 0x114;
      Value = 0;
      LibAmdPciWrite (AccessWidth32, PciAddr, &Value, &NBPtr->MemPtr->StdHeader);
    }
  }
  return TRUE;
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *    Force LvDimm voltage to 1.5V for D0 part
 *
 *     @param[in,out]   *NBPtr       - Pointer to the MEM_NB_BLOCK
 *     @param[in,out]   *MemMainPtr  - Pointer to MEM_MAIN_DATA_BLOCK
 *
 *     @return    TRUE
 */
BOOLEAN
MemNForceLvDimmVoltageC32 (
  IN OUT   MEM_NB_BLOCK *NBPtr,
  IN OUT   VOID *MemMainPtr
  )
{
  MEM_PARAMETER_STRUCT *ParameterPtr;
  MEM_SHARED_DATA *mmSharedPtr;

  mmSharedPtr = ((MEM_MAIN_DATA_BLOCK *) MemMainPtr)->mmSharedPtr;
  ParameterPtr = ((MEM_MAIN_DATA_BLOCK *) MemMainPtr)->MemPtr->ParameterListPtr;

  if ((NBPtr->ChannelPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_D0) != 0) {
    IDS_HDT_CONSOLE (MEM_FLOW, "\n\nC32 D0 on socket %d.\n", NBPtr->MCTPtr->SocketId);
    if (((1 << CONVERT_VDDIO_TO_ENCODED (VOLT1_5)) & mmSharedPtr->VoltageMap) != 0) {
      IDS_HDT_CONSOLE (MEM_FLOW, "\nDimms are 1.5V capable. Adjust voltage to 1.5V.\n");
      ParameterPtr->DDR3Voltage = VOLT1_5;
    } else {
      if (FindPSOverrideEntry (NBPtr->RefPtr->PlatformMemoryConfiguration, PSO_LVDIMM_VOLT1_5_SUPPORT, NBPtr->MCTPtr->SocketId, NBPtr->ChannelPtr->ChannelID, 0, NULL, NULL)) {
        IDS_HDT_CONSOLE (MEM_FLOW, "\nDimms are not 1.5V capable. Adjust voltage to 1.5V based on customer's choice.\n\n");
        ParameterPtr->DDR3Voltage = VOLT1_5;
      } else {
        IDS_HDT_CONSOLE (MEM_FLOW, "\nDimms are not 1.5V capable. Do not adjust voltage based on customer's choice.\n\n");
        PutEventLog (AGESA_FATAL, MEM_ERROR_VDDIO_UNSUPPORTED, NBPtr->Node, 0, 0, 0, &(NBPtr->MemPtr->StdHeader));
        SetMemError (AGESA_FATAL, NBPtr->MCTPtr);
      }
    }
  }
  return TRUE;
}

/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
