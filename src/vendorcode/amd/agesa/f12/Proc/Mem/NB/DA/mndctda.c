/* $NoKeywords:$ */
/**
 * @file
 *
 * mndctda.c
 *
 * Northbridge DA DCT supporting functions
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: (Mem/NB/DA)
 * @e \$Revision: 44324 $ @e \$Date: 2010-12-22 17:16:51 +0800 (Wed, 22 Dec 2010) $
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
#include "OptionMemory.h"       // need def for MEM_FEAT_BLOCK_NB
#include "mnda.h"
#include "merrhdl.h"
#include "cpuRegisters.h"
#include "Filecode.h"
#include "GeneralServices.h"
#include "cpuFamilyTranslation.h"
#include "cpuRegisters.h"
#include "mport.h"
#include "F10PackageType.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_MEM_NB_DA_MNDCTDA_FILECODE
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
 *   This is a general purpose function that executes before DRAM init
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */

VOID
MemNBeforeDramInitDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;

  //Setting the reset value of Phy DLL standby and shutdown registers.
  for (Dct = 0; Dct < MAX_DCTS_PER_NODE_DA; Dct++) {
    MemNSwitchDCTNb (NBPtr, Dct);
    if (NBPtr->DCTPtr->Timings.DctMemSize != 0) {
      //Set PllLockTime and DllLockTime to default.
      MemNSetBitFieldNb (NBPtr, BFPhyPLLLockTime, 0x000007D0);
      MemNSetBitFieldNb (NBPtr, BFPhyDLLLockTime, 0x00000190);
      MemNSetBitFieldNb (NBPtr, BFDisDllShutdownSR, 1);

      //Clear PHY PLL Control Register before doing fence training and reset DLL
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D080F0C, 0x00002000);
      MemNSetBitFieldNb (NBPtr, BFPhyDLLControl, 0);
      MemNSetBitFieldNb (NBPtr, BFPhy0x0D080F0C, 0);
    }
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *      This function programs the memory controller with configuration parameters
 *
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 *     @return          TRUE - An Error value lower than AGESA_FATAL may have occurred
 *     @return          FALSE - An Error value greater than or equal to AGESA_FATAL may have occurred
 *     @return          NBPtr->MCTPtr->ErrCode - Contains detailed AGESA_STATUS value
 */

BOOLEAN
memNAutoConfigDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
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
    if (DCTPtr->Timings.Dimmx4Present && DCTPtr->Timings.Dimmx8Present) {
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
  MemNSetBitFieldNb (NBPtr, BFOdtSwizzle, 1);
  // For DDR3 Registered Dimms
  if (MCTPtr->Status[SbRegistered]) {
    if (MemNGetBitFieldNb (NBPtr, BFDdr3Mode)!= 0) {
      MemNSetBitFieldNb (NBPtr, BFSubMemclkRegDly, 1);
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
MemNSendMrsCmdDA (
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
 *   Workaround for erratum 322 and 263
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
VOID
MemNBeforePlatformSpecDA (
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
 *
 *   Change Average Value of 3
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
BOOLEAN
MemNChangeAvgValue3DA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C0) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
 *
 *
 *   Change Average Value of 8
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 *
 */
BOOLEAN
MemNChangeAvgValue8DA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  if ((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C1) != 0) {
    return TRUE;
  } else {
    return FALSE;
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
MemNEnDLLShutDownDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  UINT8 Dct;
  BOOLEAN DllShutDownEn;

  DllShutDownEn = TRUE;
  IDS_OPTION_HOOK (IDS_DLL_SHUT_DOWN, &DllShutDownEn, &(NBPtr->MemPtr->StdHeader));

  if (DllShutDownEn && NBPtr->IsSupported[SetDllShutDown]) {
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

/* -----------------------------------------------------------------------------*/
/**
 *
 *
 *   This function caps speed based on batter life check.
 *
 *     @param[in,out]   *NBPtr   - Pointer to the MEM_NB_BLOCK
 */
VOID
MemNCapSpeedBatteryLifeDA (
  IN OUT   MEM_NB_BLOCK *NBPtr
  )
{
  CONST UINT16 SupportedFreq[] = {
    DDR1600_FREQUENCY,
    DDR1333_FREQUENCY,
    DDR1066_FREQUENCY,
    DDR800_FREQUENCY,
    DDR667_FREQUENCY,
    DDR533_FREQUENCY,
    DDR400_FREQUENCY
  };
  UINT32 NBFreq;
  UINT8 j;
  UINT16 DdrFreq;
  CPU_SPECIFIC_SERVICES   *FamilySpecificServices;
  UINT32                  ProcessorPackageType;

  FamilySpecificServices = NULL;
  DdrFreq = DDR800_FREQUENCY; // Set Default to be 400Mhz
  ProcessorPackageType = LibAmdGetPackageType (&(NBPtr->MemPtr->StdHeader));
  GetCpuServicesOfSocket (NBPtr->MCTPtr->SocketId, &FamilySpecificServices, &(NBPtr->MemPtr->StdHeader));
  if (FamilySpecificServices->IsNbPstateEnabled (FamilySpecificServices, NBPtr->MemPtr->PlatFormConfig, &(NBPtr->MemPtr->StdHeader))) {
    NBFreq = (MemNGetBitFieldNb (NBPtr, BFNbFid) + 4) * 100;  // Calculate the Nb P1 frequency (NbFreq / 2)
    for (j = 0; j < GET_SIZE_OF (SupportedFreq); j++) {
      if (NBFreq >= ((UINT32) 2 * SupportedFreq[j])) {
        // Pick Max MEMCLK that is less than or equal to (NCLK_P1 / 2)
        DdrFreq = SupportedFreq[j];
        break;
      }
    }
    if (NBPtr->MemPtr->PlatFormConfig->PlatformProfile.PlatformPowerPolicy == BatteryLife) {
      if (NBPtr->DCTPtr->Timings.TargetSpeed > DdrFreq) {
        NBPtr->DCTPtr->Timings.TargetSpeed = DdrFreq;
      }
    } else {
      PutEventLog (AGESA_WARNING, MEM_WARNING_PERFORMANCE_ENABLED_BATTERY_LIFE_PREFERRED, NBPtr->Node, NBPtr->Dct, NBPtr->Channel, 0, &NBPtr->MemPtr->StdHeader);
      DdrFreq = DDR800_FREQUENCY; // Set Default to be 400Mhz
      NBFreq = (MemNGetBitFieldNb (NBPtr, BFNbFid) + 4) * 200;  // Calculate the Nb P0 frequency
      for (j = 0; j < GET_SIZE_OF (SupportedFreq); j++) {
        if (NBFreq >= ((UINT32) 2 * SupportedFreq[j])) {
          // Pick Max MEMCLK that is less than or equal to (NCLK_P0 / 2)
          DdrFreq = SupportedFreq[j];
          break;
        }
      }
      if (NBPtr->DCTPtr->Timings.TargetSpeed > DdrFreq) {
        NBPtr->DCTPtr->Timings.TargetSpeed = DdrFreq;
      }
    }
    if (((NBPtr->MCTPtr->LogicalCpuid.Revision & AMD_F10_C3) != 0) && (ProcessorPackageType == PACKAGE_TYPE_S1G3_S1G4 || ProcessorPackageType == PACKAGE_TYPE_ASB2)) {
      MemNSetBitFieldNb (NBPtr, BFRdPtrInit, 4);
      MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, 1);
    } else {
      MemNSetBitFieldNb (NBPtr, BFRdPtrInit, 6);
      MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, 0);
    }
  } else {
    NBFreq = (MemNGetBitFieldNb (NBPtr, BFNbFid) + 4) * 200;  // Calculate the Nb P0 frequency
    for (j = 0; j < GET_SIZE_OF (SupportedFreq); j++) {
      if (NBFreq >= ((UINT32) 2 * SupportedFreq[j])) {
        // Pick Max MEMCLK that is less than or equal to (NCLK_P0 / 2)
        DdrFreq = SupportedFreq[j];
        break;
      }
    }
    if (NBPtr->DCTPtr->Timings.TargetSpeed > DdrFreq) {
      NBPtr->DCTPtr->Timings.TargetSpeed = DdrFreq;
    }
    MemNSetBitFieldNb (NBPtr, BFRdPtrInit, 6);
    MemNSetBitFieldNb (NBPtr, BFDataTxFifoWrDly, 0);
  }
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *
/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
