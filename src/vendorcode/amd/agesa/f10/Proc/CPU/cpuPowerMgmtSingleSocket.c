/**
 * @file
 *
 * AMD CPU Power Management Single Socket Functions.
 *
 * Contains code for doing power management for single socket CPU
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 44323 $   @e \$Date: 2010-12-22 01:24:58 -0700 (Wed, 22 Dec 2010) $
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "AGESA.h"
#include "amdlib.h"
#include "GeneralServices.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuPowerMgmtSystemTables.h"
#include "cpuPowerMgmtSingleSocket.h"
#include "Filecode.h"
#define FILECODE PROC_CPU_CPUPOWERMGMTSINGLESOCKET_FILECODE
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

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Single socket BSC call to start all system core 0s to perform a standard AP_TASK.
 *
 * This function will simply invoke the task on the executing core.  This must be
 * run by the system BSC only.
 *
 * @param[in]  TaskPtr           Function descriptor
 * @param[in]  StdHeader         Config handle for library and services
 * @param[in]  ConfigParams      AMD entry point's CPU parameter structure
 *
 */
VOID
RunCodeOnAllSystemCore0sSingle (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID *ConfigParams
  )
{
  ApUtilTaskOnExecutingCore (TaskPtr, StdHeader, ConfigParams);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Single socket BSC call to determine the maximum number of steps that any single
 * processor needs to execute.
 *
 * This function simply returns the number of steps that the BSC needs.
 *
 * @param[out] NumSystemSteps    Maximum number of system steps required
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
GetNumberOfSystemPmStepsPtrSingle (
     OUT   UINT8 *NumSystemSteps,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  SYS_PM_TBL_STEP *Ignored;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetSysPmTableStruct (FamilySpecificServices, (CONST VOID **)&Ignored, NumSystemSteps, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Single socket call to determine the frequency that the northbridges must run.
 *
 * This function simply returns the executing core's NB frequency, and that all
 * NB frequencies are equivalent.
 *
 * @param[out] SystemNbCof       NB frequency for the system in MHz
 * @param[out] SystemNbCofsMatch Whether or not all NB frequencies are equivalent
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @retval     AGESA_SUCCESS
 *
 */
AGESA_STATUS
GetSystemNbCofSingle (
     OUT   UINT32 *SystemNbCof,
     OUT   BOOLEAN *SystemNbCofsMatch,
  IN OUT   AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Ignored;
  PCI_ADDR PciAddress;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
  *SystemNbCofsMatch = TRUE;
  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  return (FamilySpecificServices->GetNbFrequency (FamilySpecificServices, &PciAddress, SystemNbCof, &Ignored, StdHeader));
}


/*---------------------------------------------------------------------------------------*/
/**
 * Single socket call to determine if the BIOS is responsible for updating the
 * northbridge operating frequency and voltage.
 *
 * This function simply returns whether or not the executing core needs NB COF
 * VID programming.
 *
 * @param[in]  StdHeader              Config handle for library and services
 *
 * @retval     TRUE    BIOS needs to set up NB frequency and voltage
 * @retval     FALSE   BIOS does not need to set up NB frequency and voltage
 *
 */
BOOLEAN
GetSystemNbCofVidUpdateSingle (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN   Ignored;
  PCI_ADDR PciAddress;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
  GetCpuServicesOfCurrentCore (&FamilySpecificServices, StdHeader);
  return (FamilySpecificServices->IsNbCofInitNeeded (FamilySpecificServices, &PciAddress, &Ignored, StdHeader));
}


/*---------------------------------------------------------------------------------------*/
/**
 * Single socket call to determine the most severe AGESA_STATUS return value after
 * processing the power management initialization tables.
 *
 * This function searches the event log for the most severe error and returns
 * the status code.  This function must be called by the BSC only.
 *
 * @param[in]  StdHeader         Config handle for library and services
 *
 * @return     The most severe error code from power management init
 *
 */
AGESA_STATUS
GetEarlyPmErrorsSingle (
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT16       i;
  AGESA_EVENT  EventLogEntry;
  AGESA_STATUS ReturnCode;

  ASSERT (IsBsp (StdHeader, &ReturnCode));

  ReturnCode = AGESA_SUCCESS;
  for (i = 0; PeekEventLog (&EventLogEntry, i, StdHeader); i++) {
    if ((EventLogEntry.EventInfo & CPU_EVENT_PM_EVENT_MASK) == CPU_EVENT_PM_EVENT_CLASS) {
      if (EventLogEntry.EventClass > ReturnCode) {
        ReturnCode = EventLogEntry.EventClass;
      }
    }
  }

  return (ReturnCode);
}

