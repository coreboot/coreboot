/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Pstate Leveling Function.
 *
 * Contains code to level the Pstates in a multi-socket system
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 35664 $   @e \$Date: 2010-07-28 20:02:15 +0800 (Wed, 28 Jul 2010) $
 *
 */
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
#include "OptionPstate.h"
#include "cpuLateInit.h"
#include "cpuRegisters.h"
#include "cpuPostInit.h"
#include "Ids.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuApicUtilities.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUPSTATELEVELING_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern OPTION_PSTATE_POST_CONFIGURATION    OptionPstatePostConfiguration;  // global user config record
extern CPU_FAMILY_SUPPORT_TABLE            PstateFamilyServiceTable;

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
PutAllCoreInPState0 (
  IN OUT   PSTATE_LEVELING    *PStateBufferPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

AGESA_STATUS
StartPstateMsrModify (
  IN       S_CPU_AMD_PSTATE    *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
STATIC
PutCoreInPState0 (
  IN       VOID   *PStateBuffer,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
PStateLevelingStub (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

AGESA_STATUS
PStateLevelingMain (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );

VOID
CorePstateRegModify (
  IN       VOID               *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  );


/**
 *---------------------------------------------------------------------------------------
 *
 *  PStateLeveling
 *
 *  Description:
 *    This function will populate the PStateBuffer, after doing the PState Leveling
 *    Note: This function should be called for every core in the system.
 *
 *  Parameters:
 *    @param[in,out]    *PStateStrucPtr
 *    @param[in]        *StdHeader
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateLeveling (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryPstateLeveling, StdHeader);
  return ((*(OptionPstatePostConfiguration.PstateLeveling)) (PStateStrucPtr, StdHeader));
  // Note: Split config struct into PEI/DXE halves. This one is PEI.
}

/**--------------------------------------------------------------------------------------
 *
 *  PStateLevelingStub
 *
 *  Description:
 *     This is the default routine for use when the PState option is NOT requested.
 *      The option install process will create and fill the transfer vector with
 *      the address of the proper routine (Main or Stub). The link optimizer will
 *      strip out of the .DLL the routine that is not used.
 *
 *  Parameters:
 *    @param[in,out]    *PStateStrucPtr
 *    @param[in]        *StdHeader
 *
 *    @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateLevelingStub (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return  AGESA_UNSUPPORTED;
}

/**--------------------------------------------------------------------------------------
 *
 *  PStateLevelingMain
 *
 *  Description:
 *     This is the common routine for creating the ACPI information tables.
 *
 *  Parameters:
 *    @param[in,out]    *PStateStrucPtr
 *    @param[in]        *StdHeader
 *
 *    @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateLevelingMain (
  IN OUT   S_CPU_AMD_PSTATE   *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                   i;
  UINT32                   k;
  UINT32                   m;
  UINT32                   TotalIterations;
  UINT32                   LogicalSocketCount;
  UINT32                   TempVar_a;
  UINT32                   TempVar_b;
  UINT32                   TempVar_c;
  UINT32                   TempVar_d;
  UINT32                   TempVar_e;
  UINT32                   TempVar_f;
  PCI_ADDR                 PciAddress;

  UINT32                   TempFreqArray[20];
  UINT32                   TempPowerArray[20];
  UINT32                   TempIddValueArray[20];
  UINT32                   TempIddDivArray[20];
  UINT32                   TempSocketPiArray[20];

  BOOLEAN                  TempFlag1;
  BOOLEAN                  TempFlag2;
  BOOLEAN                  TempFlag3;
  BOOLEAN                  TempFlag4;
  BOOLEAN                  AllCoresHaveHtcCapEquToZeroFlag;
  BOOLEAN                  AllCoreHaveMaxOnePStateFlag;
  BOOLEAN                  PstateMaxValEquToPstateHtcLimitFlag;
  BOOLEAN                  AtLeastOneCoreHasPstateHtcLimitEquToOneFlag;
  BOOLEAN                  PstateMaxValMinusHtcPstateLimitLessThan2Flag;
  PSTATE_LEVELING          *PStateBufferPtr;
  PSTATE_LEVELING          *PStateBufferPtrTmp;
  UINT32                   MaxPstateInNode;
  AGESA_STATUS             Status;

  TempFlag1 = FALSE;
  TempFlag2 = FALSE;
  TempFlag3 = FALSE;
  TempFlag4 = FALSE;
  AllCoresHaveHtcCapEquToZeroFlag = FALSE;
  AllCoreHaveMaxOnePStateFlag = FALSE;
  PstateMaxValEquToPstateHtcLimitFlag = FALSE;
  AtLeastOneCoreHasPstateHtcLimitEquToOneFlag = FALSE;
  PstateMaxValMinusHtcPstateLimitLessThan2Flag = FALSE;
  PStateBufferPtr = PStateStrucPtr->PStateLevelingStruc;
  Status = AGESA_SUCCESS;

  if (PStateBufferPtr[0].SetPState0 == PSTATE_FLAG_1) {
    PStateBufferPtr[0].AllCpusHaveIdenticalPStates = TRUE;
    PStateBufferPtr[0].InitStruct = 1;
    return AGESA_UNSUPPORTED;
  }

  LogicalSocketCount = PStateStrucPtr->TotalSocketInSystem;

  // This section of code will execute only for "core 0" i.e. BSP
  // Read P-States of all the cores.
  if (PStateBufferPtr[0].InitStruct == 0) {
    // Check if core frequency and power are same across all sockets.
    TempFlag1 = FALSE;
    for (i = 1; i < LogicalSocketCount; i++) {
      CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
      if ((PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue != PStateBufferPtr[0].PStateCoreStruct[0].PStateMaxValue)) {
        TempFlag1 = TRUE;
        break;
      }
      MaxPstateInNode = PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue;
      for (k = 0; k <= MaxPstateInNode; k++) {
        if ((PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[k].CoreFreq !=
             PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].CoreFreq) ||
            (PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[k].Power !=
             PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].Power)) {
          TempFlag1 = TRUE;
          break; // Come out of the inner FOR loop
        }
      }
      if (TempFlag1) {
        break; // Come out of the outer FOR loop
      }
    }

    if (!TempFlag1) {
      // No need to do pStateLeveling, or writing to pState MSR registers
      // if all CPUs have Identical PStates
      PStateBufferPtr[0].AllCpusHaveIdenticalPStates = TRUE;
      PStateBufferPtr[0].InitStruct = 1;
      PutAllCoreInPState0 (PStateBufferPtr, StdHeader);
      return AGESA_UNSUPPORTED;
    } else {
      PStateBufferPtr[0].AllCpusHaveIdenticalPStates = FALSE;
    }

    // 1_b) & 1_c)
    TempFlag1 = FALSE;
    TempFlag2 = FALSE;
    for (i = 0; i < LogicalSocketCount; i++) {
      CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
      if (PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue == 0) {
        TempFlag1 = TRUE;
      } else {
        TempFlag2 = TRUE;
      }
      if (PStateBufferPtrTmp->PStateCoreStruct[0].HtcCapable == 0) {
        TempFlag3 = TRUE;
      } else {
        TempFlag4 = TRUE;
      }

      if ((PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue -
           PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit) < 2) {
        PstateMaxValMinusHtcPstateLimitLessThan2Flag = TRUE;
      }

      if (PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue ==
          PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit) {
        PstateMaxValEquToPstateHtcLimitFlag = TRUE;
      }

      if (PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit == 1) {
        AtLeastOneCoreHasPstateHtcLimitEquToOneFlag = TRUE;
      }
    }

    // Do general setup of flags, that we may use later
    // Implementation of (1_b)
    if (TempFlag1 && TempFlag2) {
      //
      //Processors with only one enabled P-state (F3xDC[PstateMaxVal]=000b) cannot be mixed in a system with
      //processors with more than one enabled P-state (F3xDC[PstateMaxVal]!=000b).
      //
      PStateBufferPtr[0].InitStruct = 1;
      PStateBufferPtr[0].CreateAcpiTables = 0;
      PutAllCoreInPState0 (PStateBufferPtr, StdHeader);
      return AGESA_UNSUPPORTED;
    } else if (TempFlag1 && !TempFlag2) {
      //
      //all processors have only 1 enabled P-state
      //
      AllCoreHaveMaxOnePStateFlag = TRUE;
      PStateBufferPtr[0].OnlyOneEnabledPState = TRUE;
    }

    // Processors with F3xE8[HTC_CAPABLE] = 1 can not be
    // mixed in system with processors with F3xE8[HTC_CAPABLE] = 0.
    if (TempFlag3 && TempFlag4) {
      PStateBufferPtr[0].InitStruct = 1;
      PStateBufferPtr[0].CreateAcpiTables = 0;
      PutAllCoreInPState0 (PStateBufferPtr, StdHeader);
      return AGESA_UNSUPPORTED;
    }

    if (TempFlag3) {
      //
      //If code run to here means that all processors do not have HTC_CAPABLE.
      //
      AllCoresHaveHtcCapEquToZeroFlag = TRUE;
    }

    //--------------------------------------------------------------------------------
    // S T E P - 2
    //--------------------------------------------------------------------------------
    // Now run the PState Leveling Algorithm which will create mixed CPU P-State
    // Tables.
    // Follow the algorithm in the latest BKDG
    // -------------------------------------------------------------------------------
    // Match P0 CPU COF for all CPU cores to the lowest P0 CPU COF value in the
    // coherent fabric, and match P0 power for all CPU cores to the highest P0 power
    // value in the coherent fabric.
    // 2_a) If all processors have only 1 enabled P-State BIOS must write the
    //      appropriate CpuFid value resulting from the matched CPU COF to all
    //      copies of MSRC001_0070[CpuFid], and exit the sequence (No further
    //      steps are executed)
    //--------------------------------------------------------------------------------
    // Identify the lowest P0 Frequency and maximum P0 Power
    TempVar_d = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[0].CoreFreq;
    TempVar_e = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[0].Power;
    TempVar_a = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[0].IddValue;
    TempVar_b = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[0].IddDiv;

    for (i = 0; i < LogicalSocketCount; i++) {
      CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
      if (TempVar_d > PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].CoreFreq) {
        TempVar_d = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].CoreFreq;
      }

      if (TempVar_e < PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].Power) {
        TempVar_e = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].Power;
        TempVar_a = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].IddValue;
        TempVar_b = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].IddDiv;
      }
    }

    // Set P0 Frequency and Power for all CPUs
    for (i = 0; i < LogicalSocketCount; i++) {
      CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
      PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].CoreFreq = TempVar_d;
      PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].Power = TempVar_e;
      PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].IddValue = TempVar_a;
      PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[0].IddDiv = TempVar_b;
    }

    // 2_a)
    if (!AllCoreHaveMaxOnePStateFlag) {
      //--------------------------------------------------------------------------
      // STEP - 3
      //--------------------------------------------------------------------------
      // Match the CPU COF and power for P-states used by HTC. Skip to step 4
      // is any processor reports F3xE8[HTC_Capable] = 0;
      // 3_a) Set F3x64[HtcPstateLimit] = 001b and F3x68[StcPstateLimit] = 001b for
      //      processors with F3x64[HtcPstateLimit] = 000b.
      // 3_b) Identify the lowest CPU COF for all processors in the P-state
      //      pointed to by [The Hardware Thermal Control (HTC) Register]
      //      F3x64[HtcPstateLimit]
      // 3_c) Modify the CPU COF pointed to by [The Hardware Thermal Control
      //      (HTC) Register] F3x64[HtcPstateLimit] for all processors to the
      //      previously identified lowest CPU COF value.
      // 3_d) Identify the highest power for all processors in the P-state
      //      pointed to by [The Hardware Thermal Control (HTC) Register]
      //      F3x64[HtcPstateLimit].
      // 3_e) Modify the power pointed to by [The Hardware Thermal Control (HTC)
      //      Register] F3x64[HtcPstateLimit] to the previously identified
      //      highest power value.
      if (!AllCoresHaveHtcCapEquToZeroFlag) {
        // 3_a)
        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          if (PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit == 0) {
            // To Be Done (Set Htc and Stc PstateLimit values)
            // for this CPU (using PCI address space)
            for (k = 0; k < (UINT8)GetPlatformNumberOfModules (); k++) {
              if (GetPciAddress (StdHeader, PStateBufferPtrTmp->SocketNumber, k, &PciAddress, &Status)) {
                // Set F3x64[HtcPstateLimit] = 001b
                PciAddress.Address.Function = FUNC_3;
                PciAddress.Address.Register = HARDWARE_THERMAL_CTRL_REG;
                LibAmdPciRead (AccessWidth32, PciAddress, &TempVar_d, StdHeader);
                // Bits 30:28
                TempVar_d = (TempVar_d & 0x8FFFFFFF) | 0x10000000;
                LibAmdPciWrite (AccessWidth32, PciAddress, &TempVar_d, StdHeader);

                // Set F3x68[StcPstateLimit] = 001b
                PciAddress.Address.Register = SOFTWARE_THERMAL_CTRL_REG;
                LibAmdPciRead (AccessWidth32, PciAddress, &TempVar_d, StdHeader);
                // Bits 28:30
                TempVar_d = (TempVar_d & 0x8FFFFFFF) | 0x10000000;
                LibAmdPciWrite (AccessWidth32, PciAddress, &TempVar_d, StdHeader);
              }
            }
            // Set LocalBuffer
            PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit = 1;
            if ((PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue - 1) < 2) {
              PstateMaxValMinusHtcPstateLimitLessThan2Flag = TRUE;
            }

            if (PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue == 1) {
              PstateMaxValEquToPstateHtcLimitFlag = TRUE;
            }
          }

          if (PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit == 1) {
            AtLeastOneCoreHasPstateHtcLimitEquToOneFlag = TRUE;
          }
        }

        // 3_b) and 3_d)
        TempVar_a = PStateBufferPtr[0].PStateCoreStruct[0].HtcPstateLimit;
        TempVar_d = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].CoreFreq;
        TempVar_e = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].Power;
        TempVar_f = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].IddValue;
        TempVar_c = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].IddDiv;
        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          for (k = 0; k < 1; k++) {
            TempVar_b = PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit;
            if (TempVar_d > PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].CoreFreq) {
              TempVar_d = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].CoreFreq;
            }

            if (TempVar_e < PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].Power) {
              TempVar_e = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].Power;
              TempVar_f = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].IddValue;
              TempVar_c = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].IddDiv;
            }
          }
        }

        // 3_c) and 3_e)
        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          TempVar_a = PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit;
          PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].CoreFreq = TempVar_d;
          PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].Power = TempVar_e;
          PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].IddValue = TempVar_f;
          PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].IddDiv = TempVar_c;
        }
      } // if(AllCoresHaveHtcCapEquToZeroFlag)


      //--------------------------------------------------------------------------
      // STEP - 4
      //--------------------------------------------------------------------------
      // Match the CPU COF and power for the lowest performance P-state:
      // 4_a) If F3xDC[PstateMaxVal] = F3x64[HtcPstateLimit] for any processor,
      //      set PstateEn = 0 for all the P-states greater than
      //      F3x64[HtcPstateLimit] for all processors.
      // 4_b) Identify the lowest CPU COF for all processors in the P-state
      //      pointed to by F3xDC[PstateMaxVal].
      // 4_c) Modify the CPU COF for all processors in the P-state pointed to by
      //      F3xDC[PstateMaxVal] to the previously identified lowest CPU COF
      //      value.
      // 4_d) Identify the highest power for all processors in the P-state
      //      pointed to by F3xDC[PstateMaxVal].
      // 4_e) Modify the power for all processors in the P-state pointed to by
      //      F3xDC[PstateMaxVal] to the previously identified highest power
      //      value.

      // 4_a)
      if (PstateMaxValEquToPstateHtcLimitFlag) {
        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          TempVar_b = PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit + 1;
          for (k = TempVar_b; k <= PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue; k++) {
            PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].PStateEnable = 0;
          }
          //--------------------------------------------------------------------------
          // STEP - 5
          //--------------------------------------------------------------------------
          // 5_a) Modify F3xDC[PstateMaxVal] to indicate the lowest performance
          //      P-state with PstateEn set for each processor (Step 4 can disable
          //      P-states pointed to by F3xDC[PstateMaxVal])

          // Use this value of HtcPstateLimit to program the
          // F3xDC[pStateMaxValue]
          TempVar_e = PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit;
          TempVar_e <<= 8;
          // Bits 10:8

          for (m = 0; m < (UINT8)GetPlatformNumberOfModules (); m++) {
            if (GetPciAddress (StdHeader, PStateBufferPtrTmp->SocketNumber, m, &PciAddress, &Status)) {
              PciAddress.Address.Function = FUNC_3;
              PciAddress.Address.Register = CLOCK_POWER_TIMING_CTRL2_REG;
              LibAmdPciRead (AccessWidth32, PciAddress, &TempVar_d, StdHeader);
              TempVar_d = (TempVar_d & 0xFFFFF8FF) | TempVar_e;
              LibAmdPciWrite (AccessWidth32, PciAddress, &TempVar_d, StdHeader);
            }
          }//End of step 5
        }
      }// End of 4_a)

      // 4_b) and 4_d)
      TempVar_a = PStateBufferPtr[0].PStateCoreStruct[0].PStateMaxValue;
      TempVar_d = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].CoreFreq;
      TempVar_e = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].Power;
      TempVar_f = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].IddValue;
      TempVar_c = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempVar_a].IddDiv;

      for (i = 0; i < LogicalSocketCount; i++) {
        CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
        TempVar_b = PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue;
        if (TempVar_d >
            PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].CoreFreq) {
          TempVar_d =
          PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].CoreFreq;
        }

        if (TempVar_e < PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].Power) {
          TempVar_e = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].Power;
          TempVar_f = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].IddValue;
          TempVar_c = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_b].IddDiv;
        }
      }

      // 4_c) and 4_e)
      for (i = 0; i < LogicalSocketCount; i++) {
        CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
        TempVar_a = PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue;
        PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].CoreFreq = TempVar_d;
        PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].Power = TempVar_e;
        PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].IddValue = TempVar_f;
        PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempVar_a].IddDiv = TempVar_c;
      }


      //--------------------------------------------------------------------------
      // STEP - 6
      //--------------------------------------------------------------------------
      // Match the CPU COF and power for upper intermediate performance
      // P-state(s):
      // Upper intermediate PStates = PStates between (Not including) P0 and
      // F3x64[HtcPstateLimit]
      // 6_a) If F3x64[HtcPstateLimit] = 001b for any processor, set PstateEn = 0
      //      for enabled upper intermediate P-states for all processors with
      //      F3x64[HtcPstateLimit] > 001b and skip the remaining actions for
      //      this numbered step.
      // 6_b) Define each of the available upper intermediate P-states; for each
      //      processor concurrently evaluate the following loop; when any
      //      processor falls out of the loop (runs out of available upper
      //      intermediate Pstates) all other processors have their remaining
      //      upper intermediate P-states invalidated (PstateEn = 0);
      //      for (i = F3x64[HtcPstateLimit] - 1; i > 0; i--)
      //         - Identify the lowest CPU COF for P(i).
      //         - Identify the highest power for P(i).
      //         - Modify P(i) CPU COF for all processors to the previously
      //           identified lowest CPU COF value.
      //         - Modify P(i) power for all processors to the previously
      //           identified highest power value.

      // 6_a)
      if (AtLeastOneCoreHasPstateHtcLimitEquToOneFlag) {
        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          for (k = 1; k < (PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit); k++) {
            if (PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit > 1) {
              // Make a function call to clear the
              // structure values
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].PStateEnable = 0;
            }
          }
        }
      }
      // 6_b)
      else {
        // Identify Lowest Frequency and Highest Power
        TotalIterations = 0;
        TempFlag1 = TRUE;

        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          TempSocketPiArray[i] = PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit - 1;
        }

        do {
          //For first socket, try to find a candidate
          if (TempSocketPiArray[0] != 0) {
            while (PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].PStateEnable == 0) {
              TempSocketPiArray[0] = TempSocketPiArray[0] - 1;
              if (TempSocketPiArray[0] == 0) {
                TempFlag1 = FALSE;
                break;
              }
            }
          } else {
            TempFlag1 = FALSE;
          }
          if (TempFlag1) {
            TempFreqArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].CoreFreq;
            TempPowerArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].Power;
            TempIddValueArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].IddValue;
            TempIddDivArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].IddDiv;

            //Try to find next candidate
            for (i = 1; i < LogicalSocketCount; i++) {
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              if (TempSocketPiArray[i] != 0) {
                while (PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].PStateEnable == 0) {
                  TempSocketPiArray[i]--;
                  if (TempSocketPiArray[i] == 0) {
                    TempFlag1 = FALSE;
                    break;
                  }
                }//end while
              } else {
                TempFlag1 = FALSE;
              }

            } //end for LogicalSocketCount
          }

          if (TempFlag1) {
            for (i = 0; i < LogicalSocketCount; i++) {
              //
              //Compare
              //
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              if (TempFreqArray[TotalIterations] > PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].CoreFreq) {
                TempFreqArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].CoreFreq;
              }

              if (TempPowerArray[TotalIterations] < PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].Power) {
                TempPowerArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].Power;
                TempIddValueArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddValue;
                TempIddDivArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddDiv;
              }
            }
            // Modify (Pi) CPU COF and Power for all the CPUs
            for (i = 0; i < LogicalSocketCount; i++) {
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].CoreFreq = TempFreqArray[TotalIterations];
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].Power    = TempPowerArray[TotalIterations];
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddValue = TempIddValueArray[TotalIterations];
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddDiv   = TempIddDivArray[TotalIterations];
              TempSocketPiArray[i] = TempSocketPiArray[i] - 1;
            }
          } else {
            for (i = 0; i < LogicalSocketCount; i++) {
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              for (m = TempSocketPiArray[i]; m > 0; m--) {
                PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[m].PStateEnable = 0;
              }
            }
          }

          TotalIterations++;
        } while (TempFlag1);

      } // else

      //--------------------------------------------------------------------------
      // STEP - 7
      //--------------------------------------------------------------------------
      // Match the CPU COF and power for lower intermediate performance P - state(s)
      // Lower Intermediate Pstates = Pstates between (not including)
      // F3x64[HtcPstateLimit] and F3xDC[PstateMaxVal]
      // 7_a) If F3xDC[PstateMaxVal] - F3x64[HtcPstateLimit] < 2 for any
      //      processor, set PstateEn = 0 for enabled lower intermediate P - states
      //      for all processors with (F3xDC[PstateMaxVal] -
      //      F3x64[HtcPstateLimit] > 1) and skip the remaining actions for this
      //      numbered step.
      // 7_b) Define each of the available lower intermediate P-states; for each
      //      processor concurrently evaluate the following loop; when any
      //      processor falls out of the loop (runs out of available lower
      //      intermediate Pstates) all other processors have their remaining
      //      lower intermediate P-states invalidated (PstateEn = 0);
      //      for (i = F3xDC[PstateMaxVal]-1; i > F3x64[HtcPstateLimit]; i--)
      //         - Identify the lowest CPU COF for P-states between
      //           (not including) F3x64[HtcPstateLimit] and P(i).
      //         - Identify the highest power for P-states between
      //           (not including) F3x64[HtcPstateLimit] and P(i).
      //         - Modify P(i) CPU COF for all processors to the previously
      //           identified lowest CPU COF value.
      //         - Modify P(i) power for all processors to the previously
      //           identified highest power value.


      // 7_a)
      if (PstateMaxValMinusHtcPstateLimitLessThan2Flag) {
        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);

          for (k = PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue - 1;
              k > PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit;
              k--) {
            if ((PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue -
                 PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit) > 1) {
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[k].PStateEnable = 0;
            }
          }
        }
      }

      // 7_b)
      else {
        // Identify Lowest Frequency and Highest Power

        TotalIterations = 0;
        TempFlag1 = TRUE;

        for (i = 0; i < LogicalSocketCount; i++) {
          CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
          TempSocketPiArray[i] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateMaxValue - 1;
        }

        do {
          //For first socket, try to find a candidate
          if (TempSocketPiArray[0] != PStateBufferPtr[0].PStateCoreStruct[0].HtcPstateLimit) {
            while (PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].PStateEnable == 0) {
              TempSocketPiArray[0] = TempSocketPiArray[0] - 1;
              if (TempSocketPiArray[0] == PStateBufferPtr[0].PStateCoreStruct[0].HtcPstateLimit) {
                TempFlag1 = FALSE;
                break;
              }
            }
          } else {
            TempFlag1 = FALSE;
          }
          if (TempFlag1) {
            TempFreqArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].CoreFreq;
            TempPowerArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].Power;
            TempIddValueArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].IddValue;
            TempIddDivArray[TotalIterations] = PStateBufferPtr[0].PStateCoreStruct[0].PStateStruct[TempSocketPiArray[0]].IddDiv;

            //Try to find next candidate
            for (i = 1; i < LogicalSocketCount; i++) {
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              if (TempSocketPiArray[i] != PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit) {
                while (PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].PStateEnable == 0) {
                  TempSocketPiArray[i]--;
                  if (TempSocketPiArray[i] == PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit) {
                    TempFlag1 = FALSE;
                    break;
                  }
                }//end while
              } else {
                TempFlag1 = FALSE;
              }
            } //end for LogicalSocketCount
          }

          if (TempFlag1) {
            for (i = 0; i < LogicalSocketCount; i++) {
              //
              //Compare
              //
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              if (TempFreqArray[TotalIterations] > PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].CoreFreq) {
                TempFreqArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].CoreFreq;
              }
              if (TempPowerArray[TotalIterations] < PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].Power) {
                TempPowerArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].Power;
                TempIddValueArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddValue;
                TempIddDivArray[TotalIterations] = PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddDiv;
              }
            }
            // Modify (Pi) CPU COF and Power for all the CPUs
            for (i = 0; i < LogicalSocketCount; i++) {
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].CoreFreq = TempFreqArray[TotalIterations];
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].Power    = TempPowerArray[TotalIterations];
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddValue = TempIddValueArray[TotalIterations];
              PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[TempSocketPiArray[i]].IddDiv   = TempIddDivArray[TotalIterations];
              TempSocketPiArray[i] = TempSocketPiArray[i] - 1;
            }
          } else {
            for (i = 0; i < LogicalSocketCount; i++) {
              CpuGetPStateLevelStructure (&PStateBufferPtrTmp, PStateStrucPtr, i, StdHeader);
              for (m = TempSocketPiArray[i]; m > PStateBufferPtrTmp->PStateCoreStruct[0].HtcPstateLimit; m--) {
                PStateBufferPtrTmp->PStateCoreStruct[0].PStateStruct[m].PStateEnable = 0;
              }
            }
          }
          TotalIterations++;
        } while (TempFlag1);
      } // else
    } // if(!AllCoreHaveMaxOnePStateFlag)

    PStateBufferPtr[0].InitStruct = 1;
  } // CurrentCore


  // Update the pState MSRs
  // This can be done only by individual core
  StartPstateMsrModify (PStateStrucPtr, StdHeader);

  //----------------------------------------------------------------------------------
  // STEP - 8
  //----------------------------------------------------------------------------------
  // Place all cores into a valid COF and VID configuration corresponding to an
  // enabled P-state:
  // 8_a) Select an enabled P-state != to the P-state pointed to by
  //      MSRC001_0063[CurPstate] for each core.
  // 8_b) Transition all cores to the selected P-states by writing the Control value
  //      from the_PSS object corresponding to the selected P-state to
  //      MSRC001_0062[PstateCmd].
  // 8_c) Wait for all cores to report the Status value from the _PSS object
  //      corresponding to the selected P-state in MSRC001_0063[CurPstate].
  //
  PutAllCoreInPState0 (PStateBufferPtr, StdHeader);

  return AGESA_SUCCESS;
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  PutAllCoreInPState0
 *
 *  Description:
 *    This function will put core pstate to p0.
 *
 *  Parameters:
 *    @param[in,out]    *PStateBufferPtr
 *    @param[in]        *StdHeader
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PutAllCoreInPState0 (
  IN OUT   PSTATE_LEVELING    *PStateBufferPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  AP_TASK                 TaskPtr;
  UINT32                  BscSocket;
  UINT32                  Ignored;
  UINT32                  BscCoreNum;
  UINT32                  Core;
  UINT32                  Socket;
  UINT32                  NumberOfSockets;
  UINT32                  NumberOfCores;
  AGESA_STATUS            IgnoredSts;

  TaskPtr.FuncAddress.PfApTaskI = PutCoreInPState0;
  TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (PSTATE_LEVELING);
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  TaskPtr.DataTransfer.DataPtr = PStateBufferPtr;
  TaskPtr.DataTransfer.DataTransferFlags = DATA_IN_MEMORY;

  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  PutCoreInPState0 (PStateBufferPtr, StdHeader);

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      for (Core = 0; Core < NumberOfCores; Core++) {
        if ((Socket != (UINT32) BscSocket) || (Core != (UINT32) BscCoreNum)) {
          ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, &TaskPtr, StdHeader);
        }
      }
    }
  }

  return AGESA_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  CorePstateRegModify
 *
 *  Description:
 *    This function will setting the Pstate MSR to each APs base on Pstate Buffer.
 *    Note: This function should be called for every core in the system.
 *
 *  Parameters:
 *    @param[in,out]    *CpuAmdPState
 *    @param[in]        *StdHeader
 *
 *    @retval          VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
CorePstateRegModify (
  IN       VOID               *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  PSTATE_CPU_FAMILY_SERVICES   *FamilySpecificServices;
  FamilySpecificServices = NULL;

  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (const VOID **)&FamilySpecificServices, StdHeader);
  ASSERT (FamilySpecificServices != NULL)
  FamilySpecificServices->SetPStateLevelReg  (FamilySpecificServices, (S_CPU_AMD_PSTATE *) CpuAmdPState, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * This function will set msr on all cores of all nodes.
 *
 * @param[in]     CpuAmdPState  Pointer to S_CPU_AMD_PSTATE.
 * @param[in]     StdHeader     Header for library and services.
 *
 * @retval        AGESA_SUCCESS  Always succeeds
 *
 */
AGESA_STATUS
StartPstateMsrModify (
  IN       S_CPU_AMD_PSTATE    *CpuAmdPState,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  AP_TASK                 TaskPtr;
  UINT32                  BscSocket;
  UINT32                  Ignored;
  UINT32                  BscCoreNum;
  UINT32                  Core;
  UINT32                  Socket;
  UINT32                  NumberOfSockets;
  UINT32                  NumberOfCores;
  AGESA_STATUS            IgnoredSts;

  TaskPtr.FuncAddress.PfApTaskI = CorePstateRegModify;
  TaskPtr.DataTransfer.DataSizeInDwords = (UINT16) (CpuAmdPState->SizeOfBytes / 4 + 1);
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  TaskPtr.DataTransfer.DataPtr = CpuAmdPState;
  TaskPtr.DataTransfer.DataTransferFlags = DATA_IN_MEMORY;

  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  CorePstateRegModify (CpuAmdPState, StdHeader);

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &NumberOfCores, StdHeader)) {
      for (Core = 0; Core < NumberOfCores; Core++) {
        if ((Socket != (UINT32) BscSocket) || (Core != (UINT32) BscCoreNum)) {
          ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, &TaskPtr, StdHeader);
        }
      }
    }
  }

  return AGESA_SUCCESS;
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  CpuGetPStateLevelStructure
 *
 *  Description:
 *    Based on the LogicalSocketNumber, this function will return a pointer
 *      point to the accurate offset of the PSTATE_LEVELING structure.
 *
 *  Parameters:
 *    @param[in,out]          *PStateBufferPtr
 *    @param[in]              *CpuAmdPState
 *    @param[in]              LogicalSocketNumber
 *    @param[in]              *StdHeader
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
CpuGetPStateLevelStructure (
     OUT   PSTATE_LEVELING     **PStateBufferPtr,
  IN       S_CPU_AMD_PSTATE    *CpuAmdPState,
  IN       UINT32              LogicalSocketNumber,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  PSTATE_LEVELING         *PStateBufferPtrTmp;
  UINT32                  i;

  if (LogicalSocketNumber > CpuAmdPState->TotalSocketInSystem) {
    return AGESA_UNSUPPORTED;
  }

  PStateBufferPtrTmp = CpuAmdPState->PStateLevelingStruc;

  for (i = 1; i <= LogicalSocketNumber; i++) {
    PStateBufferPtrTmp = (PSTATE_LEVELING *) ((UINT8 *) PStateBufferPtrTmp + ((UINTN) PStateBufferPtrTmp->PStateLevelingSizeOfBytes));
  }

  *PStateBufferPtr = PStateBufferPtrTmp;

  return AGESA_SUCCESS;
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  PutCoreInPState0
 *
 *  Description:
 *    This function will take the CPU core into P0
 *
 *  Parameters:
 *    @param[in]         *PStateBuffer
 *    @param[in]         *StdHeader
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
STATIC
PutCoreInPState0 (
  IN       VOID   *PStateBuffer,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  CPU_SPECIFIC_SERVICES   *FamilySpecificServices;
  PSTATE_LEVELING         *PStateBufferPtr;

  PStateBufferPtr = (PSTATE_LEVELING *) PStateBuffer;

  if ((PStateBufferPtr[0].SetPState0 == PSTATE_FLAG_1 ) ||
     (PStateBufferPtr[0].SetPState0 == PSTATE_FLAG_2)) {
    return;
  }

  GetCpuServicesOfCurrentCore ((const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);

  FamilySpecificServices->TransitionPstate  (FamilySpecificServices, (UINT8) 0, (BOOLEAN) FALSE, StdHeader);
}
