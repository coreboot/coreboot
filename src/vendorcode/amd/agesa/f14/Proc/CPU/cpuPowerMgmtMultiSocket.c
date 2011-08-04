/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Power Management Multisocket Functions.
 *
 * Contains code for doing power management for multisocket CPUs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 35136 $   @e \$Date: 2010-07-16 11:29:48 +0800 (Fri, 16 Jul 2010) $
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "Ids.h"
#include "cpuRegisters.h"
#include "GeneralServices.h"
#include "cpuServices.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuPowerMgmtSystemTables.h"
#include "cpuPowerMgmtMultiSocket.h"
#include "GeneralServices.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

#define FILECODE PROC_CPU_CPUPOWERMGMTMULTISOCKET_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
GetNextEvent (
  IN OUT   VOID  *EventLogEntryPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

AGESA_STATUS
GetEarlyPmErrorsMulti (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Multisocket BSC call to start all system core 0s to perform a standard AP_TASK.
 *
 * This function loops through all possible socket locations, starting core 0 of
 * each populated socket to perform the passed in AP_TASK.  After starting all
 * other core 0s, the BSC will perform the AP_TASK as well.  This must be run by
 * the system BSC only.
 *
 * @param[in]  TaskPtr           Function descriptor
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  ConfigParams      AMD entry point's CPU parameter structure
 *
 */
VOID
RunCodeOnAllSystemCore0sMulti (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID *ConfigParams
  )
{
  UINT32 BscSocket;
  UINT32 BscModule;
  UINT32 BscCoreNum;
  UINT8  Socket;
  UINT32 NumberOfSockets;
  AGESA_STATUS DummyStatus;

  ASSERT (IsBsp (StdHeader, &DummyStatus));

  NumberOfSockets = GetPlatformNumberOfSockets ();

  IdentifyCore (StdHeader, &BscSocket, &BscModule, &BscCoreNum, &DummyStatus);

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (Socket != BscSocket) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        ApUtilRunCodeOnSocketCore (Socket, 0, TaskPtr, StdHeader);
      }
    }
  }
  ApUtilTaskOnExecutingCore (TaskPtr, StdHeader, ConfigParams);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Multisocket BSC call to determine the maximum number of steps that any single
 * processor needs to execute.
 *
 * This function loops through all possible socket locations, gathering the number
 * of power management steps each populated socket requires, and returns the
 * highest number.
 *
 * @param[out] NumSystemSteps    Maximum number of system steps required
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
GetNumberOfSystemPmStepsPtrMulti (
     OUT   UINT8 *NumSystemSteps,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  NumberOfSteps;
  UINT32 NumberOfSockets;
  UINT32 Socket;
  SYS_PM_TBL_STEP *Ignored;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  NumberOfSockets = GetPlatformNumberOfSockets ();
  *NumSystemSteps = 0;

  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetCpuServicesOfSocket (Socket, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
      FamilySpecificServices->GetSysPmTableStruct (FamilySpecificServices, (const VOID **)&Ignored, &NumberOfSteps, StdHeader);
      if (NumberOfSteps > *NumSystemSteps) {
        *NumSystemSteps = NumberOfSteps;
      }
    }
  }
}


/*---------------------------------------------------------------------------------------*/
/**
 * Multisocket call to determine the frequency that the northbridges must run.
 *
 * This function loops through all possible socket locations, comparing the
 * maximum NB frequencies to determine the slowest.  This function also
 * determines if all coherent NB frequencies are equivalent.
 *
 * @param[in]  NbPstate                    NB P-state number to check (0 = fastest)
 * @param[in]  PlatformConfig              Platform profile/build option config structure.
 * @param[out] SystemNbCofNumerator        NB frequency numerator for the system in MHz
 * @param[out] SystemNbCofDenominator      NB frequency denominator for the system
 * @param[out] SystemNbCofsMatch           Whether or not all NB frequencies are equivalent
 * @param[out] NbPstateIsEnabledOnAllCPUs  Whether or not NbPstate is valid on all CPUs
 * @param[in]  StdHeader                   Config handle for library and services
 *
 * @retval     TRUE                        At least one processor has NbPstate enabled.
 * @retval     FALSE                       NbPstate is disabled on all CPUs
 *
 */
BOOLEAN
GetSystemNbCofMulti (
  IN       UINT32 NbPstate,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32 *SystemNbCofNumerator,
     OUT   UINT32 *SystemNbCofDenominator,
     OUT   BOOLEAN *SystemNbCofsMatch,
     OUT   BOOLEAN *NbPstateIsEnabledOnAllCPUs,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32   Socket;
  UINT8    Module;
  UINT32   CurrentNbCof;
  UINT32   CurrentDivisor;
  UINT32   CurrentFreq;
  UINT32   LowFrequency;
  UINT32   Ignored32;
  BOOLEAN  FirstCofNotFound;
  BOOLEAN  NbPstateDisabled;
  BOOLEAN  IsNbPstateEnabledOnAny;
  PCI_ADDR PciAddress;
  AGESA_STATUS Ignored;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  // Find the slowest NB COF in the system & whether or not all are equivalent
  LowFrequency = 0xFFFFFFFF;
  *SystemNbCofsMatch = TRUE;
  *NbPstateIsEnabledOnAllCPUs = FALSE;
  IsNbPstateEnabledOnAny = FALSE;
  FirstCofNotFound = TRUE;
  NbPstateDisabled = FALSE;
  for (Socket = 0; Socket < GetPlatformNumberOfSockets (); Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetCpuServicesOfSocket (Socket, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (StdHeader, Socket, Module, &PciAddress, &Ignored)) {
          break;
        }
      }
      if (FamilySpecificServices->GetNbPstateInfo (FamilySpecificServices,
                                                   PlatformConfig,
                                                   &PciAddress,
                                                   NbPstate,
                                                   &CurrentNbCof,
                                                   &CurrentDivisor,
                                                   &Ignored32,
                                                   StdHeader)) {
        ASSERT (CurrentDivisor != 0);
        CurrentFreq = (CurrentNbCof / CurrentDivisor);
        if (FirstCofNotFound) {
          *SystemNbCofNumerator = CurrentNbCof;
          *SystemNbCofDenominator = CurrentDivisor;
          LowFrequency = CurrentFreq;
          IsNbPstateEnabledOnAny = TRUE;
          if (!NbPstateDisabled) {
            *NbPstateIsEnabledOnAllCPUs = TRUE;
          }
          FirstCofNotFound = FALSE;
        } else {
          if (CurrentFreq != LowFrequency) {
            *SystemNbCofsMatch = FALSE;
            if (CurrentFreq < LowFrequency) {
              LowFrequency = CurrentFreq;
              *SystemNbCofNumerator = CurrentNbCof;
              *SystemNbCofDenominator = CurrentDivisor;
            }
          }
        }
      } else {
        NbPstateDisabled = TRUE;
        *NbPstateIsEnabledOnAllCPUs = FALSE;
      }
    }
  }
  return IsNbPstateEnabledOnAny;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Multisocket call to determine if the BIOS is responsible for updating the
 * northbridge operating frequency and voltage.
 *
 * This function loops through all possible socket locations, checking whether
 * any populated sockets require NB COF VID programming.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @retval     TRUE    BIOS needs to set up NB frequency and voltage
 * @retval     FALSE   BIOS does not need to set up NB frequency and voltage
 *
 */
BOOLEAN
GetSystemNbCofVidUpdateMulti (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8    Module;
  UINT32   Socket;
  UINT32   NumberOfSockets;
  BOOLEAN  IgnoredBool;
  BOOLEAN  AtLeast1RequiresUpdate;
  PCI_ADDR PciAddress;
  AGESA_STATUS Ignored;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  NumberOfSockets = GetPlatformNumberOfSockets ();

  AtLeast1RequiresUpdate = FALSE;
  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (IsProcessorPresent (Socket, StdHeader)) {
      GetCpuServicesOfSocket (Socket, (const CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
      for (Module = 0; Module < GetPlatformNumberOfModules (); Module++) {
        if (GetPciAddress (StdHeader, (UINT8) Socket, Module, &PciAddress, &Ignored)) {
          break;
        }
      }
      if (FamilySpecificServices->IsNbCofInitNeeded (FamilySpecificServices, &PciAddress, &IgnoredBool, StdHeader)) {
        AtLeast1RequiresUpdate = TRUE;
        break;
      }
    }
  }
  return AtLeast1RequiresUpdate;
}


/*---------------------------------------------------------------------------------------*/
/**
 * Multisocket call to determine the most severe AGESA_STATUS return value after
 * processing the power management initialization tables.
 *
 * This function loops through all possible socket locations, collecting any
 * power management initialization errors that may have occurred.  These errors
 * are transferred from the core 0s of the socket in which the errors occurred
 * to the BSC's heap.  The BSC's heap is then searched for the most severe error
 * that occurred, and returns it.  This function must be called by the BSC only.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @return     The most severe error code from power management init
 *
 */
AGESA_STATUS
GetEarlyPmErrorsMulti (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16 i;
  UINT32 BscSocket;
  UINT32 BscModule;
  UINT32 BscCoreNum;
  UINT32 Socket;
  UINT32 NumberOfSockets;
  AP_TASK      TaskPtr;
  AGESA_EVENT  EventLogEntry;
  AGESA_STATUS ReturnCode;
  AGESA_STATUS DummyStatus;

  ASSERT (IsBsp (StdHeader, &ReturnCode));

  ReturnCode = AGESA_SUCCESS;
  EventLogEntry.EventClass = AGESA_SUCCESS;
  EventLogEntry.EventInfo = 0;
  EventLogEntry.DataParam1 = 0;
  EventLogEntry.DataParam2 = 0;
  EventLogEntry.DataParam3 = 0;
  EventLogEntry.DataParam4 = 0;

  NumberOfSockets = GetPlatformNumberOfSockets ();
  IdentifyCore (StdHeader, &BscSocket, &BscModule, &BscCoreNum, &DummyStatus);

  TaskPtr.FuncAddress.PfApTaskI = GetNextEvent;
  TaskPtr.DataTransfer.DataSizeInDwords = SIZE_IN_DWORDS (AGESA_EVENT);
  TaskPtr.DataTransfer.DataPtr = &EventLogEntry;
  TaskPtr.DataTransfer.DataTransferFlags = 0;
  TaskPtr.ExeFlags = WAIT_FOR_CORE | RETURN_PARAMS;
  for (Socket = 0; Socket < NumberOfSockets; Socket++) {
    if (Socket != BscSocket) {
      if (IsProcessorPresent (Socket, StdHeader)) {
        do {
          ApUtilRunCodeOnSocketCore ((UINT8)Socket, (UINT8) 0, &TaskPtr, StdHeader);
          if ((EventLogEntry.EventInfo & CPU_EVENT_PM_EVENT_MASK) == CPU_EVENT_PM_EVENT_CLASS) {
            PutEventLog (
              EventLogEntry.EventClass,
              EventLogEntry.EventInfo,
              EventLogEntry.DataParam1,
              EventLogEntry.DataParam2,
              EventLogEntry.DataParam3,
              EventLogEntry.DataParam4,
              StdHeader
              );
          }
        } while (EventLogEntry.EventInfo != 0);
      }
    }
  }

  for (i = 0; PeekEventLog (&EventLogEntry, i, StdHeader); i++) {
    if ((EventLogEntry.EventInfo & CPU_EVENT_PM_EVENT_MASK) == CPU_EVENT_PM_EVENT_CLASS) {
      if (EventLogEntry.EventClass > ReturnCode) {
        ReturnCode = EventLogEntry.EventClass;
      }
    }
  }
  return (ReturnCode);
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * AP task to return the next event log entry to the BSC.
 *
 * This function calls to the event log manager to retrieve the next error out
 * of the heap.
 *
 * @param[out] EventLogEntryPtr  The AP's next event log entry
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
STATIC
GetNextEvent (
  IN OUT   VOID  *EventLogEntryPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  GetEventLog ((AGESA_EVENT *) EventLogEntryPtr, StdHeader);
}
