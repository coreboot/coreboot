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
 * @e \$Revision: 63425 $   @e \$Date: 2011-12-22 11:24:10 -0600 (Thu, 22 Dec 2011) $
 *
 */
/*
 ******************************************************************************
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
