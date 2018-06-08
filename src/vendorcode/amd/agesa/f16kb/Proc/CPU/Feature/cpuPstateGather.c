/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Pstate Data Gather Function.
 *
 * Contains code to collect all the Pstate related information from MSRs, and PCI registers.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
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
 ******************************************************************************
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
#include "cpuRegisters.h"
#include "cpuServices.h"
#include "GeneralServices.h"
#include "cpuPostInit.h"
#include "Ids.h"
#include "cpuFamilyTranslation.h"
#include "cpuPstateTables.h"
#include "cpuApicUtilities.h"
#include "cpuFeatures.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_FEATURE_CPUPSTATEGATHER_FILECODE

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
PStateGatherStub (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr
  );

AGESA_STATUS
PStateGatherMain (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr
  );


/*----------------------------------------------------------------------------
 *                            EXPORTED FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
VOID
PStateGather (
  IN OUT   VOID *PStateBuffer,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );

/**
 *---------------------------------------------------------------------------------------
 *
 *  PStateGatherData
 *
 *  Description:
 *    This function will gather PState information from the MSRs and fill up the
 *    pStateBuf. This buffer will be used by the PState Leveling, and PState Table
 *    generation code later.
 *
 *  Parameters:
 *    @param[in]        *PlatformConfig
 *    @param[in, out]   *PStateStrucPtr
 *    @param[in]        *StdHeader
 *
 *    @retval      AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateGatherData (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
  IN OUT   S_CPU_AMD_PSTATE       *PStateStrucPtr,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{


  AGESA_STATUS       AgesaStatus;

  AGESA_TESTPOINT (TpProcCpuEntryPstateGather, StdHeader);
  AgesaStatus = AGESA_SUCCESS;

  // Gather data for ACPI Tables if ACPI P-States/C-States object generation is enabled.
  if ((PlatformConfig->UserOptionPState) || (IsFeatureEnabled (IoCstate, PlatformConfig, StdHeader))) {
    AgesaStatus = (*(OptionPstatePostConfiguration.PstateGather)) (StdHeader, PStateStrucPtr);
    // Note: Split config struct into PEI/DXE halves. This one is PEI.
  }

  return AgesaStatus;
}

/**--------------------------------------------------------------------------------------
 *
 *  PStateGatherStub
 *
 *  Description:
 *     This is the default routine for use when the PState option is NOT requested.
 *      The option install process will create and fill the transfer vector with
 *      the address of the proper routine (Main or Stub). The link optimizer will
 *      strip out of the .DLL the routine that is not used.
 *
 *  Parameters:
 *    @param[in]        *StdHeader
 *    @param[in, out]   *PStateStrucPtr
 *
 *    @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateGatherStub (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr
  )
{
  return  AGESA_UNSUPPORTED;
}

/**--------------------------------------------------------------------------------------
 *
 *  PStateGatherMain
 *
 *  Description:
 *     This is the common routine for BSP gathering the Pstate data.
 *
 *  Parameters:
 *    @param[in]        *StdHeader
 *    @param[in, out]   *PStateStrucPtr
 *
 *    @retval         AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateGatherMain (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr
  )
{
  AP_TASK                 TaskPtr;
  UINT32                  BscSocket;
  UINT32                  Ignored;
  UINT32                  PopulatedSockets;
  UINT32                  NumberOfSockets;
  UINT32                  Socket;
  AGESA_STATUS            IgnoredSts;
  PSTATE_LEVELING         *PStateBufferPtr;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;
  UINT32                  MaxState;
  UINT8                   IgnoredByte;

  ASSERT (IsBsp (StdHeader, &IgnoredSts));

  FamilyServices = NULL;
  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  PopulatedSockets = 1;
  PStateBufferPtr = PStateStrucPtr->PStateLevelingStruc;

  NumberOfSockets = GetPlatformNumberOfSockets ();
  IdentifyCore (StdHeader, &BscSocket, &Ignored, &Ignored, &IgnoredSts);

  PStateStrucPtr->SizeOfBytes = sizeof (S_CPU_AMD_PSTATE);

  MaxState = 0;
  FamilyServices->GetPstateMaxState (FamilyServices, &MaxState, &IgnoredByte, StdHeader);

  TaskPtr.FuncAddress.PfApTaskI = PStateGather;
  //
  // Calculate max buffer size in dwords that need to pass to ap task.
  //
  TaskPtr.DataTransfer.DataSizeInDwords = (UINT16) ((MaxState + 1) * (SIZE_IN_DWORDS (S_PSTATE_VALUES)));
  TaskPtr.ExeFlags = WAIT_FOR_CORE;
  TaskPtr.DataTransfer.DataPtr = PStateBufferPtr;
  TaskPtr.DataTransfer.DataTransferFlags = DATA_IN_MEMORY;

  //
  //Get P-States and fill the PStateBufferPtr for BSP
  //
  ApUtilTaskOnExecutingCore (&TaskPtr, StdHeader, NULL);

  //
  //Calculate next node buffer address
  //
  PStateBufferPtr->SocketNumber = (UINT8) BscSocket;
  MaxState = PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue;
  PStateBufferPtr->PStateLevelingSizeOfBytes = (UINT16) (sizeof (PSTATE_LEVELING) + (MaxState + 1) * sizeof (S_PSTATE_VALUES));
  PStateStrucPtr->SizeOfBytes += (MaxState + 1) * sizeof (S_PSTATE_VALUES);
  PStateBufferPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateBufferPtr + PStateBufferPtr->PStateLevelingSizeOfBytes);
  CpuGetPStateLevelStructure (&PStateBufferPtr, PStateStrucPtr, 1, StdHeader);
  //
  //Get CPU P-States and fill the PStateBufferPtr for each node(BSC)
  //
  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (Socket != BscSocket) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        PopulatedSockets++;
        LibAmdMemFill (PStateBufferPtr, 0, sizeof (PSTATE_LEVELING), StdHeader);
        TaskPtr.DataTransfer.DataPtr = PStateBufferPtr;
        ApUtilRunCodeOnSocketCore ((UINT8)Socket, 0, &TaskPtr, StdHeader);
        PStateBufferPtr->SocketNumber = (UINT8) Socket;
        //
        //Calculate next node buffer address
        //
        MaxState = PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue;
        PStateBufferPtr->PStateLevelingSizeOfBytes = (UINT16) (sizeof (PSTATE_LEVELING) + (MaxState + 1) * sizeof (S_PSTATE_VALUES));
        PStateStrucPtr->SizeOfBytes += PStateBufferPtr->PStateLevelingSizeOfBytes;
        PStateBufferPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateBufferPtr + PStateBufferPtr->PStateLevelingSizeOfBytes);
      }
    }
  }
  PStateStrucPtr->TotalSocketInSystem = PopulatedSockets;

  return AGESA_SUCCESS;
}
/**--------------------------------------------------------------------------------------
 *
 *  PStateGather
 *
 *  Description:
 *     This is the common routine run on each BSC for gathering Pstate data.
 *
 *  Parameters:
 *    @param[in,out]    *PStateBuffer
 *    @param[in]        *StdHeader
 *
 *    @retval         VOID
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
PStateGather (
  IN OUT   VOID                *PStateBuffer,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32                 k;
  UINT32                 IddVal;
  UINT32                 IddDiv;
  UINT32                 NodeNum;
  UINT32                 CoreNum;
  UINT32                 TempVar_c;
  UINT32                 TotalEnabledPStates;
  UINT32                 SwPstate;
  UINT8                  BoostStates;
  PCI_ADDR               PciAddress;
  PSTATE_LEVELING        *PStateBufferPtr;
  BOOLEAN                PStateEnabled;
  PSTATE_CPU_FAMILY_SERVICES  *FamilyServices;
  UINT32                 Socket;
  AGESA_STATUS           IgnoredSts;
  CPUID_DATA             CpuId;

  PStateBufferPtr = (PSTATE_LEVELING *) PStateBuffer;
  TotalEnabledPStates = 0;
  FamilyServices = NULL;
  PStateEnabled = FALSE;

  GetFeatureServicesOfCurrentCore (&PstateFamilyServiceTable, (CONST VOID **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);

  //
  /// Sockets number:  code looking at PStateBufferPtr->TotalCoresInNode
  ///        needs to know it is Processor (or socket) core count and NOT a Node Core count.
  GetActiveCoresInCurrentSocket (&CoreNum, StdHeader);
  PStateBufferPtr->TotalCoresInNode = (UINT8) CoreNum;

  //
  // Assume current CoreNum always zero.(BSC)
  //
  GetCurrentNodeAndCore (&NodeNum, &CoreNum, StdHeader);

  PStateBufferPtr->CreateAcpiTables = 1;

  //
  // We need to know the max pstate state in this socket.
  //
  FamilyServices->GetPstateMaxState (FamilyServices, &TempVar_c, &BoostStates, StdHeader);
  PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue = (UINT8) TempVar_c;
  PStateBufferPtr->PStateCoreStruct[0].NumberOfBoostedStates = BoostStates;

  for (k = 0; k <= TempVar_c; k++) {
    // Check if PState is enabled
    FamilyServices->GetPstateRegisterInfo (        FamilyServices,
                                                   k,
                                                   &PStateEnabled,
                                                   &IddVal,
                                                   &IddDiv,
                                                   &SwPstate,
                                                   StdHeader);

    LibAmdMemFill (&(PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k]), 0, sizeof (S_PSTATE_VALUES), StdHeader);

    if (PStateEnabled) {
      FamilyServices->GetPstateFrequency (
        FamilyServices,
        (UINT8) k,
        &(PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].CoreFreq),
        StdHeader);

      FamilyServices->GetPstatePower (
        FamilyServices,
        (UINT8) k,
        &(PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].Power),
        StdHeader);

      PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].IddValue = IddVal;
      PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].IddDiv = IddDiv;
      PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].SwPstateNumber = SwPstate;

      PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].PStateEnable  = 1;
      TotalEnabledPStates++;
    }
  } // for (k = 0; k < MPPSTATE_MAXIMUM_STATES; k++)

  // Don't create ACPI Tables if there is one or less than one PState is enabled
  if (TotalEnabledPStates <= 1) {
    PStateBufferPtr[0].CreateAcpiTables = 0;
  }

  //--------------------Check Again--------------------------------

  IdentifyCore (StdHeader, &Socket, &NodeNum, &CoreNum, &IgnoredSts);
  // Get the PCI address of internal die 0 as it is the only die programmed.
  GetPciAddress (StdHeader, Socket, 0, &PciAddress, &IgnoredSts);
  PciAddress.Address.Function = FUNC_3;
  PciAddress.Address.Register = NORTH_BRIDGE_CAPABILITIES_REG;
  TempVar_c = 0;
  LibAmdPciRead (AccessWidth32, PciAddress, &TempVar_c, StdHeader);
  PStateBufferPtr->PStateCoreStruct[0].HtcCapable =
    (UINT8) ((TempVar_c & 0x00000400) >> 10);    // Bit  10

  TempVar_c = 0;
  PciAddress.Address.Register = HARDWARE_THERMAL_CTRL_REG;
  LibAmdPciRead (AccessWidth32, PciAddress, &TempVar_c, StdHeader);
  PStateBufferPtr->PStateCoreStruct[0].HtcPstateLimit =
    (UINT8) ((TempVar_c & 0x70000000) >> 28);   // Bits 30:28

  // Get LocalApicId from CPUID Fn0000_0001_EBX
  LibAmdCpuidRead (AMD_CPUID_APICID_LPC_BID, &CpuId, StdHeader);
  PStateBufferPtr->PStateCoreStruct[0].LocalApicId = (UINT8) ((CpuId.EBX_Reg & 0xFF000000) >> 24);
}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */
