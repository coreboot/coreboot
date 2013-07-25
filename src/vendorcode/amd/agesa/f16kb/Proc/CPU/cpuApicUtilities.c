/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU APIC related utility functions.
 *
 * Contains code that provides mechanism to invoke and control APIC communication.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 84150 $   @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuCacheInit.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "heapManager.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUAPICUTILITIES_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/* ApFlags bits */
#define AP_TASK_HAS_INPUT    0x00000001ul
#define AP_TASK_HAS_OUTPUT   0x00000002ul
#define AP_RETURN_PARAMS     0x00000004ul
#define AP_END_AT_HLT        0x00000008ul
#define AP_PASS_EARLY_PARAMS 0x00000010ul

#define XFER_ELEMENT_SIZE    sizeof (UINT32)

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

typedef VOID F_CPU_AMD_NMI_HANDLER (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
typedef F_CPU_AMD_NMI_HANDLER *PF_CPU_AMD_NMI_HANDLER;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
ApUtilSetupIdtForHlt (
  IN       IDT_DESCRIPTOR *NmiIdtDescPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
STATIC
ApUtilRemoteRead (
  IN       UINT32 TargetApicId,
  IN       UINT8  RegAddr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
ApUtilLocalWrite (
  IN       UINT32 RegAddr,
  IN       UINT32 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT32
STATIC
ApUtilLocalRead (
  IN       UINT32 RegAddr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
ApUtilGetLocalApicBase (
     OUT   UINT64 *ApicBase,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

UINT8
STATIC
ApUtilCalculateUniqueId (
  IN      UINT8 Socket,
  IN      UINT8 Core,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
ApUtilFireDirectedNmi (
  IN      UINT32 TargetApicId,
  IN      AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
ApUtilReceivePointer (
  IN       UINT32 TargetApicId,
     OUT   VOID  **ReturnPointer,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
ApUtilTransmitPointer (
  IN       UINT32 TargetApicId,
  IN       VOID  **Pointer,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
STATIC
PerformFinalHalt (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
LocalApicInitialization (
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  );

VOID
LocalApicInitializationAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
extern
VOID
ExecuteHltInstruction (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

extern
VOID
NmiHandler (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

extern
VOID
ExecuteFinalHltInstruction (
  IN       UINT32 SharedCore,
  IN       AP_MTRR_SETTINGS  *ApMtrrSettingsList,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

extern BUILD_OPT_CFG UserOptions;

/*---------------------------------------------------------------------------------------*/
/**
 * Initialize the Local APIC.
 *
 * This function determines and programs the appropriate APIC ID value
 * for the executing core.  This code must be run after HT initialization
 * is complete.
 *
 *  @param[in]     CpuEarlyParamsPtr  Service parameters.
 *  @param[in]     StdHeader          Config handle for library and services.
 *
 */
VOID
LocalApicInitialization (
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParamsPtr,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT32    CurrentCore;
  UINT32    CurrentNodeNum;
  UINT32    CoreIdBits;
  UINT32    Mnc;
  UINT32    ProcessorCount;
  UINT32    ProcessorApicIndex;
  UINT32    IoApicNum;
  UINT32    StartLocalApicId;
  UINT64    LocalApicBase;
  UINT32    TempVar_a;
  UINT64    MsrData;
  UINT64    Address;
  CPUID_DATA  CpuidData;

  // Local variables default values
  IoApicNum = CpuEarlyParamsPtr->PlatformConfig.NumberOfIoApics;

  GetCurrentCore (&CurrentCore, StdHeader);
  GetCurrentNodeNum (&CurrentNodeNum, StdHeader);

  // Get Mnc
  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuidData, StdHeader);
  CoreIdBits = (CpuidData.ECX_Reg & 0x0000F000) >> 12;
  Mnc = 1 << (CoreIdBits & 0x000F);

  // Get ProcessorCount in the system
  ProcessorCount = GetNumberOfProcessors (StdHeader);

  // Get the APIC Index of this processor.
  ProcessorApicIndex = GetProcessorApicIndex (CurrentNodeNum, StdHeader);

  TempVar_a = (Mnc * ProcessorCount) + IoApicNum;
  ASSERT (TempVar_a < 255);

  // Apply apic enumeration rules
  // For systems with >= 16 APICs, put the IO-APICs at 0..n and
  // put the local-APICs at m..z
  // For systems with < 16 APICs, put the Local-APICs at 0..n and
  // put the IO-APICs at (n + 1)..z
  // This is needed because many IO-APIC devices only have 4 bits
  // for their APIC id and therefore must reside at 0..15
  StartLocalApicId = 0;
  if (TempVar_a >= 16) {
    if (IoApicNum >= 1) {
      StartLocalApicId = (IoApicNum - 1) / Mnc;
      StartLocalApicId = (StartLocalApicId + 1) * Mnc;
    }
  }

  // Set local apic id
  TempVar_a = (ProcessorApicIndex * Mnc) + CurrentCore + StartLocalApicId;
  IDS_HDT_CONSOLE (CPU_TRACE, "  Node %d core %d APIC ID = 0x%x\n", CurrentNodeNum, CurrentCore, TempVar_a);
  TempVar_a = TempVar_a << APIC20_ApicId;

  // Enable local apic id
  LibAmdMsrRead (MSR_APIC_BAR, &MsrData, StdHeader);
  MsrData |= APIC_ENABLE_BIT;
  LibAmdMsrWrite (MSR_APIC_BAR, &MsrData, StdHeader);

  // Get local apic base Address
  ApUtilGetLocalApicBase (&LocalApicBase, StdHeader);

  Address = LocalApicBase + APIC_ID_REG;
  LibAmdMemWrite (AccessWidth32, Address, &TempVar_a, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Initialize the Local APIC at the AmdInitEarly entry point.
 *
 * This function acts as a wrapper for calling the LocalApicInitialization
 * routine at AmdInitEarly.
 *
 *  @param[in]   FamilyServices      The current Family Specific Services.
 *  @param[in]   EarlyParams         Service parameters.
 *  @param[in]   StdHeader           Config handle for library and services.
 *
 */
VOID
LocalApicInitializationAtEarly (
  IN       CPU_SPECIFIC_SERVICES  *FamilyServices,
  IN       AMD_CPU_EARLY_PARAMS   *EarlyParams,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  AGESA_TESTPOINT (TpProcCpuLocalApicInit, StdHeader);
  LocalApicInitialization (EarlyParams, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Main entry point for all APs in the system.
 *
 * This routine puts the AP cores in an infinite loop in which the cores
 * will poll their masters, waiting to be told to perform a task.  At early,
 * all socket-relative core zeros will receive their tasks from the BSC.
 * All others will receive their tasks from the core zero of their local
 * processor.  At the end of AmdInitEarly, all cores will switch to receiving
 * their tasks from the BSC.
 *
 * @param[in]     StdHeader       Handle to config for library and services.
 * @param[in]     CpuEarlyParams  AMD_CPU_EARLY_PARAMS pointer.
 *
 */
VOID
ApEntry (
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       AMD_CPU_EARLY_PARAMS *CpuEarlyParams
  )
{
  UINT8   RemoteCmd;
  UINT8   SourceSocket;
  UINT8   CommandStart;
  UINT32  ApFlags;
  UINT32  FuncType;
  UINT32  ReturnCode;
  UINT32  CurrentSocket;
  UINT32  CurrentCore;
  UINT32  *InputDataPtr;
  UINT32  BscSocket;
  UINT32  Ignored;
  UINT32  TargetApicId;
  AP_FUNCTION_PTR FuncAddress;
  IDT_DESCRIPTOR IdtDesc[32];
  AP_DATA_TRANSFER DataTransferInfo;
  AGESA_STATUS IgnoredSts;

  ASSERT (!IsBsp (StdHeader, &IgnoredSts));

  // Initialize local variables
  ReturnCode = 0;
  DataTransferInfo.DataTransferFlags = 0;
  InputDataPtr = NULL;

  // Determine the executing core's socket and core numbers
  IdentifyCore (StdHeader, &CurrentSocket, &Ignored, &CurrentCore, &IgnoredSts);

  IDS_HDT_CONSOLE (CPU_TRACE, "  Socket %d core %d begin AP tasking engine\n", CurrentSocket, CurrentCore);

  // Determine the BSC's socket number
  GetSocketModuleOfNode ((UINT32) 0x00000000, &BscSocket, &Ignored, StdHeader);

  // Setup Interrupt Descriptor Table for sleep mode
  ApUtilSetupIdtForHlt (&IdtDesc[2], StdHeader);

  // Indicate to the BSC that we have reached the tasking engine
  ApUtilWriteControlByte (CORE_IDLE, StdHeader);

  if (CurrentCore == 0) {
    // Core 0s receive their tasks from the BSC
    SourceSocket = (UINT8) BscSocket;
  } else {
    // All non-zero cores receive their tasks from the core 0 of their socket
    SourceSocket = (UINT8) CurrentSocket;
  }

  GetLocalApicIdForCore (SourceSocket, 0, &TargetApicId, StdHeader);

  // Determine the unique value that the master will write when it has a task
  // for this core to perform.
  CommandStart = ApUtilCalculateUniqueId (
                   (UINT8)CurrentSocket,
                   (UINT8)CurrentCore,
                   StdHeader
                   );
  for (;;) {
    RemoteCmd = ApUtilReadRemoteControlByte (TargetApicId, StdHeader);
    if (RemoteCmd == CommandStart) {
      ApFlags = ApUtilReadRemoteDataDword (TargetApicId, StdHeader);

      ApUtilReceivePointer (TargetApicId, (VOID **) &FuncAddress, StdHeader);

      FuncType = ApFlags & (UINT32) (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT | AP_PASS_EARLY_PARAMS);
      if ((ApFlags & AP_TASK_HAS_INPUT) != 0) {
        DataTransferInfo.DataSizeInDwords = 0;
        DataTransferInfo.DataPtr = NULL;
        DataTransferInfo.DataTransferFlags = 0;
        if (ApUtilReceiveBuffer (SourceSocket, 0, &DataTransferInfo, StdHeader) == AGESA_ERROR) {
          // There is not enough space to put the input data on the heap.  Undefined behavior is about
          // to result.
          IDS_ERROR_TRAP;
        }
        InputDataPtr = (UINT32 *) DataTransferInfo.DataPtr;
      }
      ApUtilWriteControlByte (CORE_ACTIVE, StdHeader);
      switch (FuncType) {
      case 0:
        FuncAddress.PfApTask (StdHeader);
        break;
      case AP_TASK_HAS_INPUT:
        FuncAddress.PfApTaskI (InputDataPtr, StdHeader);
        break;
      case AP_PASS_EARLY_PARAMS:
        FuncAddress.PfApTaskC (StdHeader, CpuEarlyParams);
        break;
      case (AP_TASK_HAS_INPUT | AP_PASS_EARLY_PARAMS):
        FuncAddress.PfApTaskIC (InputDataPtr, StdHeader, CpuEarlyParams);
        break;
      case AP_TASK_HAS_OUTPUT:
        ReturnCode = FuncAddress.PfApTaskO (StdHeader);
        break;
      case (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT):
        ReturnCode = FuncAddress.PfApTaskIO (InputDataPtr, StdHeader);
        break;
      case (AP_TASK_HAS_OUTPUT | AP_PASS_EARLY_PARAMS):
        ReturnCode = FuncAddress.PfApTaskOC (StdHeader, CpuEarlyParams);
        break;
      case (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT | AP_PASS_EARLY_PARAMS):
        ReturnCode = FuncAddress.PfApTaskIOC (InputDataPtr, StdHeader, CpuEarlyParams);
        break;
      default:
        ReturnCode = 0;
        break;
      }
      if (((ApFlags & AP_RETURN_PARAMS) != 0)) {
        ApUtilTransmitBuffer (SourceSocket, 0, &DataTransferInfo, StdHeader);
      }
      if ((ApFlags & AP_TASK_HAS_OUTPUT) != 0) {
        ApUtilWriteDataDword (ReturnCode, StdHeader);
      }
      if ((ApFlags & AP_END_AT_HLT) != 0) {
        RemoteCmd = CORE_IDLE_HLT;
      } else {
        ApUtilWriteControlByte (CORE_IDLE, StdHeader);
      }
    }
    if (RemoteCmd == CORE_IDLE_HLT) {
      SourceSocket = (UINT8) BscSocket;
      GetLocalApicIdForCore (SourceSocket, 0, &TargetApicId, StdHeader);
      ApUtilWriteControlByte (CORE_IDLE_HLT, StdHeader);
      ExecuteHltInstruction (StdHeader);
      ApUtilWriteControlByte (CORE_IDLE, StdHeader);
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Reads the 'control byte' on the designated remote core.
 *
 * This function will read the current contents of the control byte
 * on the designated core using the APIC remote read inter-
 * processor interrupt sequence.
 *
 * @param[in]      TargetApicId  Local APIC ID of the desired core
 * @param[in]      StdHeader     Configuration parameters pointer
 *
 * @return         The current value of the remote cores control byte
 *
 */
UINT8
ApUtilReadRemoteControlByte (
  IN       UINT32 TargetApicId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  ControlByte;
  UINT32 ApicRegister;

  ApicRegister = ApUtilRemoteRead (TargetApicId, APIC_CTRL_DWORD, StdHeader);
  ControlByte = (UINT8) ((ApicRegister & APIC_CTRL_MASK) >> APIC_CTRL_SHIFT);
  return (ControlByte);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Writes the 'control byte' on the executing core.
 *
 * This function writes data to a local APIC offset used in inter-
 * processor communication.
 *
 * @param[in]       Value
 * @param[in]       StdHeader
 *
 */
VOID
ApUtilWriteControlByte (
  IN       UINT8 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 ApicRegister;

  ApicRegister = ApUtilLocalRead (APIC_CTRL_REG, StdHeader);
  ApicRegister = ((ApicRegister & ~APIC_CTRL_MASK) | (UINT32) (Value << APIC_CTRL_SHIFT));
  ApUtilLocalWrite (APIC_CTRL_REG, ApicRegister, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Reads the 'data dword' on the designated remote core.
 *
 * This function will read the current contents of the data dword
 * on the designated core using the APIC remote read inter-
 * processor interrupt sequence.
 *
 * @param[in]      TargetApicId  Local APIC ID of the desired core
 * @param[in]      StdHeader     Configuration parameters pointer
 *
 * @return         The current value of the remote core's data dword
 *
 */
UINT32
ApUtilReadRemoteDataDword (
  IN       UINT32 TargetApicId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  return (ApUtilRemoteRead (TargetApicId, APIC_DATA_DWORD, StdHeader));
}


/*---------------------------------------------------------------------------------------*/
/**
 * Writes the 'data dword' on the executing core.
 *
 * This function writes data to a local APIC offset used in inter-
 * processor communication.
 *
 * @param[in]      Value        Value to write
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 */
VOID
ApUtilWriteDataDword (
  IN       UINT32 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  ApUtilLocalWrite (APIC_DATA_REG, Value, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Runs the given task on the specified local core.
 *
 * This function is used to invoke an AP to run a specified AGESA
 * procedure.  It can only be called by cores that have subordinate
 * APs -- the BSC at POST, or any socket-relative core 0s at Early.
 *
 * @param[in]      Socket       Socket number of the target core
 * @param[in]      Core         Core number of the target core
 * @param[in]      TaskPtr      Function descriptor
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 * @return         Return value of the task that the AP core ran,
 *                 or zero if the task was VOID.
 *
 */
UINT32
ApUtilRunCodeOnSocketCore (
  IN       UINT8   Socket,
  IN       UINT8   Core,
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  CoreId;
  UINT8  CurrentStatus;
  UINT8  WaitStatus[3];
  UINT32 ApFlags;
  UINT32 ReturnCode;
  UINT32 TargetApicId;
  AP_WAIT_FOR_STATUS WaitForStatus;

  ApFlags = 0;
  ReturnCode = 0;

  CoreId = ApUtilCalculateUniqueId (Socket, Core, StdHeader);

  GetLocalApicIdForCore (Socket, Core, &TargetApicId, StdHeader);

  if (TaskPtr->DataTransfer.DataSizeInDwords != 0) {
    ApFlags |= AP_TASK_HAS_INPUT;
    if (((TaskPtr->ExeFlags & RETURN_PARAMS) != 0) &&
        ((TaskPtr->DataTransfer.DataTransferFlags & DATA_IN_MEMORY) == 0)) {
      ApFlags |= AP_RETURN_PARAMS;
    }
  }

  if ((TaskPtr->ExeFlags & TASK_HAS_OUTPUT) != 0) {
    ApFlags |= AP_TASK_HAS_OUTPUT;
  }

  if ((TaskPtr->ExeFlags & END_AT_HLT) != 0) {
    ApFlags |= AP_END_AT_HLT;
  }

  if ((TaskPtr->ExeFlags & PASS_EARLY_PARAMS) != 0) {
    ApFlags |= AP_PASS_EARLY_PARAMS;
  }

  WaitStatus[0] = CORE_IDLE;
  WaitStatus[1] = CORE_IDLE_HLT;
  WaitStatus[2] = CORE_UNAVAILABLE;
  WaitForStatus.Status = WaitStatus;
  WaitForStatus.NumberOfElements = 3;
  WaitForStatus.RetryCount = WAIT_INFINITELY;
  WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;
  CurrentStatus = ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);

  if (CurrentStatus != CORE_UNAVAILABLE) {
    ApUtilWriteDataDword (ApFlags, StdHeader);
    ApUtilWriteControlByte (CoreId, StdHeader);

    if (CurrentStatus == CORE_IDLE_HLT) {
      ApUtilFireDirectedNmi (TargetApicId, StdHeader);
    }

    ApUtilTransmitPointer (TargetApicId, (VOID **) &TaskPtr->FuncAddress, StdHeader);

    if ((ApFlags & AP_TASK_HAS_INPUT) != 0) {
      ApUtilTransmitBuffer (Socket, Core, &TaskPtr->DataTransfer, StdHeader);
    }

    if ((TaskPtr->ExeFlags & WAIT_FOR_CORE) != 0) {
      if (((ApFlags & AP_TASK_HAS_INPUT) != 0) &&
          ((ApFlags & AP_RETURN_PARAMS) != 0) &&
          ((TaskPtr->DataTransfer.DataTransferFlags & DATA_IN_MEMORY) == 0)) {
        if (ApUtilReceiveBuffer (Socket, Core, &TaskPtr->DataTransfer, StdHeader) == AGESA_ERROR) {
          // There is not enough space to put the return data.  This should never occur.  If it
          // does, this would point to strange heap corruption.
          IDS_ERROR_TRAP;
        }
      }

      ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
      if ((ApFlags & AP_TASK_HAS_OUTPUT) != 0) {
        ReturnCode = ApUtilReadRemoteDataDword (TargetApicId, StdHeader);
      }
    }
  } else {
    ReturnCode = 0;
  }
  return (ReturnCode);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Waits for a remote core's control byte value to either be equal or
 * not equal to any number of specified values.
 *
 * This function will loop doing remote read IPIs until the remote core's
 * control byte becomes one of the values in the input array if the input
 * flags are set for equality.  Otherwise, the loop will continue until
 * the control byte value is not equal to one of the elements in the
 * array.  The caller can also specify an iteration count for timeout
 * purposes.
 *
 * @param[in]      TargetApicId   Local APIC ID of the desired core
 * @param[in]      WaitParamsPtr  Wait parameter structure
 * @param[in]      StdHeader      Configuration parameteres pointer
 *
 * @return         The current value of the remote core's control byte
 *
 */
UINT8
ApUtilWaitForCoreStatus (
  IN       UINT32 TargetApicId,
  IN       AP_WAIT_FOR_STATUS *WaitParamsPtr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  BOOLEAN  IsEqual;
  UINT8 CoreStatus;
  UINT8 i;
  UINT8 j;

  CoreStatus = 0;
  for (i = 0; (WaitParamsPtr->RetryCount == WAIT_INFINITELY) ||
              (i < WaitParamsPtr->RetryCount); ++i) {
    CoreStatus = ApUtilReadRemoteControlByte (TargetApicId, StdHeader);
    // Determine whether or not the current remote status is equal
    // to an element in the array.
    IsEqual = FALSE;
    for (j = 0; !IsEqual && j < WaitParamsPtr->NumberOfElements; ++j) {
      if (CoreStatus == WaitParamsPtr->Status[j]) {
        IsEqual = TRUE;
      }
    }
    if ((((WaitParamsPtr->WaitForStatusFlags & WAIT_STATUS_EQUALITY) != 0) && IsEqual) ||
        (((WaitParamsPtr->WaitForStatusFlags & WAIT_STATUS_EQUALITY) == 0) && !IsEqual)) {
      break;
    }
  }
  return (CoreStatus);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Runs the AP task on the executing core.
 *
 * @param[in]      TaskPtr      Function descriptor
 * @param[in]      StdHeader    Configuration parameters pointer
 * @param[in]      ConfigParams Entry point CPU parameters pointer
 *
 * @return         Return value of the task, or zero if the task
 *                 was VOID.
 *
 */
UINT32
ApUtilTaskOnExecutingCore (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID *ConfigParams
  )
{
  UINT32   InvocationOptions;
  UINT32   ReturnCode;

  ReturnCode = 0;
  InvocationOptions = 0;

  if (TaskPtr->DataTransfer.DataSizeInDwords != 0) {
    InvocationOptions |= AP_TASK_HAS_INPUT;
  }
  if ((TaskPtr->ExeFlags & TASK_HAS_OUTPUT) != 0) {
    InvocationOptions |= AP_TASK_HAS_OUTPUT;
  }
  if ((TaskPtr->ExeFlags & PASS_EARLY_PARAMS) != 0) {
    InvocationOptions |= AP_PASS_EARLY_PARAMS;
  }

  switch (InvocationOptions) {
  case 0:
    TaskPtr->FuncAddress.PfApTask (StdHeader);
    break;
  case AP_TASK_HAS_INPUT:
    TaskPtr->FuncAddress.PfApTaskI (TaskPtr->DataTransfer.DataPtr, StdHeader);
    break;
  case AP_PASS_EARLY_PARAMS:
    TaskPtr->FuncAddress.PfApTaskC (StdHeader, ConfigParams);
    break;
  case (AP_TASK_HAS_INPUT | AP_PASS_EARLY_PARAMS):
    TaskPtr->FuncAddress.PfApTaskIC (TaskPtr->DataTransfer.DataPtr, StdHeader, ConfigParams);
    break;
  case AP_TASK_HAS_OUTPUT:
    ReturnCode = TaskPtr->FuncAddress.PfApTaskO (StdHeader);
    break;
  case (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT):
    ReturnCode = TaskPtr->FuncAddress.PfApTaskIO (TaskPtr->DataTransfer.DataPtr, StdHeader);
    break;
  case (AP_TASK_HAS_OUTPUT | AP_PASS_EARLY_PARAMS):
    ReturnCode = TaskPtr->FuncAddress.PfApTaskOC (StdHeader, ConfigParams);
    break;
  case (AP_TASK_HAS_INPUT | AP_TASK_HAS_OUTPUT | AP_PASS_EARLY_PARAMS):
    ReturnCode = TaskPtr->FuncAddress.PfApTaskIOC (TaskPtr->DataTransfer.DataPtr, StdHeader, ConfigParams);
    break;
  default:
    ReturnCode = 0;
    break;
  }
  return (ReturnCode);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Sets up the AP's IDT with NMI (INT2) being the only valid descriptor
 *
 * This function prepares the executing AP core for recovering from a hlt
 * instruction by initializing its IDTR.
 *
 * @param[in]        NmiIdtDescPtr Pointer to a writable IDT entry to
 *                                 be used for NMIs
 * @param[in]        StdHeader     Configuration parameters pointer
 *
 */
VOID
STATIC
ApUtilSetupIdtForHlt (
  IN       IDT_DESCRIPTOR *NmiIdtDescPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   DescSize;
  UINT64  HandlerOffset;
  UINT64  EferRegister;
  IDT_BASE_LIMIT IdtInfo;

  LibAmdMsrRead (MSR_EXTENDED_FEATURE_EN, &EferRegister, StdHeader);
  if ((EferRegister & 0x100) != 0) {
    DescSize = 16;
  } else {
    DescSize = 8;
  }

  HandlerOffset = (UINT64) (UINTN) NmiHandler;
  NmiIdtDescPtr->OffsetLo = (UINT16) HandlerOffset & 0xFFFF;
  NmiIdtDescPtr->OffsetHi = (UINT16) (HandlerOffset >> 16);
  GetCsSelector (&NmiIdtDescPtr->Selector, StdHeader);
  NmiIdtDescPtr->Flags = IDT_DESC_PRESENT | IDT_DESC_TYPE_INT32;
  NmiIdtDescPtr->Rsvd = 0;
  NmiIdtDescPtr->Offset64 = (UINT32) (HandlerOffset >> 32);
  NmiIdtDescPtr->Rsvd64 = 0;
  IdtInfo.Limit = (UINT16) ((DescSize * 3) - 1);
  IdtInfo.Base = (UINT64) (UINTN) NmiIdtDescPtr - (DescSize * 2);
  IDS_EXCEPTION_TRAP (IDS_IDT_UPDATE_EXCEPTION_VECTOR_FOR_AP, &IdtInfo, StdHeader);
  SetIdtr (&IdtInfo , StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Calculate the APIC ID for a given core.
 *
 * Get the current node's apic id and deconstruct it to the base id of local apic id space.
 * Then construct the target's apic id using that base.
 * @b Assumes: The target Socket and Core exist!
 * Other Notes:
 *  - Must run after HT initialization is complete.
 *  - Code sync: This calculation MUST match the assignment
 *    calculation done above in LocalApicInitializationAtEarly function.
 *  - Assumes family homogeneous population of all sockets.
 *
 *  @param[in]      TargetSocket   The socket in which the Core's Processor is installed.
 *  @param[in]      TargetCore     The Core on that Processor
 *  @param[out]     LocalApicId    Its APIC Id
 *  @param[in]      StdHeader      Handle to header for library and services.
 *
 */
VOID
GetLocalApicIdForCore (
  IN       UINT32            TargetSocket,
  IN       UINT32            TargetCore,
     OUT   UINT32            *LocalApicId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32  CoreIdBits;
  UINT32  CurrentNode;
  UINT32  CurrentCore;
  UINT32  TargetNode;
  UINT32  MaxCoresInProcessor;
  UINT32  TotalCores;
  UINT32  CurrentLocalApicId;
  UINT64  LocalApicBase;
  UINT32  TempVar_a;
  UINT64  Address;
  UINT32  ProcessorApicIndex;
  BOOLEAN ReturnResult;
  CPUID_DATA  CpuidData;

  TargetNode = 0;

  // Get local apic base Address
  ApUtilGetLocalApicBase (&LocalApicBase, StdHeader);
  Address = LocalApicBase + APIC_ID_REG;

  LibAmdMemRead (AccessWidth32, Address, &TempVar_a, StdHeader);

  // ApicId [7:0]
  CurrentLocalApicId = (TempVar_a >> APIC20_ApicId) & 0x000000FF;

  GetCurrentNodeAndCore (&CurrentNode, &CurrentCore, StdHeader);
  LibAmdCpuidRead (AMD_CPUID_ASIZE_PCCOUNT, &CpuidData, StdHeader);
  CoreIdBits = (CpuidData.ECX_Reg & 0x0000F000) >> 12;
  MaxCoresInProcessor = (1 << CoreIdBits);

  // Get the APIC Index of this processor.
  ProcessorApicIndex = GetProcessorApicIndex (CurrentNode, StdHeader);

  TotalCores = (MaxCoresInProcessor * ProcessorApicIndex) + CurrentCore;
  CurrentLocalApicId -= TotalCores;

  // Use the Node Id of TargetSocket, Module 0.  No socket transitions are missed or added,
  // even if the TargetCore is not on Module 0 in that processor and that's all that matters now.
  ReturnResult = GetNodeId (TargetSocket, 0, (UINT8 *)&TargetNode, StdHeader);
  ASSERT (ReturnResult);

  // Get the APIC Index of this processor.
  ProcessorApicIndex = GetProcessorApicIndex (TargetNode, StdHeader);

  CurrentLocalApicId += ((MaxCoresInProcessor * ProcessorApicIndex) + TargetCore);
  *LocalApicId = CurrentLocalApicId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Securely passes a buffer to the designated remote core.
 *
 * This function uses a sequence of remote reads to transmit a data
 * buffer, one UINT32 at a time.
 *
 * @param[in]      Socket       Socket number of the remote core
 * @param[in]      Core         Core number of the remote core
 * @param[in]      BufferInfo   Information about the buffer to pass, and
 *                              how to pass it
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 */
VOID
ApUtilTransmitBuffer (
  IN       UINT8   Socket,
  IN       UINT8   Core,
  IN       AP_DATA_TRANSFER  *BufferInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  TargetCore;
  UINT8  MyUniqueId;
  UINT8  CurrentStatus;
  UINT32 *CurrentPtr;
  UINT32 i;
  UINT32 MyCore;
  UINT32 MySocket;
  UINT32 Ignored;
  UINT32 TargetApicId;
  AP_WAIT_FOR_STATUS WaitForStatus;
  AGESA_STATUS IgnoredSts;

  GetLocalApicIdForCore ((UINT32) Socket, (UINT32) Core, &TargetApicId, StdHeader);

  if ((BufferInfo->DataTransferFlags & DATA_IN_MEMORY) != 0) {
    ApUtilWriteDataDword ((UINT32) 0x00000000, StdHeader);
  } else {
    ApUtilWriteDataDword ((UINT32) BufferInfo->DataSizeInDwords, StdHeader);
  }
  TargetCore = ApUtilCalculateUniqueId (Socket, Core, StdHeader);

  ApUtilWriteControlByte (TargetCore, StdHeader);

  IdentifyCore (StdHeader, &MySocket, &Ignored, &MyCore, &IgnoredSts);

  MyUniqueId = ApUtilCalculateUniqueId ((UINT8)MySocket, (UINT8)MyCore, StdHeader);

  WaitForStatus.Status = &MyUniqueId;
  WaitForStatus.NumberOfElements = 1;
  WaitForStatus.RetryCount = WAIT_INFINITELY;
  WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;

  ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
  ApUtilWriteDataDword (BufferInfo->DataTransferFlags, StdHeader);

  ApUtilWriteControlByte (CORE_DATA_FLAGS_READY, StdHeader);
  WaitForStatus.WaitForStatusFlags = 0;
  ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
  if ((BufferInfo->DataTransferFlags & DATA_IN_MEMORY) != 0) {
    ApUtilTransmitPointer (TargetApicId, (VOID **) &BufferInfo->DataPtr, StdHeader);
  } else {
    ApUtilWriteControlByte (CORE_STS_DATA_READY_1, StdHeader);
    CurrentStatus = CORE_STS_DATA_READY_0;
    WaitForStatus.Status = &CurrentStatus;
    WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;
    ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
    WaitForStatus.WaitForStatusFlags = 0;
    CurrentPtr = (UINT32 *) BufferInfo->DataPtr;
    for (i = 0; i < BufferInfo->DataSizeInDwords; ++i) {
      ApUtilWriteDataDword (*CurrentPtr++, StdHeader);
      ApUtilWriteControlByte (CurrentStatus, StdHeader);
      ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
      CurrentStatus ^= 0x01;
    }
  }
  ApUtilWriteControlByte (CORE_ACTIVE, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Securely receives a buffer from the designated remote core.
 *
 * This function uses a sequence of remote reads to receive a data
 * buffer, one UINT32 at a time.
 *
 * @param[in]      Socket       Socket number of the remote core
 * @param[in]      Core         Core number of the remote core
 * @param[in]      BufferInfo   Information about where to place the buffer
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 * @retval         AGESA_SUCCESS Transaction was successful
 * @retval         AGESA_ALERT   The non-NULL desired location to place
 *                               the buffer was not used as the buffer
 *                               resides in a shared memory space.  The
 *                               input data pointer has changed.
 * @retval         AGESA_ERROR   There is not enough room to receive the
 *                               buffer.
 *
 */
AGESA_STATUS
ApUtilReceiveBuffer (
  IN       UINT8   Socket,
  IN       UINT8   Core,
  IN OUT   AP_DATA_TRANSFER  *BufferInfo,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8    MyUniqueId;
  UINT8    SourceUniqueId;
  UINT8    CurrentStatus;
  UINT32   i;
  UINT32   MySocket;
  UINT32   MyCore;
  UINT32   Ignored;
  UINT32   *CurrentPtr;
  UINT32   TransactionSize;
  UINT32   TargetApicId;
  AGESA_STATUS ReturnStatus;
  ALLOCATE_HEAP_PARAMS HeapMalloc;
  AP_WAIT_FOR_STATUS WaitForStatus;

  ReturnStatus = AGESA_SUCCESS;
  IdentifyCore (StdHeader, &MySocket, &Ignored, &MyCore, &ReturnStatus);

  MyUniqueId = ApUtilCalculateUniqueId ((UINT8)MySocket, (UINT8)MyCore, StdHeader);

  GetLocalApicIdForCore ((UINT32) Socket, (UINT32) Core, &TargetApicId, StdHeader);

  WaitForStatus.Status = &MyUniqueId;
  WaitForStatus.NumberOfElements = 1;
  WaitForStatus.RetryCount = WAIT_INFINITELY;
  WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;

  ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
  TransactionSize = ApUtilReadRemoteDataDword (TargetApicId, StdHeader);

  if (BufferInfo->DataPtr == NULL && TransactionSize != 0) {
    HeapMalloc.BufferHandle = AMD_CPU_AP_TASKING_HANDLE;
    HeapMalloc.Persist = HEAP_LOCAL_CACHE;
    // Deallocate the general purpose heap structure, if it exists.  Ignore
    // the status in case it does not exist.
    HeapDeallocateBuffer (HeapMalloc.BufferHandle, StdHeader);
    HeapMalloc.RequestedBufferSize = (TransactionSize * XFER_ELEMENT_SIZE);
    if (HeapAllocateBuffer (&HeapMalloc, StdHeader) == AGESA_SUCCESS) {
      BufferInfo->DataPtr = (UINT32 *) HeapMalloc.BufferPtr;
      BufferInfo->DataSizeInDwords = (UINT16) (HeapMalloc.RequestedBufferSize / XFER_ELEMENT_SIZE);
    } else {
      BufferInfo->DataSizeInDwords = 0;
    }
  }

  if (TransactionSize <= BufferInfo->DataSizeInDwords) {
    SourceUniqueId = ApUtilCalculateUniqueId (Socket, Core, StdHeader);
    ApUtilWriteControlByte (SourceUniqueId, StdHeader);
    CurrentStatus = CORE_DATA_FLAGS_READY;
    WaitForStatus.Status = &CurrentStatus;
    ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
    BufferInfo->DataTransferFlags =  ApUtilReadRemoteDataDword (TargetApicId, StdHeader);
    ApUtilWriteControlByte (CORE_DATA_FLAGS_ACKNOWLEDGE, StdHeader);
    if ((BufferInfo->DataTransferFlags & DATA_IN_MEMORY) != 0) {
      if (BufferInfo->DataPtr != NULL) {
        ReturnStatus = AGESA_ALERT;
      }
      ApUtilReceivePointer (TargetApicId, (VOID **) &BufferInfo->DataPtr, StdHeader);
    } else {
      CurrentStatus = CORE_STS_DATA_READY_1;
      ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
      CurrentStatus = CORE_STS_DATA_READY_0;
      ApUtilWriteControlByte (CurrentStatus, StdHeader);
      CurrentPtr = BufferInfo->DataPtr;
      for (i = 0; i < TransactionSize; ++i) {
        ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
        *CurrentPtr++ = ApUtilReadRemoteDataDword (TargetApicId, StdHeader);
        CurrentStatus ^= 0x01;
        ApUtilWriteControlByte (CurrentStatus, StdHeader);
      }
    }
    ApUtilWriteControlByte (CORE_ACTIVE, StdHeader);
  } else {
    BufferInfo->DataSizeInDwords = (UINT16) TransactionSize;
    ReturnStatus = AGESA_ERROR;
  }
  return (ReturnStatus);
}


VOID
RelinquishControlOfAllAPs (
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32        BscSocket;
  UINT32        Ignored;
  UINT32        BscCoreNum;
  UINT32        Core;
  UINT32        Socket;
  UINT32        NumberOfSockets;
  AP_TASK       TaskPtr;
  AGESA_STATUS  IgnoredSts;

  ASSERT (IsBsp (StdHeader, &IgnoredSts));

  TaskPtr.FuncAddress.PfApTask = PerformFinalHalt;
  TaskPtr.DataTransfer.DataSizeInDwords = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE;

  IdentifyCore (StdHeader, &BscSocket, &Ignored, &BscCoreNum, &IgnoredSts);
  NumberOfSockets = GetPlatformNumberOfSockets ();

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (GetActiveCoresInGivenSocket (Socket, &Core, StdHeader)) {
      while (Core-- > 0) {
        if ((Socket != BscSocket) || (Core != BscCoreNum)) {
          ApUtilRunCodeOnSocketCore ((UINT8) Socket, (UINT8) Core, &TaskPtr, StdHeader);
        }
      }
    }
  }
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * The last AGESA code that an AP performs
 *
 * This function, run only by APs, breaks down their cache subsystem, sets up
 * for memory to be present upon wake (from IBV Init/Startup IPIs), and halts.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
STATIC
PerformFinalHalt (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 PrimaryCore;
  UINT32 HaltFlags;
  UINT32 CacheEnDis;
  CPU_SPECIFIC_SERVICES *FamilyServices;
  CONST CACHE_INFO *CacheInfoPtr;
  UINT8  NumberOfElements;

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilyServices, StdHeader);
  ASSERT (FamilyServices != NULL);
  // CacheEnDis is a family specific flag, that lets the code to decide whether to
  // keep the cache control bits set or cleared.
  CacheEnDis = FamilyServices->InitCacheDisabled;

  // Determine if the current core has the primary core role.  The first core to execute
  // in each compute unit has the primary role.
  PrimaryCore = (UINT32) IsCoreComputeUnitPrimary (FirstCoreIsComputeUnitPrimary, StdHeader);

  // If the core is not PrimaryCore, check if VarMTRRs are not shared among cores in a compute unit,
  // low 32bits of VarMTRR mask is 0xFFFFFFFF. Treat each AP as primary core for setting MTRRs.
  if (PrimaryCore != TRUE) {
    FamilyServices->GetCacheInfo (FamilyServices, (CONST VOID **) &CacheInfoPtr, &NumberOfElements, StdHeader);
    if (((UINT32) CacheInfoPtr->VariableMtrrHeapMask) == 0xFFFFFFFF) {
      PrimaryCore = TRUE;
    }
  }

  // Aggregate the flags for the halt service.
  HaltFlags = PrimaryCore | (CacheEnDis << 1);

  ApUtilWriteControlByte (CORE_UNAVAILABLE, StdHeader);
  ExecuteFinalHltInstruction (HaltFlags, UserOptions.CfgApMtrrSettingsList, StdHeader);
}

/*---------------------------------------------------------------------------------------*/
/**
 * Reads the APIC register on the designated remote core.
 *
 * This function uses the remote read inter-processor interrupt protocol
 * to read an APIC register from the remote core
 *
 * @param[in]        TargetApicId  Local APIC ID of the desired core
 * @param[in]        RegAddr       APIC register to read
 * @param[in]        StdHeader     Configuration parameters pointer
 *
 * @return           The current value of the remote core's desired APIC register
 *
 */
UINT32
STATIC
ApUtilRemoteRead (
  IN       UINT32            TargetApicId,
  IN       UINT8             RegAddr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 ApicRegister;
  UINT64 ApicBase;
  UINT64 ApicAddr;

  ApUtilGetLocalApicBase (&ApicBase, StdHeader);
  TargetApicId <<= LOCAL_APIC_ID;

  do {
    ApicAddr = ApicBase + APIC_CMD_HI_REG;
    LibAmdMemWrite (AccessWidth32, ApicAddr, &TargetApicId, StdHeader);
    ApicAddr = ApicBase + APIC_CMD_LO_REG;
    ApicRegister = CMD_REG_TO_READ | (UINT32) RegAddr;
    LibAmdMemWrite (AccessWidth32, ApicAddr, &ApicRegister, StdHeader);
    do {
      LibAmdMemRead (AccessWidth32, ApicAddr, &ApicRegister, StdHeader);
    } while ((ApicRegister & CMD_REG_DELIVERY_STATUS) != 0);
    while ((ApicRegister & CMD_REG_REMOTE_RD_STS_MSK) == CMD_REG_REMOTE_DELIVERY_PENDING) {
      LibAmdMemRead (AccessWidth32, ApicAddr, &ApicRegister, StdHeader);
    }
  } while ((ApicRegister & CMD_REG_REMOTE_RD_STS_MSK) != CMD_REG_REMOTE_DELIVERY_DONE);
  ApicAddr = ApicBase + APIC_REMOTE_READ_REG;
  LibAmdMemRead (AccessWidth32, ApicAddr, &ApicRegister, StdHeader);
  return (ApicRegister);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Writes an APIC register on the executing core.
 *
 * This function gets the base address of the executing core's local APIC,
 * and writes a UINT32 value to a specified offset.
 *
 * @param[in]      RegAddr      APIC register to write to
 * @param[in]      Value        Data to be written to the desired APIC register
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 */
VOID
STATIC
ApUtilLocalWrite (
  IN       UINT32 RegAddr,
  IN       UINT32 Value,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT64 ApicAddr;

  ApUtilGetLocalApicBase (&ApicAddr, StdHeader);
  ApicAddr += RegAddr;

  LibAmdMemWrite (AccessWidth32, ApicAddr, &Value, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Reads an APIC register on the executing core.
 *
 * This function gets the base address of the executing core's local APIC,
 * and reads a UINT32 value from a specified offset.
 *
 * @param[in]      RegAddr      APIC register to read from
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 * @return         The current value of the local APIC register
 *
 */
UINT32
STATIC
ApUtilLocalRead (
  IN       UINT32  RegAddr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 ApicRegister;
  UINT64 ApicAddr;

  ApUtilGetLocalApicBase (&ApicAddr, StdHeader);
  ApicAddr += RegAddr;
  LibAmdMemRead (AccessWidth32, ApicAddr, &ApicRegister, StdHeader);

  return (ApicRegister);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Returns the 64-bit base address of the executing core's local APIC.
 *
 * This function reads the APICBASE MSR and isolates the programmed address.
 *
 * @param[out]     ApicBase     Base address
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 */
VOID
STATIC
ApUtilGetLocalApicBase (
     OUT   UINT64 *ApicBase,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  LibAmdMsrRead (MSR_APIC_BAR, ApicBase, StdHeader);
  *ApicBase &= LAPIC_BASE_ADDR_MASK;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Determines the unique ID of the input Socket/Core.
 *
 * This routine converts a socket-core combination to to a number
 * that will be used to directly address a particular core.  This
 * unique value must be less than 128 because we only have a byte
 * to use for status.  APIC IDs are not guaranteed to be below
 * 128.
 *
 * @param[in]      Socket       Socket number of the remote core
 * @param[in]      Core         Core number of the remote core
 * @param[in]      StdHeader    Configuration parameters pointer
 *
 * @return         The unique ID of the desired core
 *
 */
UINT8
STATIC
ApUtilCalculateUniqueId (
  IN       UINT8 Socket,
  IN       UINT8 Core,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8 UniqueId;

  UniqueId = ((Core << 3) | Socket);
  ASSERT ((UniqueId & 0x80) == 0);
  return (UniqueId);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Wakes up a core from the halted state.
 *
 * This function sends a directed NMI inter-processor interrupt to
 * the input Socket/Core.
 *
 * @param[in]      TargetApicId    Local APIC ID of the desired core
 * @param[in]      StdHeader       Configuration parameters pointer
 *
 */
VOID
STATIC
ApUtilFireDirectedNmi (
  IN       UINT32 TargetApicId,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  TargetApicId <<= LOCAL_APIC_ID;

  ApUtilLocalWrite ((UINT32) APIC_CMD_HI_REG, TargetApicId, StdHeader);
  ApUtilLocalWrite ((UINT32) APIC_CMD_LO_REG, (UINT32) CMD_REG_TO_NMI, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Securely receives a pointer from the designated remote core.
 *
 * This function uses a sequence of remote reads to receive a pointer,
 * one UINT32 at a time.
 *
 * @param[in]      TargetApicId  Local APIC ID of the desired core
 * @param[out]     ReturnPointer Pointer passed from remote core
 * @param[in]      StdHeader     Configuration parameters pointer
 *
 */
VOID
STATIC
ApUtilReceivePointer (
  IN       UINT32 TargetApicId,
     OUT   VOID  **ReturnPointer,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   i;
  UINT8   WaitStatus;
  UINT32  *AddressScratchPtr;
  AP_WAIT_FOR_STATUS WaitForStatus;

  WaitStatus = CORE_STS_DATA_READY_0;
  WaitForStatus.Status = &WaitStatus;
  WaitForStatus.NumberOfElements = 1;
  WaitForStatus.RetryCount = WAIT_INFINITELY;
  AddressScratchPtr = (UINT32 *) ReturnPointer;
  for (i = 0; i < SIZE_IN_DWORDS (AddressScratchPtr); ++i) {
    ApUtilWriteControlByte (CORE_NEEDS_PTR, StdHeader);
    WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;
    ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
    *AddressScratchPtr++ = ApUtilReadRemoteDataDword (TargetApicId, StdHeader);
    ApUtilWriteControlByte (CORE_ACTIVE, StdHeader);
    WaitForStatus.WaitForStatusFlags = 0;
    ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Securely transmits a pointer to the designated remote core.
 *
 * This function uses a sequence of remote reads to transmit a pointer,
 * one UINT32 at a time.
 *
 * @param[in]      TargetApicId  Local APIC ID of the desired core
 * @param[out]     Pointer       Pointer passed from remote core
 * @param[in]      StdHeader     Configuration parameters pointer
 *
 */
VOID
STATIC
ApUtilTransmitPointer (
  IN       UINT32 TargetApicId,
  IN       VOID  **Pointer,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   i;
  UINT8   WaitStatus;
  UINT32  *AddressScratchPtr;
  AP_WAIT_FOR_STATUS WaitForStatus;

  WaitStatus = CORE_NEEDS_PTR;
  WaitForStatus.Status = &WaitStatus;
  WaitForStatus.NumberOfElements = 1;
  WaitForStatus.RetryCount = WAIT_INFINITELY;

  AddressScratchPtr = (UINT32 *) Pointer;

  for (i = 0; i < SIZE_IN_DWORDS (AddressScratchPtr); i++) {
    WaitForStatus.WaitForStatusFlags = WAIT_STATUS_EQUALITY;
    ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
    ApUtilWriteDataDword (*AddressScratchPtr++, StdHeader);
    ApUtilWriteControlByte (CORE_STS_DATA_READY_0, StdHeader);
    WaitForStatus.WaitForStatusFlags = 0;
    ApUtilWaitForCoreStatus (TargetApicId, &WaitForStatus, StdHeader);
    ApUtilWriteControlByte (CORE_ACTIVE, StdHeader);
  }
}
