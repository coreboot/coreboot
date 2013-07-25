/* $NoKeywords:$ */
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
#include "GeneralServices.h"
#include "cpuRegisters.h"
#include "cpuApicUtilities.h"
#include "cpuFamilyTranslation.h"
#include "cpuPowerMgmtSystemTables.h"
#include "cpuPowerMgmtSingleSocket.h"
#include "Filecode.h"
CODE_GROUP (G1_PEICC)
RDATA_GROUP (G1_PEICC)

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
 * @return     The severe error code from AP_TASK
 *
 */
AGESA_STATUS
RunCodeOnAllSystemCore0sSingle (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID *ConfigParams
  )
{
  AGESA_STATUS  CalledStatus;

  CalledStatus = ApUtilTaskOnExecutingCore (TaskPtr, StdHeader, ConfigParams);
  return CalledStatus;
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

  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  FamilySpecificServices->GetSysPmTableStruct (FamilySpecificServices, (CONST VOID **) &Ignored, NumSystemSteps, StdHeader);
}


/*---------------------------------------------------------------------------------------*/
/**
 * Single socket call to determine the frequency that the northbridges must run.
 *
 * This function simply returns the executing core's NB frequency, and that all
 * NB frequencies are equivalent.
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
GetSystemNbCofSingle (
  IN       UINT32 NbPstate,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32 *SystemNbCofNumerator,
     OUT   UINT32 *SystemNbCofDenominator,
     OUT   BOOLEAN *SystemNbCofsMatch,
     OUT   BOOLEAN *NbPstateIsEnabledOnAllCPUs,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT32 Ignored;
  PCI_ADDR PciAddress;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
  *SystemNbCofsMatch = TRUE;
  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  *NbPstateIsEnabledOnAllCPUs = FamilySpecificServices->GetNbPstateInfo (FamilySpecificServices,
                                                                         PlatformConfig,
                                                                         &PciAddress,
                                                                         NbPstate,
                                                                         SystemNbCofNumerator,
                                                                         SystemNbCofDenominator,
                                                                         &Ignored,
                                                                         StdHeader);
  return *NbPstateIsEnabledOnAllCPUs;
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
  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
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

/**
 * Single socket call to loop through all Nb Pstates, comparing the NB frequencies
 * to determine the slowest in the system. This routine also returns the NB P0 frequency.
 *
 * @param[in]  PlatformConfig      Platform profile/build option config structure.
 * @param[out] MinSysNbFreq        NB frequency numerator for the system in MHz
 * @param[out] MinP0NbFreq         NB frequency numerator for P0 in MHz
 * @param[in]  StdHeader           Config handle for library and services
 */
VOID
GetMinNbCofSingle (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32                 *MinSysNbFreq,
     OUT   UINT32                 *MinP0NbFreq,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PCI_ADDR              PciAddress;
  CPU_SPECIFIC_SERVICES *FamilySpecificServices;
  AGESA_STATUS AgesaStatus;

  PciAddress.AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);
  GetCpuServicesOfCurrentCore ((CONST CPU_SPECIFIC_SERVICES **)&FamilySpecificServices, StdHeader);
  AgesaStatus = FamilySpecificServices->GetMinMaxNbFrequency (FamilySpecificServices,
                                             PlatformConfig,
                                             &PciAddress,
                                             MinSysNbFreq,
                                             MinP0NbFreq,
                                             StdHeader);
  ASSERT (AgesaStatus == AGESA_SUCCESS);
  ASSERT ((MinSysNbFreq != 0) && (MinP0NbFreq != 0));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Get PCI Config Space Address for the current running core.
 *
 * @param[out]   PciAddress   The Processor's PCI Config Space address (Function 0, Register 0)
 * @param[in]    StdHeader    Header for library and services.
 *
 * @retval       TRUE         The core is present, PCI Address valid
 * @retval       FALSE        The core is not present, PCI Address not valid.
 */
BOOLEAN
GetCurrPciAddrSingle (
     OUT   PCI_ADDR               *PciAddress,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  PciAddress->AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);

  return TRUE;
}

/*---------------------------------------------------------------------------------------*/
/**
 *  Writes to all nodes on the executing core's socket.
 *
 *  @param[in]     PciAddress    The Function and Register to update
 *  @param[in]     Mask          The bitwise AND mask to apply to the current register value
 *  @param[in]     Data          The bitwise OR mask to apply to the current register value
 *  @param[in]     StdHeader     Header for library and services.
 *
 */
VOID
ModifyCurrSocketPciSingle (
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 Mask,
  IN       UINT32                 Data,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32 LocalPciRegister;
  PCI_ADDR Reg;

  Reg.AddressValue     = MAKE_SBDFO (0, 0, 24, 0, 0);
  Reg.Address.Function = PciAddress->Address.Function;
  Reg.Address.Register = PciAddress->Address.Register;
  LibAmdPciRead (AccessWidth32, Reg, &LocalPciRegister, StdHeader);
  LocalPciRegister &= Mask;
  LocalPciRegister |= Data;
  LibAmdPciWrite (AccessWidth32, Reg, &LocalPciRegister, StdHeader);
}
