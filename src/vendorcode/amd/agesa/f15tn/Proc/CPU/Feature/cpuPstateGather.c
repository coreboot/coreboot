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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
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
