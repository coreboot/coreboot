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
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
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
#include "cpuApicUtilities.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_FEATURE_CPUPSTATEGATHER_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */
extern OPTION_PSTATE_POST_CONFIGURATION    OptionPstatePostConfiguration;  // global user config record

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
VOID
PStateGather (
  IN OUT   VOID *PStateBuffer,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  );
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
 *    Note:  This function should be called for every core in the system.
 *
 *  Parameters:
 *    @param[in]        *StdHeader
 *    @param[in, out]   *PStateStrucPtr
 *
 *    @retval      AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
AGESA_STATUS
PStateGatherData (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN OUT   S_CPU_AMD_PSTATE  *PStateStrucPtr
  )
{
  AGESA_TESTPOINT (TpProcCpuEntryPstateGather, StdHeader);
  return (*(OptionPstatePostConfiguration.PstateGather)) (StdHeader, PStateStrucPtr);
  // Note: Split config struct into PEI/DXE halves. This one is PEI.
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
  CPU_SPECIFIC_SERVICES   *FamilySpecificServices;
  UINT32                  MaxState;

  ASSERT (IsBsp (StdHeader, &IgnoredSts));

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  ASSERT (FamilySpecificServices != NULL);

  PopulatedSockets = 1;
  PStateBufferPtr = PStateStrucPtr->PStateLevelingStruc;

  NumberOfSockets = GetPlatformNumberOfSockets ();
  IdentifyCore (StdHeader, &BscSocket, &Ignored, &Ignored, &IgnoredSts);

  PStateStrucPtr->SizeOfBytes = sizeof (S_CPU_AMD_PSTATE);

  MaxState = 0;
  FamilySpecificServices->GetPstateMaxState (FamilySpecificServices, &MaxState, StdHeader);

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
  PStateBufferPtr->PStateLevelingSizeOfBytes = (UINT16) (sizeof (PSTATE_LEVELING) + (UINT32) (PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
  PStateStrucPtr->SizeOfBytes += (UINT32) (PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES));
  PStateBufferPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateBufferPtr + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
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
        PStateBufferPtr->PStateLevelingSizeOfBytes = (UINT16) (sizeof (PSTATE_LEVELING) + (UINT32) (PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
        PStateStrucPtr->SizeOfBytes += PStateBufferPtr->PStateLevelingSizeOfBytes;
        PStateBufferPtr = (PSTATE_LEVELING *) ((UINT8 *) PStateBufferPtr + (UINTN) sizeof (PSTATE_LEVELING) + (UINTN) (PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue * sizeof (S_PSTATE_VALUES)));
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
  PCI_ADDR               PciAddress;
  PSTATE_LEVELING        *PStateBufferPtr;
  BOOLEAN                PStateEnabled;
  CPU_SPECIFIC_SERVICES  *FamilySpecificServices;
  UINT32                 Socket;
  AGESA_STATUS           IgnoredSts;

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  ASSERT (FamilySpecificServices != NULL);

  PStateBufferPtr = (PSTATE_LEVELING *) PStateBuffer;
  TotalEnabledPStates = 0;
  PStateEnabled = FALSE;

  //
  /// Sockets number:  code looking at PStateBufferPtr->TotalCoresInNode
  ///        needs to know it is Processor (or socket) core count and NOT a Node Core count.
  GetActiveCoresInCurrentSocket (&CoreNum, StdHeader);
  PStateBufferPtr->TotalCoresInNode = (UINT8) CoreNum;

  //
  // Assume current CoreNum always zero.(BSC)
  //
  GetCurrentNodeAndCore (&NodeNum, &CoreNum, StdHeader);

  PStateBufferPtr->CreateAcpiTables = 1; /// @todo need remove

  //
  // We need to know the max pstate state in this socket.
  //
  FamilySpecificServices->GetPstateMaxState (FamilySpecificServices, &TempVar_c, StdHeader);
  PStateBufferPtr->PStateCoreStruct[0].PStateMaxValue = (UINT8) TempVar_c;

  for (k = 0; k <= TempVar_c; k++) {
    // Check if PState is enabled
    FamilySpecificServices->GetPstateRegisterInfo (FamilySpecificServices,
                                                   k,
                                                   &PStateEnabled,
                                                   &IddVal,
                                                   &IddDiv,
                                                   StdHeader);

    LibAmdMemFill (&(PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k]), 0, sizeof (S_PSTATE_VALUES), StdHeader);

    if (PStateEnabled) {

      FamilySpecificServices->GetPstateFrequency (
        FamilySpecificServices,
        (UINT8) k,
        &(PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].CoreFreq),
        StdHeader);

      FamilySpecificServices->GetPstatePower (
        FamilySpecificServices,
        (UINT8) k,
        &(PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].Power),
        StdHeader);

      PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].IddValue = IddVal;
      PStateBufferPtr->PStateCoreStruct[0].PStateStruct[k].IddDiv = IddDiv;

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
  GetPciAddress (StdHeader, Socket, NodeNum, &PciAddress, &IgnoredSts);
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

}


/*----------------------------------------------------------------------------
 *                              LOCAL FUNCTIONS
 *
 *----------------------------------------------------------------------------
 */


